/*
  ==============================================================================

    GayOscillator.cpp
    Created: 16 Jul 2022 11:24:02am
    Author:  Ryan Devens

  ==============================================================================
*/

#include "GayOscillator.h"
#include "GayParam.h"

GayOscillator::GayOscillator(WaveTableVector& vector)  : waveVector(vector)
{
    mGain = std::make_unique<GayParam>(GayParam::ParamType::Gain);
    mFrequency = std::make_unique<GayParam>(GayParam::ParamType::Frequency);
    mWavePosition = std::make_unique<GayParam>(GayParam::ParamType::WavePosition);

}

GayOscillator::~GayOscillator(){}

void GayOscillator::prepare(double sampleRate)
{

    oSampleRate = sampleRate;
    mGain->prepare(sampleRate);
    mFrequency->prepare(sampleRate);
    mWavePosition->prepare(sampleRate);

}

void GayOscillator::noteOn(float vel, float newFreq)
{
    mFrequency->setValue(newFreq);
}

void GayOscillator::noteOff(){}

//==============================================================================
// called by pitch bend
void GayOscillator::setFrequency(float newValue, bool force = false)
{
    mFrequency->setValue(newValue);
}

void GayOscillator::setGain(float newValue){}

void GayOscillator::reset() noexcept{}

//*****************
// SAMPLE RETRIEVAL
float GayOscillator::getNextSample()
{
    // increments freq param while calculating new phase inc and next sample
    float nextIndex     = _getNextSampleIndex(mFrequency->getNextValue());
    
    float nextWavePos   = mWavePosition->getNextValue();
    
    float nextSample    = waveVector.getSampleAtIndexAndWavePosition(nextIndex, nextWavePos);
    
    return nextSample * mGain->getNextValue();
}

//==============================================================================

WaveTableVector& GayOscillator::getWaveVector()
{
    return waveVector;
}


// changes in value tree
void GayOscillator::update(float gain, float gainLFOScale, float gainEnvScale,
            float frequency, float frequencyLFOScale, float frequencyEnvScale,
            float wavePosition, float waveLFOScale, float waveEnvScale)
{
    mGain->setValue(gain);
    mGain->setLFOScale(gainLFOScale);
    mGain->setEnvScale(gainEnvScale);

    mFrequency->setOffset(frequency);
    mFrequency->setLFOScale(frequencyLFOScale);
    mFrequency->setEnvScale(frequencyEnvScale);

    mWavePosition->setValue(wavePosition);
    mWavePosition->setLFOScale(waveLFOScale);
    mWavePosition->setEnvScale(waveEnvScale);

}

void GayOscillator::update(float newGain, float frequency, float wavePosition)
{
    mGain->setValue(newGain);
    mFrequency->setOffset(frequency);
    mWavePosition->setValue(wavePosition);
}



void GayOscillator::assignLFO(GayOscillator* mLFO, GayParam::ParamType pType)
{
    using GayType = GayParam::ParamType;
    
    if (pType == GayType::Gain)
        mGain->assignLFO(mLFO);
    if (pType == GayType::WavePosition)
        mWavePosition->assignLFO(mLFO);
    if (pType == GayParam::Frequency)
        mFrequency->assignLFO(mLFO);
}

void GayOscillator::setNoLFO(GayParam::ParamType pType)
{
    using GayType = GayParam::ParamType;
    if (pType == GayType::Gain)
        mGain->setNoLFO();
    if (pType == GayType::WavePosition)
        mWavePosition->setNoLFO();
    if (pType == GayParam::Frequency)
        mFrequency->setNoLFO();
}

void GayOscillator::assignEnvelope(GayADSR* mEnv, GayParam::ParamType pType)
{
    using GayType = GayParam::ParamType;
    if (pType == GayType::Gain)
        mGain->assignEnvelope(mEnv);
    if (pType == GayType::WavePosition)
        mWavePosition->assignEnvelope(mEnv);
    if (pType == GayParam::Frequency)
        mFrequency->assignEnvelope(mEnv);
    //gain->assignEnvelope(mEnv);
}

void GayOscillator::setNoEnv(GayParam::ParamType pType)
{
    using GayType = GayParam::ParamType;
    if (pType == GayType::Gain)
        mGain->setNoEnv();
    if (pType == GayType::WavePosition)
        mWavePosition->setNoEnv();
    if (pType == GayParam::Frequency)
        mFrequency->setNoEnv();
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
