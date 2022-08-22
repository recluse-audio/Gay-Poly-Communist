/*
  ==============================================================================

    ParameterBase.cpp
    Created: 20 Aug 2022 3:20:14pm
    Author:  Ryan Devens

  ==============================================================================
*/

#include "Parameter_Base.h"

Parameter_Base::Parameter_Base()
{
    
}

Parameter_Base::~Parameter_Base()
{
    
}

void Parameter_Base::prepare(double sampleRate)
{
    currentValue.reset(sampleRate, 0.1);
    currentOffset.reset(sampleRate, 0.1);
    
    jassert(sampleRate > 0);
}

void Parameter_Base::increment()
{
    currentOffset.getNextValue();
    currentValue.getNextValue();
}

float Parameter_Base::getCurrentValue()
{
    return currentValue.getCurrentValue() + currentOffset.getCurrentValue();
}

void Parameter_Base::setSmoothingTime(double smoothingTime)
{
    currentValue.reset(mSampleRate, smoothingTime);
    currentOffset.reset(mSampleRate, smoothingTime);

}

void Parameter_Base::setTargetOffset(float newTarget)
{
    currentOffset.setTargetValue(newTarget);
}

void Parameter_Base::setTargetValue(float newTarget)
{
    currentValue.setTargetValue(newTarget);
}


