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
    // Construct synth with default buffer array to 
    synth = std::make_unique<GaySynth>(loadDefaultWaves());
    
    apvts.state.addListener(this);

    //waveDatabase.loadFiles();
    update();
}

GayPolyCommunistAudioProcessor::~GayPolyCommunistAudioProcessor()
{
    apvts.state.removeListener(this);
}

//***********************
//
juce::Array<juce::AudioBuffer<float>> GayPolyCommunistAudioProcessor::loadDefaultWaves()
{
    auto bufferArray = waveLoader.getBufferArrayFromFilePath(GPC_CONSTANTS::defaultFolderPath);
    return bufferArray;
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
    synth->prepare(spec);
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

    if (noteOn.get())
    {
        MidiMessage m(MidiMessage::noteOn(1, 36, 1.f));
        m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
        midiMessages.addEvent(m, 128); // put at end of buffer because I don't know where to put it
        noteOn = false;
    }
    if (noteOff.get())
    {
        MidiMessage m(MidiMessage::noteOff(1, 36, 0.5f));
        midiMessages.addEvent(m, 128);
        noteOff = false;
    }
    

    if (processing.get() && checkVoices())
    {
        synth->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
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
    XmlElement xmlRoot( "SessionData" );

    // All the parameters
    std::unique_ptr<juce::XmlElement> valueTreeXml = apvts.copyState().createXml();
    valueTreeXml->setTagName("Parameter");
    
    // Path to current wavetable
    std::unique_ptr<juce::XmlElement> waveTablePathXml = std::make_unique<juce::XmlElement>();
    valueTreeXml->setTagName("Wavetable");
    
    // Path to current preset
    std::unique_ptr<juce::XmlElement> presetPathXml = std::make_unique<juce::XmlElement>();
    valueTreeXml->setTagName("Preset");
    
    copyXmlToBinary(xmlRoot, destData);

}

void GayPolyCommunistAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    juce::ValueTree copyState = juce::ValueTree::fromXml(*xml.get());
    apvts.replaceState(copyState);
}

void GayPolyCommunistAudioProcessor::update()
{
    synth->update(apvts);
    mustUpdateProcessing = false;
}

juce::AudioProcessorValueTreeState::ParameterLayout GayPolyCommunistAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // mods
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK 1", "Attack 1", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f, 0.5f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY 1", "Decay 1", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01, 0.5f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN 1", "Sustain 1", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE 1", "Release 1", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f, 0.5f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth 1", "LFO Depth 1", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate 1", "LFO Rate 1", juce::NormalisableRange<float>(0.0f, 20.0f), 10.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK 2", "Attack 2", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f, 0.5f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY 2", "Decay 2", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN 2", "Sustain 2", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE 2", "Release 2", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f, 0.5f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth 2", "LFO Depth 2", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate 2", "LFO Rate 2", juce::NormalisableRange<float>(0.0f, 20.0f), 10.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK 3", "Attack 3", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f, 0.5f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY 3", "Decay 3", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN 3", "Sustain 3", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE 3", "Release 3", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f, 0.5f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth 3", "LFO Depth 3", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate 3", "LFO Rate 3", juce::NormalisableRange<float>(0.0f, 20.0f), 10.f));

    // osc params, currently can only assign one lfo and one env to each param
    // TO DO: add scale params for lfo and env
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 1", "Gain 1", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 1 LFO Source", "Gain 1 LFO Source", 0.f, 3.f, 0.f));// 0 = no modulator1
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 1 LFO Scale", "Gain 1 LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 1 Env Source", "Gain 1 Env Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 1 Env Scale", "Gain 1 Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 1", "Pitch 1", NormalisableRange<float>(-1.0f, 1.0f, 0.001f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 1 LFO Source", "Pitch 1 LFO Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 1 LFO Scale", "Pitch 1 LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 1 Env Source", "Pitch 1 Env Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 1 Env Scale", "Pitch 1 Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 1 Position", "Wave 1 Position", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 1 LFO Source", "Wave 1 LFO Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 1 LFO Scale", "Wave 1 LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 1 Env Source", "Wave 1 Env Source", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 1 Env Scale", "Wave 1 Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));


    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 2", "Gain 2", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 2 LFO Source", "Gain 2 LFO Source", 0.f, 3.f, 0.f));// 0 = no modulator1
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 2 LFO Scale", "Gain 2 LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 2 Env Source", "Gain 2 Env Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Gain 2 Env Scale", "Gain 2 Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 2", "Pitch 2", NormalisableRange<float>(-1.0f, 1.0f, 0.001f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 2 LFO Source", "Pitch 2 LFO Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 2 LFO Scale", "Pitch 2 LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 2 Env Source", "Pitch 2 Env Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Pitch 2 Env Scale", "Pitch 2 Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 2 Position", "Wave 2 Position", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.65f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 2 LFO Source", "Wave 2 LFO Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 2 LFO Scale", "Wave 2 LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 2 Env Source", "Wave 2 Env Source", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wave 2 Env Scale", "Wave 2 Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));


    // Filter params (held in voice class rather than individual oscillators
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter Freq", "Filter Freq", NormalisableRange<float>(100.0f, 15000.0f, 0.001f, 0.5f), 500.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter LFO Source", "Filter LFO Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter LFO Scale", "Filter LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter Env Source", "Filter Env Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter Env Scale", "Filter Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter Res", "Filter Res", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Res LFO Source", "Res LFO Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Res LFO Scale", "Res LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Res Env Source", "Res Env Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Res Env Scale", "Res Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter Drive", "Filter Drive", NormalisableRange<float>(1.0f, 10.0f, 0.001f), 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Drive LFO Source", "Drive LFO Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Drive LFO Scale", "Drive LFO Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Drive Env Source", "Drive Env Source", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Drive Env Scale", "Drive Env Scale", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("Filter Mode", "Filter Mode", 0.f, 2.f, 1.f));// 1 = LPF

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate Env Source 1", "LFO Rate Env Source 1", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate Env Scale 1", "LFO Rate Env Scale 1", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate Env Source 2", "LFO Rate Env Source 2", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate Env Scale 2", "LFO Rate Env Scale 2", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate Env Source 3", "LFO Rate Env Source 3", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Rate Env Scale 3", "LFO Rate Env Scale 3", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth Env Source 1", "LFO Depth Env Source 1", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth Env Scale 1", "LFO Depth Env Scale 1", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth Env Source 2", "LFO Depth Env Source 2", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth Env Scale 2", "LFO Depth Env Scale 2", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth Env Source 3", "LFO Depth Env Source 3", 0, 3, 0));// 0 = no modulator
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFO Depth Env Scale 3", "LFO Depth Env Scale 3", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    return { params.begin(), params.end() };
}

