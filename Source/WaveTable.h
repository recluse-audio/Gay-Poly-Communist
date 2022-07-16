/*
  ==============================================================================

    WaveTable.h
    Created: 27 May 2021 7:42:15am
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GPC_Constants.h"

class WaveTable
{
public:
    /*
        Basically a juce::AudioBuffer<float> that handles down/up-sampling to make everything the table size constant
        Also returns sample value at float index, interpolating linearly between sequential samples
     
        TODO: Template this?
    */
    WaveTable(juce::AudioBuffer<float> wBuffer)
    : waveBuffer(wBuffer)
    {
    }



    ~WaveTable() {}

    void prepare(double sampleRate)
    {
        mSampleRate = sampleRate;
    }


    // Does calculate interp between samples here
    float getSampleAtIndex(float index)
    {
        // needs to handle wrapping before gett*ing here
        jassert(index < GPC_CONSTANTS::TABLE_SIZE);
        
        int lowIndex  = (int)index;
        int highIndex = lowIndex + 1;
        
        float spillOver         = index - lowIndex; // how far into next sample this index is
        float inverseSpillOver  = 1.f   - spillOver;  //
        
        float sample1 = waveBuffer.getSample(0, lowIndex) * spillOver;
        float sample2 = waveBuffer.getSample(0, highIndex) * inverseSpillOver;
        
        float sample = (sample1 + sample2) / 2.f;
        
        return waveBuffer.getSample(0, sample);
    }

    
    AudioBuffer<float>& getBuffer()
    {
        return waveBuffer;
    }


    // passes new buffer to wavetable, handles conversion to size of 2048
    void passBuffer(juce::AudioBuffer<float> newTable) // coming in at length of period
    {

        auto buffRead = newTable.getArrayOfReadPointers();
        auto buffWrite = waveBuffer.getArrayOfWritePointers();
        float sizeRatio = (float)newTable.getNumSamples() / (float)GPC_CONSTANTS::TABLE_SIZE;

        for (int i = 0; i < GPC_CONSTANTS::TABLE_SIZE; i++)
        {
            auto waveIndex = i; // for our nice 2048 sample sized wavetable
                
            // downsampling / interpolation algorithm to make new table into 2048
            auto readIndex = i * sizeRatio;
            float frac = readIndex - (int)readIndex; 

            float readSample0 = buffRead[0][(int)readIndex] * (1 - frac);
            float readSample1 = buffRead[0][(int)readIndex + 1] * frac;
            float readSample = readSample0 + readSample1;
              
            buffWrite[0][i] = readSample;

        }

    }

    void setGain(float gainVal)
    {
        gain = gainVal;
    }
    
private:
    juce::AudioBuffer<float> waveBuffer;
    double mSampleRate = 48000;
    float tableDelta = 0.f, currentIndex = 0.f, currentSample = 0.f;
    float gain = 1.f;
};
