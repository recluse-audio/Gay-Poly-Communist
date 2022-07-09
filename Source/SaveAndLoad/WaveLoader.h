/*
  ==============================================================================

    WaveDatabase.h
    Created: 8 Dec 2021 2:32:12pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct WaveFolder;
class WaveTableVector;



/*
    This class handles loading waves that have been saved from an index
    as well as wavetables/vectors that were dragdropped.
 
    
   Give it a file path, it will store it (for saving and menus)
    and return buffers in place of audio files
*/

class WaveLoader
{
public:
    WaveLoader();
    ~WaveLoader();

    
    // fills our wavefolders with the important info
    void populateWaveFolders(juce::StringRef waveFolderPath);
    
    juce::OwnedArray<juce::AudioBuffer<float>> getBufferArrayFromIndex(int index);
    
    // Don't want to hold on to these, pass them along
    juce::OwnedArray<juce::AudioBuffer<float>> getBufferArrayFromFilePath(juce::StringRef filePath);
    


    // Used to populate menu and get data from waves (name artist etc)
    OwnedArray<WaveFolder>& getWaveFolders();
    
    



    
    
private:
    OwnedArray<WaveFolder> waveFolders;
    StringArray allWavePaths; // see 'getPathFromIndex'
    juce::String lastSelectedFilePath; // path to containing folder of current wavetable

    AudioFormatManager formatManager;
    
    void _prepTables();
    
    juce::StringRef _getPathFromIndex(int index);

    juce::OwnedArray<juce::AudioBuffer<float>> _getWaveBuffersFromFilePath(juce::StringRef filePath);
    

};
