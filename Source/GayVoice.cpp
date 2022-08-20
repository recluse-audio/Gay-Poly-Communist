/*
  ==============================================================================

    GayVoice.cpp
    Created: 16 Jul 2022 11:47:44am
    Author:  Ryan Devens

  ==============================================================================
*/

#include "GayVoice.h"
#include "GayParam.h"
#include "GayOscillator.h"
#include "GayADSR.h"


GayVoice::GayVoice(WaveTableVector& oscVector1, WaveTableVector& oscVector2,
         WaveTableVector& lfoVector1, WaveTableVector& lfoVector2, WaveTableVector& lfoVector3)
{
    osc1 = std::make_unique<GayOscillator>(oscVector1);
    osc2 = std::make_unique<GayOscillator>(oscVector2);
    lfo1 = std::make_unique<GayOscillator>(lfoVector1);
    lfo2 = std::make_unique<GayOscillator>(lfoVector2);
    lfo3 = std::make_unique<GayOscillator>(lfoVector3);

    initParams();
    resetEnvs();

}

GayVoice::~GayVoice()
{
    
}

void GayVoice::initParams()
{
    osc1Freq = std::make_unique<GayParam>(GayParam::ParamType::Frequency);
    osc1Gain = std::make_unique<GayParam>(GayParam::ParamType::Gain);
    osc1WavePos = std::make_unique<GayParam>(GayParam::ParamType::WavePosition);
    
    osc2Freq = std::make_unique<GayParam>(GayParam::ParamType::Frequency);
    osc2Gain = std::make_unique<GayParam>(GayParam::ParamType::Gain);
    osc2WavePos = std::make_unique<GayParam>(GayParam::ParamType::WavePosition);
    
    lfoDepth1 = std::make_unique<GayParam>(GayParam::ParamType::Gain);
    lfoRate1 = std::make_unique<GayParam>(GayParam::ParamType::Frequency);
    
    lfoDepth2 = std::make_unique<GayParam>(GayParam::ParamType::Gain);
    lfoRate2 = std::make_unique<GayParam>(GayParam::ParamType::Frequency);
    
    lfoDepth3 = std::make_unique<GayParam>(GayParam::ParamType::Gain);
    lfoRate3 = std::make_unique<GayParam>(GayParam::ParamType::Frequency);
    
    filtFreq = std::make_unique<GayParam>(GayParam::ParamType::Frequency); // rename this to be freq?
    filtDrive = std::make_unique<GayParam>(GayParam::ParamType::Gain);
    filtRes = std::make_unique<GayParam>(GayParam::ParamType::Gain); // rename this type to be normalized?
}

void GayVoice::resetEnvs()
{
    env1.reset(); env2.reset(); env3.reset();
}

//==============================================================================
void GayVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    prepareMods(spec.sampleRate);

    osc1->prepare(spec.sampleRate);
    osc2->prepare(spec.sampleRate);

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

void GayVoice::prepareMods(double sampleRate)
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
void GayVoice::noteStarted()
{
    auto velocity = getCurrentlyPlayingNote().noteOnVelocity.asUnsignedFloat();
    auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();

    pitch = freqHz;

    env1.noteOn();
    env2.noteOn();
    env3.noteOn();

    osc1->noteOn(velocity, freqHz);
    osc2->noteOn(velocity, freqHz);
}

//==============================================================================
void GayVoice::notePitchbendChanged()
{
    auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();
    osc1->setFrequency(freqHz);
    osc2->setFrequency(freqHz);
}

//==============================================================================
void GayVoice::noteStopped(bool allowTailOff)
{
   env1.noteOff();
   env2.noteOff();
   env3.noteOff();
}

//==============================================================================
void GayVoice::notePressureChanged()  {}
void GayVoice::noteTimbreChanged()  {}
void GayVoice::noteKeyStateChanged()  {}

//==============================================================================
void GayVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    auto blockWrite = outputBuffer.getArrayOfWritePointers();

    for (int sampleIndex = startSample; sampleIndex < numSamples; ++sampleIndex) // start from start sample incase it is not 0 (usually is)
    {
        if (env1.isActive()) // env1 is the "amp" env, so it controls note off (maybe I should name it that?)
        {
            incrementLFOs();
            incrementEnvelopes();
            incrementFilter();

            auto sample = (osc1->getNextSample() + osc2->getNextSample()) * env1.getCurrentValue();
            
            for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
            {
                // TO DO: Pan settings for osc?
                blockWrite[channel][sampleIndex] += sample * 0.3f;
            }
        }
        else
        {
           // Not only does the voice release with this 'clearCurrentNote()' commented out
           // It resolves a clicking noise I was dealing with
           // How does this work?
            
           // clearCurrentNote();
            break;
        }
    }
    if (isFiltering)
    {
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
    
}


void GayVoice::incrementLFOs()
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

void GayVoice::incrementEnvelopes()
{
    env1.getNextSample();
    env2.getNextSample();
    env3.getNextSample();
}

void GayVoice::incrementFilter()
{
    filtFreq->getNextValue();
    filtDrive->getNextValue();
    filtRes->getNextValue();
}

