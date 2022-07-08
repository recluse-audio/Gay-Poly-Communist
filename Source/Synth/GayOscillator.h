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
class GayOscillator
{
public:

    //==============================================================================
    // each oscillator
    GayOscillator()
    {
        gain = std::make_unique<GayParam>(GayParam::ParamType::gain);
        pitch = std::make_unique<GayParam>(GayParam::ParamType::pitch);
        wave = std::make_unique<GayParam>(GayParam::ParamType::wave);

    }

    ~GayOscillator(){}

    void prepare(double sampleRate)
    {
        waveVector.prepare(sampleRate);
        waveVector.setWave(0.5f);

        gain->prepare(sampleRate);
        pitch->prepare(sampleRate);
        wave->prepare(sampleRate);

    }

    void noteOn(float vel, float freq)
    {
        pitch->setValue(freq);
    }

    void noteOff(){}
 
    //==============================================================================
    void setFrequency(float newValue, bool force = false)
    {

        pitch->setValue(newValue);
        //waveVector.setFrequency(newValue);
    }

    void setLevel(float newValue){}

    void reset() noexcept{}

    // iterates and returns
    float getNextSample()
    {
        waveVector.setWave(wave->getNextValue());
        waveVector.setFrequency(pitch->getNextValue());
        return waveVector.getNextSample() * gain->getNextValue();
    }

    //==============================================================================

    WaveTableVector& getWaveVector()
    {
        return waveVector;
    }

    void loadTables(StringRef filePath)
    {
        waveVector.loadTables(filePath);
    }

    void loadTableFromBuffer(AudioBuffer<float>& waveBuffer)
    {
        waveVector.loadTableFromBuffer(waveBuffer);
    }

    void update(float g, float gLFOScale, float gEnvScale, float w, float wLFOScale, float wEnvScale, float p, float pLFOScale, float pEnvScale)
    {
        gain->setValue(g);
        gain->setLFOScale(gLFOScale);
        gain->setEnvScale(gEnvScale);

        pitch->setOffset(p);
        pitch->setLFOScale(pLFOScale);
        pitch->setEnvScale(pEnvScale);

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
            pitch->assignLFO(mLFO);
    }
    
    void setNoLFO(GayParam::ParamType pType)
    {
        using GayType = GayParam::ParamType;
        if (pType == GayType::gain)
            gain->setNoLFO();
        if (pType == GayType::wave)
            wave->setNoLFO();
        if (pType == GayParam::pitch)
            pitch->setNoLFO();
    }

    void assignEnvelope(GayADSR* mEnv, GayParam::ParamType pType)
    {
        using GayType = GayParam::ParamType;
        if (pType == GayType::gain)
            gain->assignEnvelope(mEnv);
        if (pType == GayType::wave)
            wave->assignEnvelope(mEnv);
        if (pType == GayParam::pitch)
            pitch->assignEnvelope(mEnv);
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
            pitch->setNoEnv();
    }
private:
    WaveTableVector waveVector;
    double glideTime = 0.1;
    std::unique_ptr<GayParam> gain, wave, pitch;
    
    

};

