/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GaySynth.h"

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

    WaveTableVector& getWaveVector();
    GaySynth& getSynth();

    void shouldProcess(bool isProcessing);
    bool checkVoices();

    float getRMS();

private:
    GaySynth synth;

    juce::AudioProcessorValueTreeState apvts;
    juce::Atomic<bool> mustUpdateProcessing{ false };
    Atomic<bool> processing{ true };



    float RMS = 0.f;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        mustUpdateProcessing = true;
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GayPolyCommunistAudioProcessor)
};