void GayVoice::update(AudioProcessorValueTreeState& apvts)
{
    //////////////////// VOICE ////////////////////
    auto filterMode = apvts.getRawParameterValue("Filter Mode")->load();
    updateFilterMode(filterMode);

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

    env1.setParameters(envParam1);
    env2.setParameters(envParam2);
    env3.setParameters(envParam3);

    //////////////// OSCILLATORS ///////////////
    
    // oscillator 1 params
    osc1Gain->setValue(apvts.getRawParameterValue("Gain 1")->load());
    osc1Gain->setLFOScale(apvts.getRawParameterValue("Gain 1 LFO Scale")->load());
    osc1Gain->setEnvScale(apvts.getRawParameterValue("Gain 1 Env Scale")->load());

    osc1WavePos->setValue(apvts.getRawParameterValue("Wave 1 Position")->load());
    osc1WavePos->setLFOScale(apvts.getRawParameterValue("Wave 1 LFO Scale")->load());
    osc1WavePos->setEnvScale(apvts.getRawParameterValue("Wave 1 Env Scale")->load());

    osc1Freq->setOffset(apvts.getRawParameterValue("Pitch 1")->load());
    osc1Freq->setLFOScale(apvts.getRawParameterValue("Pitch 1 LFO Scale")->load());
    osc1Freq->setEnvScale(apvts.getRawParameterValue("Pitch 1 Env Scale")->load());


    

    // oscillator 1 modulation sources
    auto gLFO1 = apvts.getRawParameterValue("Gain 1 LFO Source")->load();
    auto gEnv1 = apvts.getRawParameterValue("Gain 1 Env Source")->load();

    auto wLFO1 = apvts.getRawParameterValue("Wave 1 LFO Source")->load();
    auto wEnv1 = apvts.getRawParameterValue("Wave 1 Env Source")->load();

    auto pLFO1 = apvts.getRawParameterValue("Pitch 1 LFO Source")->load();
    auto pEnv1 = apvts.getRawParameterValue("Pitch 1 Env Source")->load();


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



    // oscillator 2 modulation sources
    auto gLFO2 = apvts.getRawParameterValue("Gain 2 LFO Source")->load();
    auto gEnv2 = apvts.getRawParameterValue("Gain 2 Env Source")->load();

    auto wLFO2 = apvts.getRawParameterValue("Wave 2 LFO Source")->load();
    auto wEnv2 = apvts.getRawParameterValue("Wave 2 Env Source")->load();

    auto pLFO2 = apvts.getRawParameterValue("Pitch 2 LFO Source")->load();
    auto pEnv2 = apvts.getRawParameterValue("Pitch 2 Env Source")->load();

    assignOscMods(osc2.get(), gLFO2, wLFO2, pLFO2, gEnv2, wEnv2, pEnv2);
}

void GayVoice::updateFilterMode(int mode)
{
    switch (mode)
    {
    case 0:
        {
            isFiltering = true;
            filter.setMode(juce::dsp::LadderFilter<float>::Mode::HPF24);
        }
        break;

    case 1:
        {
            isFiltering = true;
            filter.setMode(dsp::LadderFilter<float>::Mode::LPF24);
        }
        break;

    case 2:
        isFiltering = false;
        break;
    }
}




// void incrementFilter()
// assigning modulators to the oscillators
void GayVoice::assignOscMods(GayVoice::OscillatorIds oscId, int gainLFO, int waveLFO, int pitchLFO, int gainEnv, int waveEnv, int pitchEnv)
{
    
    GayOscillator* osc = nullptr;
    
    switch(oscId)
    {
        case GayVoice::OscillatorIds::Oscillator1:
            osc = osc1.get(); break;
        case GayVoice::OscillatorIds::Oscillator2:
            osc = osc2.get(); break;
        case GayVoice::OscillatorIds::LFO1:
            osc = lfo1.get(); break;
        case GayVoice::OscillatorIds::LFO2:
            osc = lfo2.get(); break;
        case GayVoice::OscillatorIds::LFO3:
            osc = lfo3.get(); break;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    auto gain = GayParam::ParamType::Gain;
    auto wave = GayParam::ParamType::WavePosition;
    auto freq = GayParam::ParamType::Frequency;

    switch (gainLFO)
    {
        case 0: osc->setNoLFO(gain); break;
        case 1: osc->assignLFO(lfo1.get(), gain); break;
        case 2: osc->assignLFO(lfo2.get(), gain); break;
        case 3: osc->assignLFO(lfo3.get(), gain); break;
    }

    switch (waveLFO)
    {
        case 0: osc->setNoLFO(wave); break;
        case 1: osc->assignLFO(lfo1.get(), wave); break;
        case 2: osc->assignLFO(lfo2.get(), wave); break;
        case 3: osc->assignLFO(lfo3.get(), wave); break;
    }

    switch (pitchLFO)
    {
        case 0: osc->setNoLFO(freq); break;
        case 1: osc->assignLFO(lfo1.get(), freq); break;
        case 2: osc->assignLFO(lfo2.get(), freq); break;
        case 3: osc->assignLFO(lfo3.get(), freq); break;
    }

    // assinging envelopes to oscillators first, then to filter that is held by the voice itself
    switch (gainEnv)
    {
    case 0: osc->setNoEnv(gain); break;
    case 1: osc->assignEnvelope(&env1, gain); break;
    case 2: osc->assignEnvelope(&env2, gain); break;
    case 3: osc->assignEnvelope(&env3, gain); break;
    }

    switch (waveEnv)
    {
    case 0: osc->setNoEnv(wave); break;
    case 1: osc->assignEnvelope(&env1, wave); break;
    case 2: osc->assignEnvelope(&env2, wave); break;
    case 3: osc->assignEnvelope(&env3, wave); break;
    }

    switch (pitchEnv)
    {
    case 0: osc->setNoEnv(freq); break;
    case 1: osc->assignEnvelope(&env1, freq); break;
    case 2: osc->assignEnvelope(&env2, freq); break;
    case 3: osc->assignEnvelope(&env3, freq); break;
    }
        
}

void GayVoice::assignFilterMods(int filtLFO, int driveLFO, int resLFO, int filtEnv, int driveEnv, int resEnv)
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

// Envelopes modifying lfo's
void GayVoice::assignLFOMods(int rateEnv1, int rateEnv2, int rateEnv3, int depthEnv1, int depthEnv2, int depthEnv3)
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
