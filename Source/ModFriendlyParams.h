/*
  ==============================================================================

    ModFriendlyParams.h
    Created: 16 Jul 2022 1:30:23pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once
#include "GayParam.h"

/*
    Container class that organizes parameters that can be destinations of modulation into easy to navigate arrays
*/
class ModFriendlyParams
{
public:

    enum class ParamIds
    {
        Oscillator1_Gain = 0,
        Oscillator1_WavePosition,
        Oscillator1_Frequency,
        
        Oscillator2_Gain,
        Oscillator2_WavePosition,
        Oscillator2_Frequency,
        
        LFO1_Depth,
        LFO1_Rate,
        
        LFO2_Depth,
        LFO2_Rate,
        
        LFO3_Depth,
        LFO3_Rate,
        
        FilterFrequency,
        FilterResonance,
        FilterDrive,
        
        TotalNumParams // Don't assign this, but use it to resize paramArray
    };
    
    ModFriendlyParams()
    {
        
    }
    
    ~ModFriendlyParams(){}
    
//    void setParamValue(ModFriendlyParams::Parameter Ids paramId)
//    {
//        
//    }
//    
//    float getParamValue(ModFriendlyParams::ParameterIds paramId)
//    {
//        
//    }
    
private:
    juce::OwnedArray<GayParam> mParamArray;
   //ModFriendlyParams::ParameterIds;
}
