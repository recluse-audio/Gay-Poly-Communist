/*
  ==============================================================================

    GayOscillator.h
    Created: 20 Sep 2021 2:35:36pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WaveTableVector.h"
#include "GayADSR.h"

//==============================================================================
/*
    This should only retrieve samples from the waveVector
 */

class GayOscillator
{
public:

    //==============================================================================
    // each oscillator
    GayOscillator(WaveTableVector& vector);

    ~GayOscillator();

    void prepare(double sampleRate);

    void noteOn(float vel, float freq);

    void noteOff();
 
    
    //***************
    // SETTERS
    void setGain(float gain);
    void setFrequency(float freq);
    void setWavePosition(float wavePosition);
    
    

    void reset() noexcept;

    //*****************
    // SAMPLE RETRIEVAL
    float getNextSample();
    float getCurrentSample();

    //==============================================================================

    // Directly change value by slider change
    void updateGainOffset(float newOffset);
    void updateWaveOffset(float newOffset);
    void updateFreqOffset(float newOffset);
    
    // Depth of LFO modulation (set by sliders)
    void updateGainLFODepth(float newDepth);
    void updateWaveLFODepth(float newDepth);
    void updateFreqLFODepth(float newDepth);
    
    // Depth of Envelope modulation (set by sliders)
    void updateGainEnvDepth(float newDepth);
    void updateWaveEnvDepth(float newDepth);
    void updateFreqEnvDepth(float newDepth);
    
    // Assign modulating LFO to a parameter
    void assignGainLFO(GayOscillator* lfo);
    void assignWaveLFO(GayOscillator* lfo);
    void assignFreqLFO(GayOscillator* lfo);
    
    // Assign modulating envelope to a parameter
    void assignGainEnvelope(GayADSR* env);
    void assignWaveEnvelope(GayADSR* env);
    void assignFreqEnvelope(GayADSR* env);


    
private:
    WaveTableVector& waveVector;
    double glideTime = 0.1;
    double mSampleRate = -1;
    float currentIndex = 0;
    
    // Values set by MIDI note on, will be offset by sliders and modulated by lfo's and envs
    juce::Atomic<float> noteOnGain         { 0.f };
    juce::Atomic<float> noteOnFreq         { 0.f };
    
    // Directly change value by slider change
    juce::SmoothedValue<float> gainOffset;
    juce::SmoothedValue<float> waveOffset;
    juce::SmoothedValue<float> freqOffset;
    
    // Depth of LFO modulation (set by sliders)
    juce::SmoothedValue<float> gainLFODepth;
    juce::SmoothedValue<float> waveLFODepth;
    juce::SmoothedValue<float> freqLFODepth;
    
    // Depth of Envelope modulation (set by sliders)
    juce::SmoothedValue<float> gainEnvDepth;
    juce::SmoothedValue<float> waveEnvDepth;
    juce::SmoothedValue<float> freqEnvDepth;
    
    juce::Atomic<float> currentGain   { 0.f };
    juce::Atomic<float> currentFreq   { 0.f };
    juce::Atomic<float> currentWave   { 0.f };
    juce::Atomic<float> currentSample { 0.f };

    
    // Oscillators modulating parameters of this oscillator
    GayOscillator* gainLFO;
    GayOscillator* waveLFO;
    GayOscillator* freqLFO;
    
    // Envelopes modulating parameters of this oscillator
    GayADSR* gainEnv;
    GayADSR* waveEnv;
    GayADSR* freqEnv;


    
    
//**************
// PRIVATE FUNCTIONS
    
    void _incrementParameters();
    
    // Applies current modulations and offsets to the given parameter
    void _applyGainModulation();
    void _applyFreqModulation();
    void _applyWaveModulation();
    
    // Calculate the next waveform index based on frequency passed in
    float _getNextSampleIndex(float freq);
    

    

};

