/*
  ==============================================================================

    GayOscillator.h
    Created: 20 Sep 2021 2:35:36pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../WaveTable/WaveTableVector.h"
#include "GayADSR.h"
#include "GayParam.h"


//==============================================================================
/*
    This should only retrieve samples from the waveVector
 */

class GayOscillator
{
public:

    //==============================================================================
    // each oscillator
    GayOscillator(WaveTableVector& vector)  : waveVector(vector)
    {
        gain = std::make_unique<GayParam>(GayParam::ParamType::gain);
        freq = std::make_unique<GayParam>(GayParam::ParamType::pitch);
        wave = std::make_unique<GayParam>(GayParam::ParamType::wave);

    }

    ~GayOscillator(){}

    void prepare(double sampleRate)
    {

        oSampleRate = sampleRate;
        gain->prepare(sampleRate);
        freq->prepare(sampleRate);
        wave->prepare(sampleRate);

    }

    void noteOn(float vel, float newFreq)
    {
        freq->setValue(newFreq);
    }

    void noteOff(){}
 
    //==============================================================================
    // called by pitch bend
    void setFrequency(float newValue, bool force = false)
    {
        freq->setValue(newValue);
    }

    void setLevel(float newValue){}

    void reset() noexcept{}

    //*****************
    // SAMPLE RETRIEVAL
    float getNextSample()
    {
        // increments freq param while calculating new phase inc and next sample
        float nextIndex     = _getNextSampleIndex(freq->getNextValue());
        
        float nextWavePos   = wave->getNextValue();
        
        float nextSample    = waveVector.getSampleAtIndexAndWavePosition(nextIndex, nextWavePos);
        
        return nextSample * gain->getNextValue();
    }

    //==============================================================================

    WaveTableVector& getWaveVector()
    {
        return waveVector;
    }


    void update(AudioProcessorValueTreeState& apvts)
    {
        gain->setValue(g);
        gain->setLFOScale(gLFOScale);
        gain->setEnvScale(gEnvScale);

        freq->setOffset(p);
        freq->setLFOScale(pLFOScale);
        freq->setEnvScale(pEnvScale);

        wave->setValue(w);
        wave->setLFOScale(wLFOScale);
        wave->setEnvScale(wEnvScale);

    }

    void assignLFO(WaveTable* mLFO, GayParam::ParamType pType)
    {
        using GayType = GayParam::ParamType;
        
        if (pType == GayType::gain)
            gain->assignLFO(mLFO);
        if (pType == GayType::wave)
            wave->assignLFO(mLFO);
        if (pType == GayParam::pitch)
            freq->assignLFO(mLFO);
    }
    
    void setNoLFO(GayParam::ParamType pType)
    {
        using GayType = GayParam::ParamType;
        if (pType == GayType::gain)
            gain->setNoLFO();
        if (pType == GayType::wave)
            wave->setNoLFO();
        if (pType == GayParam::pitch)
            freq->setNoLFO();
    }

    void assignEnvelope(GayADSR* mEnv, GayParam::ParamType pType)
    {
        using GayType = GayParam::ParamType;
        if (pType == GayType::gain)
            gain->assignEnvelope(mEnv);
        if (pType == GayType::wave)
            wave->assignEnvelope(mEnv);
        if (pType == GayParam::pitch)
            freq->assignEnvelope(mEnv);
        //gain->assignEnvelope(mEnv);
    }
    
    void setNoEnv(GayParam::ParamType pType)
    {
        using GayType = GayParam::ParamType;
        if (pType == GayType::gain)
            gain->setNoEnv();
        if (pType == GayType::wave)
            wave->setNoEnv();
        if (pType == GayParam::pitch)
            freq->setNoEnv();
    }
private:
    WaveTableVector& waveVector;
    double glideTime = 0.1;
    double oSampleRate = -1;
    float currentIndex = 0;
    std::unique_ptr<GayParam> gain, wave, freq;
    
    
    float _getNextSampleIndex(float freq)
    {
        int tableSizeOverSampleRate = GPC_CONSTANTS::TABLE_SIZE / oSampleRate;
        
        float phaseIncrement = freq * tableSizeOverSampleRate;
        
        float nextSampleIndex = currentIndex;
        
        currentIndex += phaseIncrement;
        
        return nextSampleIndex;
    }

    

};

