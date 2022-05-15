/*
  ==============================================================================

    WaveDatabase.h
    Created: 8 Dec 2021 2:32:12pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class WaveDatabase
{
public:
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
    WaveDatabase(){}
    ~WaveDatabase(){}

    void loadFiles()
    {
        auto folders = juce::File("C:/ProgramData/Recluse-Audio/GPC/WaveTables/").findChildFiles(1, true);

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

    OwnedArray<WaveFolder>& getWaveFolders()
    {
        return waveFolders;
    }

    // this function exists because PopupMenu keeps track of indices in a one dimensional array
    // If I have 100 wavetables in 10x10 groups, it can't tell me [10][9] (vector 10 wave 9)
    // only '90'

    StringRef getPathFromIndex(int index)
    {
        return allWavePaths[index];
    }

private:
    OwnedArray<WaveFolder> waveFolders;
    StringArray allWavePaths; // see 'getPathFromIndex'

};