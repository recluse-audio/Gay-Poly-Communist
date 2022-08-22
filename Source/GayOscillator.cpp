/*
  ==============================================================================

    GayOscillator.cpp
    Created: 16 Jul 2022 11:24:02am
    Author:  Ryan Devens

  ==============================================================================
*/

#include "GayOscillator.h"
#include <cmath>

GayOscillator::GayOscillator(WaveTableVector& vector)  : waveVector(vector)
{

}

GayOscillator::~GayOscillator(){}

void GayOscillator::prepare(double sampleRate)
{
    mSampleRate = sampleRate;
    
    gainOffset.reset(sampleRate, 0.1);
    waveOffset.reset(sampleRate, 0.1);
    freqOffset.reset(sampleRate, 0.1);
    
    gainLFODepth.reset(sampleRate, 0.1);
    waveLFODepth.reset(sampleRate, 0.1);
    freqLFODepth.reset(sampleRate, 0.1);
    
    gainEnvScale.reset(sampleRate, 0.1);
    waveEnvScale.reset(sampleRate, 0.1);
    freqEnvScale.reset(sampleRate, 0.1);
}

//===================================================
void GayOscillator::reset() noexcept
{
    
}

//===================================================
void GayOscillator::noteOn(float vel, float newFreq)
{
    setFrequency(newFreq);
    setGain(vel);
}

//===================================================
void GayOscillator::noteOff()
{
    
}


//*******************
// Parameter values set by basic synth operations (note on)
//===================================================
void GayOscillator::setFrequency(float freq)
{
    currentFreq = freq;
}

void GayOscillator::setGain(float gain)
{
    currentGain = gain;
}


//********************
// Parameter values set by slider changes
//===================================================
void GayOscillator::updateGainOffset(float newOffset)
{
    gainOffset.setTargetValue(newOffset);
}


//===================================================
void GayOscillator::updateWaveOffset(float newOffset)
{
    waveOffset.setTargetValue(newOffset);
}


//===================================================
void GayOscillator::updateFreqOffset(float newOffset)
{
    freqOffset.setTargetValue(newOffset);
}



//********************
// Parameter modulation by LFO depth values set by slider changes
//===================================================
void GayOscillator::updateGainLFODepth(float newDepth)
{
    gainLFODepth.setTargetValue(newDepth);
}

//===================================================
void GayOscillator::updateWaveLFODepth(float newDepth)
{
    waveLFODepth.setTargetValue(newDepth);

}

//===================================================
void GayOscillator::updateFreqLFODepth(float newDepth)
{
    freqLFODepth.setTargetValue(newDepth);

}



//********************
// Parameter modulation by envelope depth values set by slider changes
//===================================================
void GayOscillator::updateGainEnvScale(float newScale)
{
    gainEnvScale.setTargetValue(newScale);
}

//===================================================
void GayOscillator::updateWaveEnvScale(float newScale)
{
    waveEnvScale.setTargetValue(newScale);
}


//===================================================
void GayOscillator::updateFreqEnvScale(float newScale)
{
    freqEnvScale.setTargetValue(newScale);
}



//*****************
// SAMPLE RETRIEVAL AND INCREMENT
//===================================================
float GayOscillator::getNextSample()
{
    _incrementParameters();
    
    float nextIndex     = _getNextSampleIndex(currentFreq.get());
    
    float nextWavePos   = currentWave.get();
    
    float nextSample    = waveVector.getSampleAtIndexAndWavePosition(nextIndex, nextWavePos);
    
    currentSample = nextSample * currentGain.get();
    
    return currentSample.get();
}


//*****************
// SAMPLE RETRIEVAL
//===================================================
float GayOscillator::getCurrentSample()
{
    return currentSample.get();
}






//*****************
// PRIVATE FUNCTIONS

//===================================================
void GayOscillator::_incrementParameters()
{
    gainOffset.getNextValue();
    gainLFODepth.getNextValue();
    gainEnvScale.getNextValue();
    
    freqOffset.getNextValue();
    freqLFODepth.getNextValue();
    freqEnvScale.getNextValue();
    
    waveOffset.getNextValue();
    waveLFODepth.getNextValue();
    waveEnvScale.getNextValue();
    
    _applyGainModulation();
    _applyFreqModulation();
    _applyWaveModulation();
}

//===================================================
void GayOscillator::_applyGainModulation()
{
    currentGain = currentGain.get() + gainOffset.getCurrentValue();
    
    if(gainLFO != nullptr)
    {
        auto scaledLFOValue = gainLFO->getCurrentSample() * gainLFODepth.getCurrentValue();
        currentGain = currentGain.get() + scaledLFOValue;
    }
    
    if(gainEnv != nullptr)
    {
        auto scaledEnvValue = gainEnv->getCurrentValue() * gainEnvScale.getCurrentValue();
        currentGain = currentGain.get() + scaledEnvValue;
    }
        
}

//===================================================
void GayOscillator::_applyFreqModulation()
{
    auto mappedOffsetValue = jmap(freqOffset.getCurrentValue(), 0.f, 1.f, 0.5f, 2.f);
    currentFreq = currentFreq.get() + (currentFreq.get() * mappedOffsetValue);
    
    if(freqLFO != nullptr)
    {
        // max modulation in freq is up/down 1 octave i.e. halving/doubling the current freq
        auto scaledLFOValue = freqLFO->getCurrentSample() * freqLFODepth.getCurrentValue();
        auto mappedLFOValue = jmap(scaledLFOValue, -1.f, 1.f, 0.5f, 2.f);
        
        currentFreq = currentFreq.get() * mappedLFOValue;
    }
    
    if(freqEnv != nullptr)
    {
        auto scaledEnvValue = freqEnv->getCurrentValue() * freqEnvScale.getCurrentValue();
        auto mappedEnvValue = jmap(scaledEnvValue, 0.f, 1.f, 1.f, 2.f);
        currentFreq = currentFreq.get() + mappedEnvValue;
    }
}

//===================================================
void GayOscillator::_applyWaveModulation()
{
    currentWave = currentWave.get() + waveOffset.getCurrentValue();
    
    if(waveLFO != nullptr)
    {
        auto scaledLFOValue = waveLFO->getCurrentSample() * waveLFODepth.getCurrentValue();
        currentWave = currentWave.get() + scaledLFOValue;
    }
    
    if(waveEnv != nullptr)
    {
        auto scaledEnvValue = waveEnv->getCurrentValue() * waveEnvScale.getCurrentValue();
        currentWave = currentWave.get() + scaledEnvValue;
    }
}

//===================================================
float GayOscillator::_getNextSampleIndex(float freq)
{
    int tableSizeOverSampleRate = GPC_CONSTANTS::TABLE_SIZE / mSampleRate;
    
    float phaseIncrement = freq * tableSizeOverSampleRate;
    
    float nextSampleIndex = currentIndex;
    
    currentIndex += phaseIncrement;
    
    return nextSampleIndex;
}

