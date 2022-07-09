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
#include "../GPC_Constants.h"


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
    WaveTableVector(int maxNumTables, juce::OwnedArray<juce::AudioBuffer<float>>& waveBufferArray)
    : tableSize(GPC_CONSTANTS::TABLE_SIZE)
    {

        // intitialize 100, then just change their waveform 
        // we keep track of arraySize so that we can map our 0-1 wave value to map to an appropriate value
        // Wasn't working to clear the array each time
        for (int i = 0; i < maxNumTables; i++)
        {
            tableArray.add(new WaveTable(tableSize));
        }
    }

    ~WaveTableVector() 
    {
        tableArray.clear(true);
    }


    void addTableFromBuffer(juce::AudioBuffer<float>& waveBuffer)
    {
        numberOfWaveTables++;
        tableArray[numberOfWaveTables - 1]->passBuffer(waveBuffer);
    }
    
    
    // clears and reloads buffer with new vector (array) of wavetables
    void loadVectorFromBufferArray(juce::OwnedArray<juce::AudioBuffer<float>>& waveBufferArray)
    {
        loading = true;
        
        // SINGLE WAVE IN ARRAY
        if(waveBufferArray.size() == 1)
        {
            addTableFromBuffer(*waveBufferArray[0]);
            return;
        }
        
        // MULTIPLE WAVES IN ARRAY
        numberOfWaveTables = waveBufferArray.size();
        
        // Put new buffers in array
        for (int i = 0; i < numberOfWaveTables; i++)
        {
            tableArray[i]->passBuffer(*waveBufferArray[i]);
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

    WaveTable& getInterpolatedTable()
    {
        // TO DO: This is not satisfactory
        auto table = *tableArray[0];
        return table;
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
