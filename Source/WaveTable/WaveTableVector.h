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

static const juce::String wavetablePath = {"/Library/Application Support/Recluse-Audio/GPC/Wavetables/Signature Wavetables/Test/"};

class WaveTableVector
{
public:
    WaveTableVector() : tableSize(2048)
    {
        formatManager.registerBasicFormats();

        // intitialize 100, then just change their waveform 
        // we keep track of arraySize so that we can map our 0-1 wave value to map to an appropriate value
        // Wasn't working to clear the array each time
        for (int i = 0; i < 100; i++)
        {
            tableArray.add(new WaveTable(tableSize));
        }
        loadTables(wavetablePath);
    }

    ~WaveTableVector() 
    {
        tableArray.clear(true);
    }

    void prepare(double sampleRate)
    {
        mSampleRate = sampleRate;
        prepTables();
        waveVal.reset(sampleRate, 0.01);
    }

    void prepTables()
    {
        for (int i = 0; i < tableArray.size(); i++)
        {
            tableArray[i]->prepare(mSampleRate);
        }
        loading = false;

    }

    /*
        Loading files into table array
        Notice that we are tracking arraySize separately because deleting/clearing the array each time was not working for me
        if the file path leads to a directory (user dragged on a folder of tables) then we rewrite the existing wave at those
        indices of the table array
    */
    void loadTables(StringRef filePath)
    {
        loading = true;

        auto waveFile = File(filePath);

        if (waveFile.isDirectory())
        {
            auto waveFolders = waveFile.findChildFiles(2, true, "*.wav");

            arraySize = waveFolders.size() - 1;

            for (int i = 0; i < arraySize; i++)
            {
                auto waveIter = File(waveFolders[i].getFullPathName());
                std::unique_ptr<AudioFormatReader> formatReader{ formatManager.createReaderFor(waveIter) };

                formatReader->read(&tableArray[i]->getBuffer(), 0, tableSize, 0, true, false);
            }
        }
        else
        {
            if (waveFile.hasFileExtension(".wav"))
            {    
                arraySize++; // accounting for added table
                std::unique_ptr<AudioFormatReader> formatReader{ formatManager.createReaderFor(waveFile) };
                formatReader->read(&tableArray[arraySize-1]->getBuffer(), 0, tableSize, 0, true, false);
            }
        }
        

        prepTables();
    }

    void loadTableFromBuffer(AudioBuffer<float>& waveBuffer)
    {
        arraySize++;
        tableArray[arraySize - 1]->passBuffer(waveBuffer);
    }

    void setFrequency(float freq)
    {
        for (int i = 0; i < arraySize; i++)
        {
            tableArray[i]->setFrequency(freq);
        }
    }

    
    float getNextSample()
    {
        float wavePos = waveVal.getNextValue();

        int lowerWaveIndex = (int)wavePos;
        int upperWaveIndex = lowerWaveIndex + 1;

        if (lowerWaveIndex + 1 > arraySize - 1)
        {
            upperWaveIndex = 0;
        }

        float interp = wavePos - (float)lowerWaveIndex;

        auto sample1 = tableArray[lowerWaveIndex]->getNextSample() * (1.f - interp);

        auto sample2 = tableArray[upperWaveIndex]->getNextSample() * (interp);

        auto sample = sample1 + sample2;

        return sample;

        
    }

    WaveTable& getInterpolatedTable()
    {
        // TO DO: This is not satisfactory
        auto table = *tableArray[0];
        return table;
    }


    void setWave(float waveForm)
    {
        auto mappedWaveIndex = jmap(waveForm, 0.f, (float)arraySize - 1.f);
        waveVal.setTargetValue(mappedWaveIndex);
    }

    WaveTable* atIndex(int index)
    {
        return tableArray[index];
    }

    int vectorSize()
    {
        return tableArray.size();
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

    float getWaveVal()
    {
        return waveVal.getCurrentValue();
    }

    int getTableSize()
    {
        return tableSize;
    }

    int getArraySize()
    {
        return arraySize;
    }
private:

    OwnedArray<WaveTable> tableArray;
    AudioFormatManager formatManager;
    CriticalSection lock;

    int tableSize = 0;
    int arraySize = 0;

    SmoothedValue<float> waveVal; // float interpVal{ 0.f };


    Atomic<bool> loading { false };
    
    double mSampleRate = 48000;
};
