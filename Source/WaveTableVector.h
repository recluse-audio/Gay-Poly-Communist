/*
  ==============================================================================

    WaveTableVector.h
    Created: 21 Oct 2021 7:10:20pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WaveTable.h"
#include "GPC_Constants.h"


/*
    Used to retrieve samples from 2 wavetables out of a vector (array)
    then interpolate between then based on wavePos at the time of sample retrieval
 
    this is for READING and not calculating phase increments
    Use an oscillator to set frequency, phase inc, and to retrieveSample() at the right index
    Same goes for wave position.  That is done by the oscillator
 
 */
class WaveTableVector
{
public:
    WaveTableVector(juce::Array<juce::AudioBuffer<float>> waveBufferArray)
    {

        for (auto waveBuffer : waveBufferArray)
        {
            tableArray.add(new WaveTable(waveBuffer));
        }
    }

    ~WaveTableVector() 
    {
        tableArray.clear(true);
    }


    void addTableFromBuffer(juce::AudioBuffer<float> waveBuffer)
    {
        tableArray.add(new WaveTable(waveBuffer));
    }
    
    
    // clears and reloads buffer with new vector (array) of wavetables
    void loadVectorFromBufferArray(juce::Array<juce::AudioBuffer<float>> waveBufferArray)
    {
        loading = true;
        
        // SINGLE WAVE IN ARRAY
        if(waveBufferArray.size() == 1)
        {
            addTableFromBuffer(waveBufferArray[0]);
            return;
        }
        
        // MULTIPLE WAVES IN ARRAY
        numberOfWaveTables = waveBufferArray.size();
        
        // Put new buffers in array
        for (int i = 0; i < numberOfWaveTables; i++)
        {
            tableArray[i]->passBuffer(waveBufferArray[i]);
        }
    }

    
    
    float getSampleAtIndexAndWavePosition(float index, float wavePos)
    {
        int lowerWaveIndex = (int)wavePos;
        int upperWaveIndex = lowerWaveIndex + 1;

        if (lowerWaveIndex + 1 > numberOfWaveTables - 1)
        {
            upperWaveIndex = 0;
        }

        float interp = wavePos - (float)lowerWaveIndex;

        auto sample1 = tableArray[lowerWaveIndex]->getSampleAtIndex(index) * (1.f - interp);

        auto sample2 = tableArray[upperWaveIndex]->getSampleAtIndex(index) * (interp);

        auto sample = sample1 + sample2;

        return sample;

        
    }



    WaveTable* atIndex(int index)
    {
        return tableArray[index];
    }


    bool isFinishedLoading()
    {
        return !loading.get();
    }

    WaveTable* getLowerWave(int lowerWaveIndex)
    {
        return tableArray[lowerWaveIndex];
    }

    WaveTable* getUpperWave(int upperWaveIndex)
    {
        return tableArray[upperWaveIndex];
    }


    int getTableSize()
    {
        return tableSize;
    }

    int getNumberOfWaveTables()
    {
        return numberOfWaveTables;
    }
    
    
private:
    juce::OwnedArray<WaveTable> tableArray;
    CriticalSection lock;

    int tableSize = 0;
    int numberOfWaveTables = 0;
    

    juce::Atomic<bool> loading { false };
    
};
