/*
  ==============================================================================

    GayParam.h
    Created: 7 Dec 2021 4:01:08pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GayOscillator.h"
#include "GayADSR.h"

class GayOscillator;

class GayParam
{
public:
    enum ParamType
    {
        Gain,
        Frequency,
        WavePosition,
    };

    GayParam(ParamType t) : type(t) {}
    ~GayParam() {}

    void prepare(double sampleRate) 
    { 
        value.reset(sampleRate, 0.01); 
    }

    void setValue(float val) 
    { 
        value.setTargetValue(val); 
    }

    void setOffset(float off)
    {
        offset = off; // currently just for pitch
    }
    
    void assignLFO(GayOscillator* mLFO)
    {
        lfo = mLFO;
        hasLFO = true;
    }
    void setNoLFO() 
    { 
        hasLFO = false; 
    }
    
    void setLFOScale(float scale)
    {
        lfoScale = scale;
    }

    void assignEnvelope(GayADSR* mEnv)
    {
        env = mEnv;
        hasEnv = true;
    }
    
    void setNoEnv() 
    { 
        hasEnv = false; 
    }

    void setEnvScale(float scale)
    {
        envScale = scale;
    }

    float getNextValue()
    {
        val = value.getNextValue();

        if (type == Gain) // limit to 0 - 1
        {
            if (hasLFO)
            {
                val = val + (lfo->getCurrentSample() * lfoScale); // mods are incremented in the voice
            }
            if (hasEnv)
            {
                val = val * (env->getCurrentValue()* envScale);

            }
            return val;
        }
        if (type == Frequency)
        {
            val += (offset * val); // scale -1., 1 
            if (hasLFO)
            {
                val = val + (val * lfo->getCurrentSample() * lfoScale);
            }
            if (hasEnv)
            {
                val = val + (val * env->getCurrentValue() * envScale);
            }
            return val;
        }
        if (type == WavePosition)
        {
            if (hasLFO)
            {
                val = val + (lfo->getCurrentSample()*lfoScale);
            }
            if (hasEnv)
            {
                val = val + (env->getCurrentValue() * envScale);
            }

            if (val > 1.f)
                val = val - 1.f;

            return val;
        }
    }

    float getCurrentValue()
    {
        return val;
    }

private:
    GayOscillator* lfo;
    GayADSR* env;
    bool hasLFO = false;
    bool hasEnv = false;
    SmoothedValue<float> value;
    float val = 1.f;  // used to get value outside of sample loop
    float offset = 0.f; // only used in pitch right now, must separate from note on pitch msg. (do i have to do this?)
    float lfoScale = 1.f, envScale = 1.f; // scaling modulator values
    ParamType type;
};
