/*
  ==============================================================================

    GayVoice.h
    Created: 20 Sep 2021 2:35:54pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GaySynth.h"
#include "GayOscillator.h"
#include "GayADSR.h"
#include "../Processor/PluginProcessor.h"


class GayVoice : public MPESynthesiserVoice
{
public:
    GayVoice()
    {
        initMods();
        filtFreq = std::make_unique<GayParam>(GayParam::ParamType::pitch); // rename this to be freq?
        filtDrive = std::make_unique<GayParam>(GayParam::ParamType::gain);
        filtRes = std::make_unique<GayParam>(GayParam::ParamType::gain); // rename this type to be normalized?
    }

    void initMods()
    {
        env1.reset(); env2.reset(); env3.reset();

        lfo1 = std::make_unique<WaveTable>(2048);
        lfo1->createSineTable();
        lfoDepth1 = std::make_unique<GayParam>(GayParam::ParamType::gain);
        lfoRate1 = std::make_unique<GayParam>(GayParam::ParamType::pitch);

        lfo2 = std::make_unique<WaveTable>(2048);
        lfo2->createSineTable();
        lfoDepth2 = std::make_unique<GayParam>(GayParam::ParamType::gain);
        lfoRate2 = std::make_unique<GayParam>(GayParam::ParamType::pitch);


        lfo3 = std::make_unique<WaveTable>(2048);
        lfo3->createSineTable();
        lfoDepth3 = std::make_unique<GayParam>(GayParam::ParamType::gain);
        lfoRate3 = std::make_unique<GayParam>(GayParam::ParamType::pitch);


    }
    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        prepareMods(spec.sampleRate);

        osc1.prepare(spec.sampleRate);
        osc2.prepare(spec.sampleRate);

        filtFreq->prepare(spec.sampleRate);
        filtRes->prepare(spec.sampleRate);
        filtDrive->prepare(spec.sampleRate);

        filtFreq->setValue(400.f);
        filtRes->setValue(0.f);
        filtDrive->setValue(1.f);

        filter.prepare(spec);
        filter.setCutoffFrequencyHz(400.f);
        filter.setMode(juce::dsp::LadderFilter<float>::Mode::LPF24);
    }

    void prepareMods(double sampleRate)
    {
        lfo1->prepare(sampleRate);
        lfo2->prepare(sampleRate);
        lfo3->prepare(sampleRate);

        lfoRate1->prepare(sampleRate);
        lfoRate2->prepare(sampleRate);
        lfoRate3->prepare(sampleRate);

        lfoDepth1->prepare(sampleRate);
        lfoDepth2->prepare(sampleRate);
        lfoDepth3->prepare(sampleRate);

        env1.setSampleRate(sampleRate);
        env1.reset();
        env2.setSampleRate(sampleRate);
        env2.reset();
        env3.setSampleRate(sampleRate);
        env3.reset();

    }

    //==============================================================================
    void noteStarted() override
    {
        auto velocity = getCurrentlyPlayingNote().noteOnVelocity.asUnsignedFloat();
        auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();

        pitch = freqHz;
        osc1.noteOn(velocity, freqHz);
        osc2.noteOn(velocity, freqHz);

        env1.setParameters(envParam1);
        env1.noteOn();

        env2.setParameters(envParam2);
        env2.noteOn();

        env3.setParameters(envParam3);
        env3.noteOn();

    }

    //==============================================================================
    void notePitchbendChanged() override
    {
        auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();
        osc1.setFrequency(freqHz);
        osc2.setFrequency(freqHz);
    }

    //==============================================================================
    void noteStopped(bool allowTailOff) override
    {
       env1.noteOff();
       env2.noteOff();
       env3.noteOff();
    }

    //==============================================================================
    void notePressureChanged() override {}
    void noteTimbreChanged() override {}
    void noteKeyStateChanged() override {}

    //==============================================================================
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        auto blockWrite = outputBuffer.getArrayOfWritePointers();

        for (int sampleIndex = startSample; sampleIndex < numSamples; ++sampleIndex) // start from start sample incase it is not 0 (usually is)
        {
            if (env1.isActive()) // env1 is the "amp" env, so it controls note off (maybe I should name it that?)
            {
                incrementLFOs();
                incrementEnvelopes();
                incrementFilter();

                auto sample = (osc1.getNextSample() + osc2.getNextSample()) * env1.getCurrentValue();

                for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
                {
                    // TO DO: Pan settings for osc?
                    blockWrite[channel][sampleIndex] = sample;
                }
            }
            else
            {
                clearCurrentNote();
                break;
            }
        }

        auto block = dsp::AudioBlock<float>(outputBuffer);
        auto blockToUse = block.getSubBlock((size_t)startSample, (size_t)numSamples);
        auto contextToUse = dsp::ProcessContextReplacing<float>(blockToUse);
        
        filter.setCutoffFrequencyHz(filtFreq->getCurrentValue());
        filter.setDrive(filtDrive->getCurrentValue());
        /*
            TO DO: fix GayParam so I can do things like init 'val' and other stuff
        */
        filter.setResonance(jlimit(0.f, 1.f, filtRes->getCurrentValue()));
        filter.process(contextToUse);
    }


    void incrementLFOs()
    {
        lfo1->setFrequency(lfoRate1->getNextValue());
        lfo1->setGain(lfoDepth1->getNextValue());
        lfo1->getNextSample();

        lfo2->setFrequency(lfoRate2->getNextValue());
        lfo2->setGain(lfoDepth2->getNextValue());
        lfo2->getNextSample();

        lfo3->setFrequency(lfoRate3->getNextValue());
        lfo3->setGain(lfoDepth3->getNextValue());
        lfo3->getNextSample();
    }

    void incrementEnvelopes()
    {
        env1.getNextSample();
        env2.getNextSample();
        env3.getNextSample();
    }

    void incrementFilter()
    {
        filtFreq->getNextValue();
        filtDrive->getNextValue();
        filtRes->getNextValue();
    }

    void update(AudioProcessorValueTreeState& apvts)
    {  
        //////////////////// VOICE ////////////////////
        
        // set value on filter params (these are GayParam(s) that exist in the voice class)
        filtFreq->setValue(apvts.getRawParameterValue("Filter Freq")->load());
        filtFreq->setLFOScale(apvts.getRawParameterValue("Filter LFO Scale")->load());
        filtFreq->setEnvScale(apvts.getRawParameterValue("Filter Env Scale")->load());

        filtDrive->setValue(apvts.getRawParameterValue("Filter Drive")->load());
        filtDrive->setLFOScale(apvts.getRawParameterValue("Drive LFO Scale")->load());
        filtDrive->setEnvScale(apvts.getRawParameterValue("Drive Env Scale")->load());

        filtRes->setValue(apvts.getRawParameterValue("Filter Res")->load());
        filtRes->setLFOScale(apvts.getRawParameterValue("Res LFO Scale")->load());
        filtRes->setEnvScale(apvts.getRawParameterValue("Res Env Scale")->load());



        // assign filter modulation sources (occurs in voice class not oscillator)
        auto fLFO = apvts.getRawParameterValue("Filter LFO Source")->load();
        auto fEnv = apvts.getRawParameterValue("Filter Env Source")->load();

        auto dLFO = apvts.getRawParameterValue("Drive LFO Source")->load();
        auto dEnv = apvts.getRawParameterValue("Drive Env Source")->load();

        auto rLFO = apvts.getRawParameterValue("Res LFO Source")->load();
        auto rEnv = apvts.getRawParameterValue("Res Env Source")->load();

        assignFilterMods(fLFO, dLFO, rLFO, fEnv, dEnv, rEnv);

        // assign LFO modulation sources (envelopes only)
        auto lEnv1 = apvts.getRawParameterValue("LFO Rate Env Source 1")->load();
        auto lEnv2 = apvts.getRawParameterValue("LFO Rate Env Source 2")->load();
        auto lEnv3 = apvts.getRawParameterValue("LFO Rate Env Source 3")->load();

        auto depthEnv1 = apvts.getRawParameterValue("LFO Depth Env Source 1")->load();
        auto depthEnv2 = apvts.getRawParameterValue("LFO Depth Env Source 2")->load();
        auto depthEnv3 = apvts.getRawParameterValue("LFO Depth Env Source 3")->load();
        assignLFOMods(lEnv1, lEnv2, lEnv3, depthEnv1, depthEnv2, depthEnv3);

        // LFO Params (in voice class)
        auto aRate1 = apvts.getRawParameterValue("LFO Rate 1")->load();
        auto rateScale1 = apvts.getRawParameterValue("LFO Rate Env Scale 1")->load();
        lfoRate1->setValue(aRate1);
        lfoRate1->setEnvScale(rateScale1);

        auto gDepth1 = apvts.getRawParameterValue("LFO Depth 1")->load();
        auto gainScale1 = apvts.getRawParameterValue("LFO Depth Env Scale 1")->load();
        lfoDepth1->setValue(gDepth1);
        lfoDepth1->setEnvScale(gainScale1);

        auto aRate2 = apvts.getRawParameterValue("LFO Rate 2")->load();
        auto rateScale2 = apvts.getRawParameterValue("LFO Rate Env Scale 2")->load();
        lfoRate2->setValue(aRate2);
        lfoRate2->setEnvScale(rateScale2);

        auto gDepth2 = apvts.getRawParameterValue("LFO Depth 2")->load();
        auto gainScale2 = apvts.getRawParameterValue("LFO Depth Env Scale 2")->load();
        lfoDepth2->setValue(gDepth2);
        lfoDepth2->setEnvScale(gainScale2);

        auto aRate3 = apvts.getRawParameterValue("LFO Rate 3")->load();
        auto rateScale3 = apvts.getRawParameterValue("LFO Rate Env Scale 3")->load();
        lfoRate3->setValue(aRate3);
        lfoRate3->setEnvScale(rateScale3);

        auto gDepth3 = apvts.getRawParameterValue("LFO Depth 3")->load();
        auto gainScale3 = apvts.getRawParameterValue("LFO Depth Env Scale 3")->load();
        lfoDepth3->setValue(gDepth3);
        lfoDepth3->setEnvScale(gainScale1);

        // Assign Env params (in voice class)
        auto atk1 = apvts.getRawParameterValue("ATTACK 1")->load();
        auto dec1 = apvts.getRawParameterValue("DECAY 1")->load();
        auto sus1 = apvts.getRawParameterValue("SUSTAIN 1")->load();
        auto rel1 = apvts.getRawParameterValue("RELEASE 1")->load();
        envParam1 = GayADSR::Parameters(atk1, dec1, sus1, rel1);

        auto atk2 = apvts.getRawParameterValue("ATTACK 2")->load();
        auto dec2 = apvts.getRawParameterValue("DECAY 2")->load();
        auto sus2 = apvts.getRawParameterValue("SUSTAIN 2")->load();
        auto rel2 = apvts.getRawParameterValue("RELEASE 2")->load();
        envParam2 = GayADSR::Parameters(atk2, dec2, sus2, rel2);

        auto atk3 = apvts.getRawParameterValue("ATTACK 3")->load();
        auto dec3 = apvts.getRawParameterValue("DECAY 3")->load();
        auto sus3 = apvts.getRawParameterValue("SUSTAIN 3")->load();
        auto rel3 = apvts.getRawParameterValue("RELEASE 3")->load();
        envParam3 = GayADSR::Parameters(atk3, dec3, sus3, rel3);

        //////////////// OSCILLATORS ///////////////
        
        // oscillator 1 params
        auto g1 = apvts.getRawParameterValue("Gain 1")->load();
        auto gLFOScale1 = apvts.getRawParameterValue("Gain 1 LFO Scale")->load();
        auto gEnvScale1 = apvts.getRawParameterValue("Gain 1 Env Scale")->load();

        auto w1 = apvts.getRawParameterValue("Wave 1 Position")->load();
        auto wLFOScale1 = apvts.getRawParameterValue("Wave 1 LFO Scale")->load();
        auto wEnvScale1 = apvts.getRawParameterValue("Wave 1 Env Scale")->load();

        auto p1 = apvts.getRawParameterValue("Pitch 1")->load();
        auto pLFOScale1 = apvts.getRawParameterValue("Pitch 1 LFO Scale")->load();
        auto pEnvScale1 = apvts.getRawParameterValue("Pitch 1 Env Scale")->load();

        osc1.update(g1, gLFOScale1, gEnvScale1,
            w1, wLFOScale1, wEnvScale1, p1, pLFOScale1, pEnvScale1);

        // oscillator 1 modulation sources
        auto gLFO1 = apvts.getRawParameterValue("Gain 1 LFO Source")->load();
        auto gEnv1 = apvts.getRawParameterValue("Gain 1 Env Source")->load();

        auto wLFO1 = apvts.getRawParameterValue("Wave 1 LFO Source")->load();
        auto wEnv1 = apvts.getRawParameterValue("Wave 1 Env Source")->load();

        auto pLFO1 = apvts.getRawParameterValue("Pitch 1 LFO Source")->load();
        auto pEnv1 = apvts.getRawParameterValue("Pitch 1 Env Source")->load();

        assignOscMods(osc1, gLFO1, wLFO1, pLFO1, gEnv1, wEnv1, pEnv1);

        // oscillator 2 params
        auto g2 = apvts.getRawParameterValue("Gain 2")->load();
        auto gLFOScale2 = apvts.getRawParameterValue("Gain 2 LFO Scale")->load();
        auto gEnvScale2 = apvts.getRawParameterValue("Gain 2 Env Scale")->load();

        auto w2 = apvts.getRawParameterValue("Wave 2 Position")->load();
        auto wLFOScale2 = apvts.getRawParameterValue("Wave 2 LFO Scale")->load();
        auto wEnvScale2 = apvts.getRawParameterValue("Wave 2 Env Scale")->load();

        auto p2 = apvts.getRawParameterValue("Pitch 2")->load();
        auto pLFOScale2 = apvts.getRawParameterValue("Pitch 2 LFO Scale")->load();
        auto pEnvScale2 = apvts.getRawParameterValue("Pitch 2 Env Scale")->load();

        osc2.update(g2, gLFOScale2, gEnvScale2,
            w2, wLFOScale2, wEnvScale2, p2, pLFOScale2, pEnvScale2);

        // oscillator 2 modulation sources
        auto gLFO2 = apvts.getRawParameterValue("Gain 2 LFO Source")->load();
        auto gEnv2 = apvts.getRawParameterValue("Gain 2 Env Source")->load();

        auto wLFO2 = apvts.getRawParameterValue("Wave 2 LFO Source")->load();
        auto wEnv2 = apvts.getRawParameterValue("Wave 2 Env Source")->load();

        auto pLFO2 = apvts.getRawParameterValue("Pitch 2 LFO Source")->load();
        auto pEnv2 = apvts.getRawParameterValue("Pitch 2 Env Source")->load();

        assignOscMods(osc2, gLFO2, wLFO2, pLFO2, gEnv2, wEnv2, pEnv2);
    }

    WaveTableVector& getTable(int oscNumber)
    {
        if (oscNumber == 1)
        {
            return osc1.getWaveVector();
        }
        if (oscNumber == 2)
        {
            return osc2.getWaveVector();
        }

    }

    // rethinking this oscNum business and the waveMenu overall
    void loadTables(StringRef tableFilePath, int oscNum)
    {
        if (oscNum == 1)
        {
            osc1.loadTables(tableFilePath);
        }
        else
        {
            osc2.loadTables(tableFilePath);
        }
    }

   // void incrementFilter()
    // assigning modulators to the oscillators
    void assignOscMods(GayOscillator& osc, int gainLFO, int waveLFO, int pitchLFO, int gainEnv, int waveEnv, int pitchEnv)
    {
        auto gain = GayParam::ParamType::gain;
        auto wave = GayParam::ParamType::wave;
        auto pitch = GayParam::ParamType::pitch;

        switch (gainLFO)
        {
            case 0: osc.setNoLFO(gain); break;
            case 1: osc.assignLFO(lfo1.get(), gain); break;
            case 2: osc.assignLFO(lfo2.get(), gain); break;
            case 3: osc.assignLFO(lfo3.get(), gain); break;
        }

        switch (waveLFO)
        {
            case 0: osc.setNoLFO(wave); break;
            case 1: osc.assignLFO(lfo1.get(), wave); break;
            case 2: osc.assignLFO(lfo2.get(), wave); break;
            case 3: osc.assignLFO(lfo3.get(), wave); break;
        }

        switch (pitchLFO)
        {
            case 0: osc.setNoLFO(pitch); break;
            case 1: osc.assignLFO(lfo1.get(), pitch); break;
            case 2: osc.assignLFO(lfo2.get(), pitch); break;
            case 3: osc.assignLFO(lfo3.get(), pitch); break;
        }

        // assinging envelopes to oscillators first, then to filter that is held by the voice itself
        switch (gainEnv)
        {
        case 0: osc.setNoEnv(gain); break;
        case 1: osc.assignEnvelope(&env1, gain); break;
        case 2: osc.assignEnvelope(&env2, gain); break;
        case 3: osc.assignEnvelope(&env3, gain); break;
        }

        switch (waveEnv)
        {
        case 0: osc.setNoEnv(wave); break;
        case 1: osc.assignEnvelope(&env1, wave); break;
        case 2: osc.assignEnvelope(&env2, wave); break;
        case 3: osc.assignEnvelope(&env3, wave); break;
        }

        switch (pitchEnv)
        {
        case 0: osc.setNoEnv(pitch); break;
        case 1: osc.assignEnvelope(&env1, pitch); break;
        case 2: osc.assignEnvelope(&env2, pitch); break;
        case 3: osc.assignEnvelope(&env3, pitch); break;
        }
            
    }

    void assignFilterMods(int filtLFO, int driveLFO, int resLFO, int filtEnv, int driveEnv, int resEnv)
    {
        // These are held in the GayVoice class so it calls the assign functions on the GayParam directly
        switch (filtLFO)
        {
        case 0: filtFreq->setNoLFO(); break;
        case 1: filtFreq->assignLFO(lfo1.get()); break;
        case 2: filtFreq->assignLFO(lfo2.get()); break;
        case 3: filtFreq->assignLFO(lfo3.get()); break;
        }

        switch (resLFO)
        {
        case 0: filtRes->setNoLFO(); break;
        case 1: filtRes->assignLFO(lfo1.get()); break;
        case 2: filtRes->assignLFO(lfo2.get()); break;
        case 3: filtRes->assignLFO(lfo3.get()); break;
        }

        // local filter parameters
        switch (filtEnv)
        {
        case 0: filtFreq->setNoEnv(); break;
        case 1: filtFreq->assignEnvelope(&env1); break;
        case 2: filtFreq->assignEnvelope(&env2); break;
        case 3: filtFreq->assignEnvelope(&env3); break;
        }

        switch (resEnv)
        {
        case 0: filtRes->setNoEnv(); break;
        case 1: filtRes->assignEnvelope(&env1); break;
        case 2: filtRes->assignEnvelope(&env2); break;
        case 3: filtRes->assignEnvelope(&env3); break;
        }

    }
    
    void assignLFOMods(int rateEnv1, int rateEnv2, int rateEnv3, int depthEnv1, int depthEnv2, int depthEnv3)
    {
        switch (rateEnv1)
        {
        case 0: lfoRate1->setNoEnv(); break;
        case 1: lfoRate1->assignEnvelope(&env1); break;
        case 2: lfoRate1->assignEnvelope(&env2); break;
        case 3: lfoRate1->assignEnvelope(&env3); break;
        }

        switch (rateEnv2)
        {
        case 0: lfoRate2->setNoEnv(); break;
        case 1: lfoRate2->assignEnvelope(&env1); break;
        case 2: lfoRate2->assignEnvelope(&env2); break;
        case 3: lfoRate2->assignEnvelope(&env3); break;
        }

        switch (rateEnv3)
        {
        case 0: lfoRate3->setNoEnv(); break;
        case 1: lfoRate3->assignEnvelope(&env1); break;
        case 2: lfoRate3->assignEnvelope(&env2); break;
        case 3: lfoRate3->assignEnvelope(&env3); break;
        }
        // local filter parameters
        switch (depthEnv1)
        {
        case 0: lfoDepth1->setNoEnv(); break;
        case 1: lfoDepth1->assignEnvelope(&env1); break;
        case 2: lfoDepth1->assignEnvelope(&env2); break;
        case 3: lfoDepth1->assignEnvelope(&env3); break;
        }

        switch (depthEnv2)
        {
        case 0: lfoDepth2->setNoEnv(); break;
        case 1: lfoDepth2->assignEnvelope(&env1); break;
        case 2: lfoDepth2->assignEnvelope(&env2); break;
        case 3: lfoDepth2->assignEnvelope(&env3); break;
        }

        switch (depthEnv3)
        {
        case 0: lfoDepth3->setNoEnv(); break;
        case 1: lfoDepth3->assignEnvelope(&env1); break;
        case 2: lfoDepth3->assignEnvelope(&env2); break;
        case 3: lfoDepth3->assignEnvelope(&env3); break;
        }
    }
private:
   dsp::LadderFilter<float> filter;

   GayOscillator osc1, osc2;
    
   std::unique_ptr<WaveTable> lfo1, lfo2, lfo3;
   
   GayADSR env1, env2, env3; // keeping in the voice (not synth) because this needs to change only when a note is triggered
   GayADSR::Parameters envParam1, envParam2, envParam3;

   std::unique_ptr<GayParam> filtFreq, filtRes, filtDrive;
   std::unique_ptr<GayParam> lfoRate1, lfoRate2, lfoRate3;
   std::unique_ptr<GayParam> lfoDepth1, lfoDepth2, lfoDepth3;

   double glideTime = 0.01;


   float pitch = 0.f;
};
