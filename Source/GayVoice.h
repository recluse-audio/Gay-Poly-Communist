/*
  ==============================================================================

    GayVoice.h
    Created: 20 Sep 2021 2:35:54pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GayADSR.h"
#include "ModFriendlyParams.h"
#include "ObjectID.h"

class GayParam;
class GayOscillator;
class WaveTableVector;

class GayVoice : public MPESynthesiserVoice
{
public:

    
    
    GayVoice(WaveTableVector& oscVector1, WaveTableVector& oscVector2,
             WaveTableVector& lfoVector1, WaveTableVector& lfoVector2, WaveTableVector& lfoVector3);

    ~GayVoice();
    void initParams();
    
    void resetEnvs();
    
    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec);

    void prepareMods(double sampleRate);

    //==============================================================================
    void noteStarted() override;

    //==============================================================================
    void notePitchbendChanged() override;

    //==============================================================================
    void noteStopped(bool allowTailOff) override;

    //==============================================================================
    void notePressureChanged() override ;
    void noteTimbreChanged() override ;
    void noteKeyStateChanged() override ;

    //==============================================================================
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;


    void incrementOscillators();
    
    void incrementLFOs();

    void incrementEnvelopes();

    void incrementFilter();

    void update(AudioProcessorValueTreeState& apvts);

    void updateFilterMode(int mode);
    
    


   // void incrementFilter()
    // assigning modulators to the the parameters
    void assignOscMods(GayOscillator* osc, int gainLFO, int waveLFO, int freqLFO, int gainEnv, int waveEnv, int freqEnv);

    void assignFilterMods(int filtLFO, int driveLFO, int resLFO, int filtEnv, int driveEnv, int resEnv);
    
    // Envelopes modifying lfo's
    void assignLFOMods(int rateEnv1, int rateEnv2, int rateEnv3, int depthEnv1, int depthEnv2, int depthEnv3);

private:
   dsp::LadderFilter<float> filter;
   bool isFiltering = true;

   std::unique_ptr<GayOscillator> osc1, osc2, lfo1, lfo2, lfo3;
   
   GayADSR env1, env2, env3;
   GayADSR::Parameters envParam1, envParam2, envParam3;
    
   std::unique_ptr<GayParam> osc1Freq, osc2Freq;
   std::unique_ptr<GayParam> osc1Gain, osc2Gain;
   std::unique_ptr<GayParam> osc1WavePos, osc2WavePos;
   std::unique_ptr<GayParam> filtFreq, filtRes, filtDrive;
   std::unique_ptr<GayParam> lfoRate1, lfoRate2, lfoRate3;
   std::unique_ptr<GayParam> lfoDepth1, lfoDepth2, lfoDepth3;


   double glideTime = 0.01;

   float pitch = 0.f;
    
    
    
};
