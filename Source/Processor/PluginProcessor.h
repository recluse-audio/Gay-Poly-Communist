/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Synth/GaySynth.h"
#include "WaveDatabase.h"

//==============================================================================
/**
*/
class GayPolyCommunistAudioProcessor  : public juce::AudioProcessor,
    public juce::ValueTree::Listener
{
public:
    //==============================================================================
    GayPolyCommunistAudioProcessor();
    ~GayPolyCommunistAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void update();

    juce::AudioProcessorValueTreeState& getValueTree() { return apvts; }
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    WaveTableVector& getWaveVector(int oscNumber);
    GaySynth& getSynth();

    void shouldProcess(bool isProcessing);
    bool checkVoices();

    float getRMS();

    float getLFOSource();
    float getEnvSource();
    void setLFOSource(int source);
    void setEnvSource(int source);

    void setMappingLFO(bool lfoMapping, float source);
    void setMappingEnv(bool envMapping, float source);
    bool isMappingLFO();
    bool isMappingEnv();
    int getMappingLFO();

    void toggleMidiTest(bool shouldToggle);
    void triggerMidi(bool isNoteOn);

    WaveDatabase& getWaveDatabase();

    void loadWaveTables(const StringArray& filePath, int oscNum);
    void loadTableFromBuffer(AudioBuffer<float>& wave, int oscNum);

    float getLFODepth(int lfoNum);
private:
    GaySynth synth;

    float lfoSource = 0.f;
    float envSource = 0.f;
    bool mappingLFO = false;
    bool mappingEnv = false;

    juce::AudioProcessorValueTreeState apvts;
    juce::Atomic<bool> mustUpdateProcessing{ false };
    Atomic<bool> processing{ true };
    Atomic<bool> noteOn{ false };
    Atomic<bool> noteOff{ false };

    float RMS = 0.f;

    WaveDatabase waveDatabase;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        mustUpdateProcessing = true;
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GayPolyCommunistAudioProcessor)
};
