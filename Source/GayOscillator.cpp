/*
  ==============================================================================

    GayOscillator.cpp
    Created: 16 Jul 2022 11:24:02am
    Author:  Ryan Devens

  ==============================================================================
*/

#include "GayOscillator.h"

GayOscillator::GayOscillator(WaveTableVector& vector)  : waveVector(vector)
{

}

GayOscillator::~GayOscillator(){}

void GayOscillator::prepare(double sampleRate)
{
    mSampleRate = sampleRate;
}

void GayOscillator::noteOn(float vel, float newFreq)
{
    mFrequency = newFreq;
}

void GayOscillator::noteOff(){}

//*******************
// Parameter values set by basic synth operations (note on
void GayOscillator::setFrequency(float freq)
{
    mFrequency = freq;
}

void GayOscillator::setGain(float gain)
{
    mGain = gain;
}

void GayOscillator::reset() noexcept{}

//*****************
// SAMPLE RETRIEVAL
float GayOscillator::getNextSample()
{
    // increments freq param while calculating new phase inc and next sample
    float nextIndex     = _getNextSampleIndex(mFrequency.get());
    
    float nextWavePos   = mWavePosition.get();
    
    float nextSample    = waveVector.getSampleAtIndexAndWavePosition(nextIndex, nextWavePos);
    
    return nextSample * mGain.get();
}

//==============================================================================

WaveTableVector& GayOscillator::getWaveVector()
{
    return waveVector;
}



void GayOscillator::update(float newGain, float frequency, float wavePosition)
{
    mGain.setValue(newGain);
    mFrequency.setOffset(frequency);
    mWavePosition->setValue(wavePosition);
}


//*****************
// PRIVATE FUNCTIONS

float GayOscillator::_getNextSampleIndex(float freq)
{
    int tableSizeOverSampleRate = GPC_CONSTANTS::TABLE_SIZE / oSampleRate;
    
    float phaseIncrement = freq * tableSizeOverSampleRate;
    
    float nextSampleIndex = currentIndex;
    
    currentIndex += phaseIncrement;
    
    return nextSampleIndex;
}
