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
    void setFrequency(float freq);
    void setGain(float gain);
    void setWavePosition(float wavePosition);
    
    

    void reset() noexcept;

    //*****************
    // SAMPLE RETRIEVAL
    float getNextSample();

    //==============================================================================


    void update(float newGain, float frequency, float wavePosition);
    
    
private:
    WaveTableVector& waveVector;
    double glideTime = 0.1;
    double mSampleRate = -1;
    float currentIndex = 0;
    
    
    juce::Atomic<float> mGain         { 0.f };
    juce::Atomic<float> mWavePosition { 0.f };
    juce::Atomic<float> mFrequency    { 0.f };
    
//    juce::Atomic<float> mGainOffset         { 0.f };
//    juce::Atomic<float> mWavePositionOffset { 0.f };
//    juce::Atomic<float> mFrequencyOffset    { 0.f };
    
//**************
// PRIVATE FUNCTIONS
    
    float _getNextSampleIndex(float freq);
    
    

};

