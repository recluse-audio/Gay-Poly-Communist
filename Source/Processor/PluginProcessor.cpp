/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "../Editor/PluginEditor.h"

//==============================================================================
GayPolyCommunistAudioProcessor::GayPolyCommunistAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
    apvts.state.addListener(this);
}

GayPolyCommunistAudioProcessor::~GayPolyCommunistAudioProcessor()
{
    apvts.state.removeListener(this);
}

//==============================================================================
const juce::String GayPolyCommunistAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GayPolyCommunistAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GayPolyCommunistAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GayPolyCommunistAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GayPolyCommunistAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GayPolyCommunistAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GayPolyCommunistAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GayPolyCommunistAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GayPolyCommunistAudioProcessor::getProgramName (int index)
{
    return {};
}

void GayPolyCommunistAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GayPolyCommunistAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) 
{
    dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 2 };
    synth.prepare(spec);
    update();
}

void GayPolyCommunistAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GayPolyCommunistAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GayPolyCommunistAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (mustUpdateProcessing.get())
        update();

 
    if (processing.get() && checkVoices())
    {
        synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }
    
    for (int chan = 0; chan < buffer.getNumChannels(); chan++)
    {
        RMS += buffer.getRMSLevel(chan, 0, buffer.getNumSamples());
    }

    RMS /= buffer.getNumChannels(); // rms measured channel by channel, this averages their value
}

//==============================================================================
bool GayPolyCommunistAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GayPolyCommunistAudioProcessor::createEditor()
{
    return new GayPolyCommunistAudioProcessorEditor (*this);
}

//==============================================================================
void GayPolyCommunistAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{

}

void GayPolyCommunistAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void GayPolyCommunistAudioProcessor::update()
{
    synth.update(apvts);
    mustUpdateProcessing = false;
}

juce::AudioProcessorValueTreeState::ParameterLayout GayPolyCommunistAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK 1", "Attack 1", juce::NormalisableRange<float>(0.0f, 3.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY 1", "Decay 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN 1", "Sustain 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE 1", "Release 1", juce::NormalisableRange<float>(0.0f, 3.0f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK 2", "Attack 2", juce::NormalisableRange<float>(0.0f, 3.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY 2", "Decay 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN 2", "Sustain 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE 2", "Release 2", juce::NormalisableRange<float>(0.0f, 3.0f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO 1 Depth", "LFO 1 Depth", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO 1 Rate", "LFO 1 Rate", juce::NormalisableRange<float>(0.0f, 20.0f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO 2 Depth", "LFO 2 Depth", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO 2 Rate", "LFO 2 Rate", juce::NormalisableRange<float>(0.0f, 20.0f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wavetable", "Wavetable", NormalisableRange<float>(0.0f, 1.0f, 0.0001f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain", "Gain", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter Freq", "Filter Freq", NormalisableRange<float>(100.0f, 15000.0f, 0.001f, 0.5f), 500.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter Res", "Filter Res", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Amp LFO 1", "Amp LFO 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filt LFO 1", "Filt LFO 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch LFO 1", "Pitch LFO 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave LFO 1", "Wave LFO 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Amp LFO 2", "Amp LFO 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filt LFO 2", "Filt LFO 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch LFO 2", "Pitch LFO 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave LFO 2", "Wave LFO 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Amp Env 1", "Amp Env 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filt Env 1", "Filt Env 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch Env 1", "Pitch Env 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave Env 1", "Wave Env 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Amp Env 2", "Amp Env 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filt Env 2", "Filt Env 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch Env 2", "Pitch Env 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave Env 2", "Wave Env 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.f));

    return { params.begin(), params.end() };
}

WaveTableVector& GayPolyCommunistAudioProcessor::getWaveVector()
{

    // only passing one wavetable because the visualizer only needs one
    if (auto voice = dynamic_cast<GayVoice*>(synth.getVoice(0)))
    {
        return voice->getTable();
    }
}

GaySynth& GayPolyCommunistAudioProcessor::getSynth()
{
    return synth;
}

void GayPolyCommunistAudioProcessor::shouldProcess(bool isProcessing)
{
    processing = isProcessing;
}

bool GayPolyCommunistAudioProcessor::checkVoices()
{
    bool allLoaded = false;
    for (int voiceNum = 0; voiceNum < synth.getNumVoices(); voiceNum++)
    {
        if (auto voice = dynamic_cast<GayVoice*>(synth.getVoice(voiceNum)))
        {
            allLoaded = voice->getTable().isFinishedLoading();
        }
    }
    
    return allLoaded;
}

float GayPolyCommunistAudioProcessor::getRMS()
{
    return RMS;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GayPolyCommunistAudioProcessor();
}
