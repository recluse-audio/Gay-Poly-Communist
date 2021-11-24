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
#include "../Processor/PluginProcessor.h"


class GayVoice : public MPESynthesiserVoice
{
public:
    GayVoice()
    {
        env1.reset();
        env2.reset();

        lfoTable1.createSineTable();
        lfoTable2.createSineTable();

        processorChain.get<osc1Index>().setWaveform(0.4f);
        
        auto& masterGain = processorChain.get<masterGainIndex>();
        masterGain.setGainLinear(0.7f);

        auto& filter = processorChain.get<filterIndex>();
        filter.setMode(juce::dsp::LadderFilter<float>::Mode::LPF24);
        filter.setResonance(0.7f);
        filter.setCutoffFrequencyHz(800.0f);

        updateProcessorValues();
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        env1.setSampleRate(spec.sampleRate);
        env2.setSampleRate(spec.sampleRate);

        lfoTable1.prepare(spec.sampleRate);
        lfoTable1.setFrequency(10.f);

        lfoTable2.prepare(spec.sampleRate);
        lfoTable2.setFrequency(10.f);

        tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);
        processorChain.prepare(spec);
    }

    //==============================================================================
    void noteStarted() override
    {
        auto velocity = getCurrentlyPlayingNote().noteOnVelocity.asUnsignedFloat();
        auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();

        pitch = freqHz;

        processorChain.get<osc1Index>().setFrequency(freqHz, true);
        processorChain.get<osc1Index>().setLevel(velocity);

        env1.noteOn();
        env2.noteOn();

    }

    //==============================================================================
    void notePitchbendChanged() override
    {
        auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();
        pitch = freqHz;
        
    }

    //==============================================================================
    void noteStopped(bool) override
    {
       env1.noteOff();
       env2.noteOff();
    }

    //==============================================================================
    void notePressureChanged() override {}
    void noteTimbreChanged() override {}
    void noteKeyStateChanged() override {}

    //==============================================================================
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        // get audio block that matches this buffer in size, don't get why the tutorial adds this step and we don't just use the tempBlock to create the context but...
        auto block = tempBlock.getSubBlock(0, (size_t)numSamples);

        // clear out garbage
        block.clear();

        // Prepare to write to this 
        dsp::ProcessContextReplacing<float> context(block);
        processorChain.process(context);


        for (int j = 0; j < numSamples; ++j)
        {
            updateEnvelope();
            updateLFO();
            updateProcessorValues();

            for (int channel = 0; channel < block.getNumChannels(); channel++)
            {
                auto blockWrite = block.getChannelPointer(channel);
                blockWrite[j] *= amp;
            }
        }
        
        dsp::AudioBlock<float>(outputBuffer)
            .getSubBlock((size_t)startSample, (size_t)numSamples)
            .add(tempBlock);

    }

    void updateEnvelope()
    {
        envVal1 = env1.getNextSample() * envDepth1.get();
        envVal2 = env2.getNextSample() * envDepth2.get();
    }

    void updateLFO()
    {
        lfoVal1 = lfoTable1.getNextSample() * lfoDepth1.get();
        lfoVal2 = lfoTable2.getNextSample() * lfoDepth2.get();
    }

    void updateProcessorValues()
    {
        amp = envVal1 + (envVal1 * lfoVal1 * ampLFO1.get()) + (envVal2 * lfoVal2 * ampLFO2.get());

        
        /*
            scale envelope output by the mod coef, then map from filtFreq to filtFreq * 4
            lfo then modulates the envScaled filter frequency
        */
        auto filtRange = NormalisableRange<float>(filtFreq.get(), 15000.f, 0.1f, 0.5f);
        auto combinedFilterFreq = (envVal1 * filtEnv1.get()) + (envVal2 * filtEnv2.get());
        combinedFilterFreq = jlimit(0.f, 1.f, combinedFilterFreq);
        auto filtEnvScaled = filtRange.convertFrom0to1(envVal1 * combinedFilterFreq);
        filt = filtEnvScaled + (lfoVal1 * filtEnvScaled * filtLFO1.get()) + (lfoVal2 * filtEnvScaled * filtLFO2.get());
        processorChain.get<filterIndex>().setCutoffFrequencyHz(filt);

        auto pEnv = pitch.get() + (pitch.get() * ((envVal1 * pitchEnv1.get()) + (envVal2 * pitchEnv2.get())));
        auto pVal = (lfoVal1 * pitchLFO1.get()) + (lfoVal2 * pitchLFO2.get());
        freq = pEnv * (pow(2, (pVal * 2.f) / 12.0));

        processorChain.get<osc1Index>().setFrequency(freq);

        auto combinedWaveEnv = (envVal1 * waveEnv1.get()) + (envVal2 * waveEnv2.get());
        auto combinedWaveLFO = (lfoVal1 * waveLFO1.get()) + (lfoVal2 * waveLFO2.get());

        waveTable = wave.get() + combinedWaveEnv + combinedWaveLFO;
        waveTable = jlimit(0.f, 1.f, waveTable);

        processorChain.get<osc1Index>().setWaveform(waveTable);
        
    }

    void update(AudioProcessorValueTreeState& apvts)
    {
        envParam1.attack = apvts.getRawParameterValue("ATTACK 1")->load();
        envParam1.decay = apvts.getRawParameterValue("DECAY 1")->load();
        envParam1.sustain = apvts.getRawParameterValue("SUSTAIN 1")->load();
        envParam1.release = apvts.getRawParameterValue("RELEASE 1")->load();
        env1.setParameters(envParam1);

        envParam2.attack = apvts.getRawParameterValue("ATTACK 2")->load();
        envParam2.decay = apvts.getRawParameterValue("DECAY 2")->load();
        envParam2.sustain = apvts.getRawParameterValue("SUSTAIN 2")->load();
        envParam2.release = apvts.getRawParameterValue("RELEASE 2")->load();
        env2.setParameters(envParam2);

        lfoDepth1 = apvts.getRawParameterValue("LFO 1 Depth")->load();
        lfoRate1 = apvts.getRawParameterValue("LFO 1 Rate")->load();
        lfoTable1.setFrequency(lfoRate1.get());

        lfoDepth2 = apvts.getRawParameterValue("LFO 2 Depth")->load();
        lfoRate2 = apvts.getRawParameterValue("LFO 2 Rate")->load();
        lfoTable2.setFrequency(lfoRate2.get());

        gain = apvts.getRawParameterValue("Gain")->load();
        filtFreq = apvts.getRawParameterValue("Filter Freq")->load();
        wave = apvts.getRawParameterValue("Wavetable")->load();

        ampLFO1 = apvts.getRawParameterValue("Amp LFO 1")->load();
        filtLFO1 = apvts.getRawParameterValue("Filt LFO 1")->load();
        pitchLFO1 = apvts.getRawParameterValue("Pitch LFO 1")->load();
        waveLFO1 = apvts.getRawParameterValue("Wave LFO 1")->load();

        filtEnv1 = apvts.getRawParameterValue("Filt Env 1")->load();
        pitchEnv1 = apvts.getRawParameterValue("Pitch Env 1")->load();
        waveEnv1 = apvts.getRawParameterValue("Wave Env 1")->load();

        ampLFO2 = apvts.getRawParameterValue("Amp LFO 2")->load();
        filtLFO2 = apvts.getRawParameterValue("Filt LFO 2")->load();
        pitchLFO2 = apvts.getRawParameterValue("Pitch LFO 2")->load();
        waveLFO2 = apvts.getRawParameterValue("Wave LFO 2")->load();

        filtEnv2 = apvts.getRawParameterValue("Filt Env 2")->load();
        pitchEnv2 = apvts.getRawParameterValue("Pitch Env 2")->load();
        waveEnv2 = apvts.getRawParameterValue("Wave Env 2")->load();
    }

    WaveTableVector& getTable()
    {
        return processorChain.get<osc1Index>().getWaveVector();
    }

    void loadTables(StringRef tableFilePath)
    {
        processorChain.get<osc1Index>().loadTables(tableFilePath);
    }

