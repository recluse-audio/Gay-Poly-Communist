/*
  ==============================================================================

    WaveLoader.cpp
    Created: 8 Jul 2022 5:04:02pm
    Author:  Ryan Devens

  ==============================================================================
*/

#include "WaveLoader.h"
#include "WaveTableVector.h"
#include "GPC_Constants.h"

struct WaveFolder
{
    WaveFolder(StringRef name) : vectorName(name) {}
    ~WaveFolder() {}

    // all this because I can't figure out how to properly concat strings but can with File
    void addWavePath(StringRef wavePath)
    {
        wavePaths.add(wavePath);
    }
    void addWaveName(StringRef waveName)
    {
        waveNames.add(waveName);
    }
    int getNumWaves()
    {
        return wavePaths.size();
    }
    StringRef getWaveName(int index)
    {
        return waveNames[index];
    }
    StringRef getVectorName()
    {
        return vectorName;
    }
private:
    String vectorName;
    StringArray wavePaths;
    StringArray waveNames;
    
};


//*************
// WAVE LOADING STARTS HERE
WaveLoader::WaveLoader()
{
    formatManager.registerBasicFormats();
    populateWaveFolders(GPC_CONSTANTS::defaultFolderPath);
}

WaveLoader::~WaveLoader()
{
    
}


//******************
// INTERFACE TO CALL PRIVATE GET WAVE BUFFERS
juce::Array<juce::AudioBuffer<float>> WaveLoader::getBufferArrayFromIndex(int index)
{
    auto filePath = _getPathFromIndex(index);
    return _getWaveBuffersFromFilePath(filePath);
}


//******************
// INTERFACE TO CALL PRIVATE GET WAVE BUFFERS
juce::Array<juce::AudioBuffer<float>> WaveLoader::getBufferArrayFromFilePath(juce::StringRef filePath)
{
    return _getWaveBuffersFromFilePath(filePath);
}


//******************
// Private implementation called by various interface methods
juce::Array<juce::AudioBuffer<float>> WaveLoader::_getWaveBuffersFromFilePath(juce::StringRef filePath)
{
    auto waveFile = juce::File(filePath);
    auto bufferArray = juce::Array<juce::AudioBuffer<float>>();
    bufferArray.clear();
    
    if (waveFile.isDirectory())
    {
        auto waveDirectories = waveFile.findChildFiles(2, true, "*.wav");

        for (int i = 0; i < waveDirectories.size() - 1; i++)
        {
            auto waveIter = File(waveDirectories[i].getFullPathName());
            
            std::unique_ptr<AudioFormatReader> formatReader{ formatManager.createReaderFor(waveIter) };

            auto tempBuffer = juce::AudioBuffer<float>();
            formatReader->read(&tempBuffer, 0, GPC_CONSTANTS::TABLE_SIZE, 0, true, false);
            
            bufferArray.add(tempBuffer);
        }
        return bufferArray;
    }
    else
    {
        if (waveFile.hasFileExtension(".wav"))
        {
            std::unique_ptr<AudioFormatReader> formatReader{ formatManager.createReaderFor(waveFile) };

            auto tempBuffer = juce::AudioBuffer<float>();
            formatReader->read(&tempBuffer, 0, GPC_CONSTANTS::TABLE_SIZE, 0, true, false);
            
            bufferArray.add(tempBuffer);
        }
        return bufferArray;
    }
    
}
//******************
//
void WaveLoader::populateWaveFolders(juce::StringRef waveFolderPath)
{
    lastSelectedFilePath.clear();
    lastSelectedFilePath = waveFolderPath; // store for later when we want to save/load
    
    auto folders = juce::File(waveFolderPath).findChildFiles(1, true);

    for (int i = 0; i < folders.size(); i++)
    {
        auto currentFolder = folders[i];

        waveFolders.set(i, new WaveFolder(currentFolder.getFileName()));

        // all the waves stored inside the current folder
        auto waves = currentFolder.findChildFiles(juce::File::findFiles, true);


        for (int j = 0; j < waves.size(); j++)
        {
            waveFolders[i]->addWavePath(waves[j].getFullPathName());
            waveFolders[i]->addWaveName(waves[j].getFileNameWithoutExtension());
            allWavePaths.add(waves[j].getFullPathName());
        }

        waveFolders[i]->addWavePath(currentFolder.getFullPathName());
        waveFolders[i]->addWaveName(currentFolder.getFileNameWithoutExtension());
        allWavePaths.add(currentFolder.getFullPathName());

    }
}

juce::AudioBuffer<float> WaveLoader::getValidBuffer(juce::AudioBuffer<float> bufferToConform)
{
    // Stop and return if already exactly correct size
    if(bufferToConform.getNumSamples() == GPC_CONSTANTS::TABLE_SIZE)
        return bufferToConform;
    
    auto conformedBuffer = juce::AudioBuffer<float>(1, GPC_CONSTANTS::TABLE_SIZE);
    
    auto buffRead  = bufferToConform.getArrayOfReadPointers();
    auto buffWrite = conformedBuffer.getArrayOfWritePointers();
    
    // Scale how much we increment the read index then interpolate before writing to conformed buffer
    float sizeRatio = (float)bufferToConform.getNumSamples() / (float)GPC_CONSTANTS::TABLE_SIZE;

    for (int i = 0; i < GPC_CONSTANTS::TABLE_SIZE; i++)
    {
        // downsampling / interpolation algorithm to make new table into 2048
        auto readIndex = i * sizeRatio;
        float frac = readIndex - (int)readIndex;

        float readSample0 = buffRead[0][(int)readIndex] * (1 - frac);
        float readSample1 = buffRead[0][(int)readIndex + 1] * frac;
        float readSample = readSample0 + readSample1;
          
        buffWrite[0][i] = readSample;

    }
    
    return conformedBuffer;
}


juce::OwnedArray<WaveFolder>& WaveLoader::getWaveFolders()
{
    return waveFolders;
}


StringRef WaveLoader::_getPathFromIndex(int index)
{
    return allWavePaths[index];
}

