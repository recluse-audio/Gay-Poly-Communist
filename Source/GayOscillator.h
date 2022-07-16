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


class GayParam;
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

    void noteOn(float vel, float newFreq);

    void noteOff(){}
 
    //==============================================================================
    // called by pitch bend
    void setFrequency(float newValue, bool force = false);

    void setGain(float newValue);

    void reset() noexcept;

    //*****************
    // SAMPLE RETRIEVAL
    float getNextSample();

    //==============================================================================

    WaveTableVector& getWaveVector();


    // changes in value tree
    void update(float gain, float gainLFOScale, float gainEnvScale,
                float frequency, float frequencyLFOScale, float frequencyEnvScale,
                float wavePosition, float waveLFOScale, float waveEnvScale);
    
    void update(float newGain, float frequency, float wavePosition);
    
    

    void assignLFO(GayOscillator* mLFO, GayParam::ParamType pType);
    
    void setNoLFO(GayParam::ParamType pType);

    void assignEnvelope(GayADSR* mEnv, GayParam::ParamType pType);
    
    void setNoEnv(GayParam::ParamType pType);
    
private:
    WaveTableVector& waveVector;
    double glideTime = 0.1;
    double oSampleRate = -1;
    float currentIndex = 0;
    std::unique_ptr<GayParam> mGain, mWavePosition, mFrequency;
    
//**************
// PRIVATE FUNCTIONS
    
    float _getNextSampleIndex(float freq);

    

};

