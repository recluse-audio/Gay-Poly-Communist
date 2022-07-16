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
    
    juce::Array<juce::AudioBuffer<float>> getBufferArrayFromIndex(int index);
    
    // Don't want to hold on to these, pass them along
    juce::Array<juce::AudioBuffer<float>> getBufferArrayFromFilePath(juce::StringRef filePath);
    
    // Makes sure that buffer is GPC_CONSTANTS::TABLE_SIZE in length.
    // TODO:  Normalize amplitudes?  Compress them?  Give Option?
    juce::AudioBuffer<float> getValidBuffer(juce::AudioBuffer<float> bufferToConform);

    // Used to populate menu and get data from waves (name artist etc)
    juce::OwnedArray<WaveFolder>& getWaveFolders();
    
    



    
    
private:
    juce::OwnedArray<WaveFolder> waveFolders;
    juce::StringArray allWavePaths; // see 'getPathFromIndex'
    juce::String lastSelectedFilePath; // path to containing folder of current wavetable

    AudioFormatManager formatManager;
    
    void _prepTables();
    
    juce::StringRef _getPathFromIndex(int index);

    juce::Array<juce::AudioBuffer<float>> _getWaveBuffersFromFilePath(juce::StringRef filePath);
    

};
