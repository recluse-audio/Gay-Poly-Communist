/*
  ==============================================================================

    ParameterBase.h
    Created: 20 Aug 2022 3:20:14pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Parameter_Base
{
public:
    Parameter_Base();
    ~Parameter_Base();
    
    void prepare(double sampleRate);
    
    void increment();
    
    float getCurrentValue();

    void setSmoothingTime(double time);
    void setTargetOffset(float newTarget);
    void setTargetValue(float newTarget);
    
protected:
    double mSampleRate = -1;
    juce::SmoothedValue<float> currentOffset;
    juce::SmoothedValue<float> currentValue;
    
private:
    
};
