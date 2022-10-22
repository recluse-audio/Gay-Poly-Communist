/*
  ==============================================================================

    GaySynth.h
    Created: 20 Sep 2021 2:36:08pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WaveTableVector.h"

#include "GayVoice.h"

class GaySynth : public MPESynthesiser
{
public:
    static constexpr size_t maxNumVoices = 4;

    enum class WaveTableVectorIds
    {
        Oscillator1 = 1,
        Oscillator2,
        LFO1,
        LFO2,
        LFO3
    };
    
    GaySynth(juce::Array<juce::AudioBuffer<float>> bufferArray)
    {
        initWaveVectors(bufferArray);
        
        for (size_t i = 0; i <= maxNumVoices; ++i)
        {
            addVoice(new GayVoice(*oscVector1.get(), *oscVector2.get(),
								  *lfoVector1.get(), *lfoVector2.get(), *lfoVector3.get()));
        }

        setVoiceStealingEnabled(true);
    }
    
    
    ~GaySynth() {}

    // Setting 
    void initWaveVectors(juce::Array<juce::AudioBuffer<float>> bufferArray)
    {
        oscVector1 = std::make_unique<WaveTableVector>(bufferArray);
        oscVector2 = std::make_unique<WaveTableVector>(bufferArray);
        lfoVector1 = std::make_unique<WaveTableVector>(bufferArray);
        lfoVector2 = std::make_unique<WaveTableVector>(bufferArray);
        lfoVector3 = std::make_unique<WaveTableVector>(bufferArray);
    }
    
    
    void prepare(dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate(spec.sampleRate);

        for (auto* v : voices)
        {
            dynamic_cast<GayVoice*> (v)->prepare(spec);
        }

    }

    
    void update(AudioProcessorValueTreeState& apvts)
    {
        for (int i = 0; i < getNumVoices(); i++)
        {
            if ((myVoice = dynamic_cast<GayVoice*>(getVoice(i))))
            {
                myVoice->update(apvts);
            }
        }
    }

    //*******************
    // Pass in an array of audio buffers to be loaded into one of the wavetables vectors
    void loadWaveVectorFromBufferArray(juce::Array<juce::AudioBuffer<float>> newBufferArray, GaySynth::WaveTableVectorIds targetWaveTable)
    {
        switch(targetWaveTable)
        {
            case GaySynth::WaveTableVectorIds::Oscillator1:
                return oscVector1->loadVectorFromBufferArray(newBufferArray);
                
            case GaySynth::WaveTableVectorIds::Oscillator2:
                return oscVector2->loadVectorFromBufferArray(newBufferArray);
                
            case GaySynth::WaveTableVectorIds::LFO1:
                return lfoVector1->loadVectorFromBufferArray(newBufferArray);
                
            case GaySynth::WaveTableVectorIds::LFO2:
                return lfoVector2->loadVectorFromBufferArray(newBufferArray);
                
            case GaySynth::WaveTableVectorIds::LFO3:
                return lfoVector3->loadVectorFromBufferArray(newBufferArray);
        }
    }
    
    
    
    WaveTableVector* getWaveTableVector(GaySynth::WaveTableVectorIds targetWaveTable)
    {
        switch(targetWaveTable)
        {
            case GaySynth::WaveTableVectorIds::Oscillator1:
                return oscVector1.get();
                
            case GaySynth::WaveTableVectorIds::Oscillator2:
                return oscVector2.get();
                
            case GaySynth::WaveTableVectorIds::LFO1:
                return lfoVector1.get();
                
            case GaySynth::WaveTableVectorIds::LFO2:
                return lfoVector2.get();
                
            case GaySynth::WaveTableVectorIds::LFO3:
                return lfoVector3.get();
        }
    }
    
    
private:
    GayVoice* myVoice; // This is used to check the type of voice being used by the synth
    
    std::unique_ptr<WaveTableVector> oscVector1;
    std::unique_ptr<WaveTableVector> oscVector2;
    std::unique_ptr<WaveTableVector> lfoVector1;
    std::unique_ptr<WaveTableVector> lfoVector2;
    std::unique_ptr<WaveTableVector> lfoVector3;


    void renderNextSubBlock(juce::AudioBuffer<float>& outputAudio, int startSample, int numSamples) override
    {
        MPESynthesiser::renderNextSubBlock(outputAudio, startSample, numSamples);
    }
};
 