private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    enum
    {
        osc1Index,
        filterIndex,
        masterGainIndex
    };

   dsp::ProcessorChain
   <
       GayOscillator<float>, 
       dsp::LadderFilter<float>,
       dsp::Gain<float>
   > processorChain;
   
   Atomic<float> gain{ 0.f };
   Atomic<float> filtFreq{ 400.f };
   Atomic<float> pitch{ 0.f }; // in hz
   Atomic<float> wave{ 0.7f };

   float amp  { 0.f };
   float filt{ 0.f };
   float freq{ 0.f };
   float waveTable{ 0.f };
    

    WaveTable lfoTable1;
    WaveTable lfoTable2;


    Atomic<float> lfoDepth1{ 1.f }; // how much mod will affect a param
    Atomic<float> lfoRate1{ 0.f };
    float lfoVal1{ 0.f }; // sample by sample value of env output (scaled by depth)

    Atomic<float> lfoDepth2{ 1.f }; 
    Atomic<float> lfoRate2{ 0.f };
    float lfoVal2{ 0.f };

    Atomic<float> envDepth1{ 1.f }; // how much mod will affect a param
    float envVal1 { 0.f }; // sample by sample value of env output (scaled by depth)

    Atomic<float> envDepth2{ 1.f }; // how much mod will affect a param
    float envVal2{ 0.f };

    

    /*
        How much a parameter is affected by the given modulator (env or lfo)
    */
    Atomic<float> ampLFO1{ 0.f };
    Atomic<float> ampEnv1{ 1.f };

    Atomic<float> filtLFO1{ 0.f };
    Atomic<float> filtEnv1{ 0.f };

    Atomic<float> pitchLFO1{ 0.5f };
    Atomic<float> pitchEnv1{ 0.5f };

    Atomic<float> waveLFO1   { 0.f };
    Atomic<float> waveEnv1   { 0.f };

    Atomic<float> ampLFO2{ 0.f };
    Atomic<float> ampEnv2{ 1.f };

    Atomic<float> filtLFO2{ 0.f };
    Atomic<float> filtEnv2{ 0.f };

    Atomic<float> pitchLFO2{ 0.5f };
    Atomic<float> pitchEnv2{ 0.5f };

    Atomic<float> waveLFO2{ 0.f };
    Atomic<float> waveEnv2{ 0.f };

    ADSR env1;
    ADSR::Parameters envParam1;

    ADSR env2;
    ADSR::Parameters envParam2;
};