WaveTableVector& GayPolyCommunistAudioProcessor::getWaveVector(int oscNumber)
{

    // only passing one wavetable because the visualizer only needs one
    if (auto voice = dynamic_cast<GayVoice*>(synth->getVoice(0)))
    {
        return voice->getTable(oscNumber);
    }
}

GaySynth& GayPolyCommunistAudioProcessor::getSynth()
{
    return *synth.get();
}

void GayPolyCommunistAudioProcessor::shouldProcess(bool isProcessing)
{
    processing = isProcessing;
}

bool GayPolyCommunistAudioProcessor::checkVoices()
{
    bool allLoaded = false;
    for (int voiceNum = 0; voiceNum < synth->getNumVoices(); voiceNum++)
    {
        if (auto voice = dynamic_cast<GayVoice*>(synth->getVoice(voiceNum)))
        {
            allLoaded = voice->getTable(1).isFinishedLoading(); // checking that both Oscs are done loading
            allLoaded = voice->getTable(2).isFinishedLoading();

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

float GayPolyCommunistAudioProcessor::getLFOSource()
{
    return lfoSource;
}

// returns which envelope is being mapped.  Functions that call this 
float GayPolyCommunistAudioProcessor::getEnvSource()
{
    return envSource;
}

void GayPolyCommunistAudioProcessor::setLFOSource(int sourceNum)
{
    lfoSource = sourceNum;
}

void GayPolyCommunistAudioProcessor::setEnvSource(int sourceNum)
{
    envSource = sourceNum;
}

void GayPolyCommunistAudioProcessor::setMappingLFO(bool lfoMapping, float source)
{
    lfoSource = source;
    mappingLFO = lfoMapping;
}

void GayPolyCommunistAudioProcessor::setMappingEnv(bool envMapping, float source)
{
    envSource = source;
    mappingEnv = envMapping;
}

bool GayPolyCommunistAudioProcessor::isMappingLFO()
{
    return mappingLFO;
}

bool GayPolyCommunistAudioProcessor::isMappingEnv()
{
    return mappingEnv;
}

int GayPolyCommunistAudioProcessor::getMappingLFO()
{
    return 0;
}


void GayPolyCommunistAudioProcessor::triggerMidi(bool isNoteOn)
{
    if (isNoteOn)
    {
        noteOn = true;
        noteOff = false;
    }
    else
    {
        noteOn = false;
        noteOff = true;
    }


}

WaveLoader& GayPolyCommunistAudioProcessor::getWaveLoader()
{
    return waveLoader;
}

// drag/drop returns string array, not sure why
void GayPolyCommunistAudioProcessor::loadWaveVectorFromFilePaths(const StringArray& filePaths, GaySynth::WaveTableVectorIds targetOscillator)
{
    // I think I have to iterate because each of these could be a directory
    for (auto filePath : filePaths)
    {
        auto bufferArray = waveLoader.getBufferArrayFromFilePath(filePath);
        
        this->_loadWaveVectorFromBufferArray(bufferArray, targetOscillator);
    }
}



void GayPolyCommunistAudioProcessor::loadWaveVectorFromIndex(int index, GaySynth::WaveTableVectorIds targetOscillator)
{
    auto bufferArray = waveLoader.getBufferArrayFromIndex(index);
    
    this->_loadWaveVectorFromBufferArray(bufferArray, targetOscillator);
}
       

//*************************
// PRIVATE FUNCTIONS

void GayPolyCommunistAudioProcessor::_loadWaveVectorFromBufferArray( juce::Array<juce::AudioBuffer<float>> waveBufferArray,
                                                                    GaySynth::WaveTableVectorIds targetOscillator)
{
    synth->loadWaveVectorFromBufferArray(waveBufferArray, targetOscillator);
}


void GayPolyCommunistAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) 
{
    mustUpdateProcessing = true;
}
