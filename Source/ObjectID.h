/*
  ==============================================================================

    ObjectIDs.h
    Created: 21 Aug 2022 3:23:02pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once

class ObjectID
{
public:
    enum class OscillatorID
    {
        Oscillator1 = 1,
        Oscillator2,
        LFO1,
        LFO2,
        LFO3
    };
    
    enum class ParamID
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
    
private:
    
};
