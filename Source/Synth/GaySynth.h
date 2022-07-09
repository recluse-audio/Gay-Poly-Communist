/*
  ==============================================================================

    GaySynth.h
    Created: 20 Sep 2021 2:36:08pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../WaveTable/WaveTableVector.h"

#include "GayVoice.h"

class GaySynth : public MPESynthesiser
{
public:
    static constexpr size_t maxNumVoices = 4;

    public enum class WaveTableVectors
    {
        Oscillator1 = 1,
        Oscillator2,
        LFO1,
        LFO2,
        LFO3
    };
    
    GaySynth()
    {
        initWaveVectors();
        
        for (size_t i = 0; i <= maxNumVoices; ++i)
        {
            addVoice(new GayVoice());
        }

        setVoiceStealingEnabled(true);
    }
    
    
    ~GaySynth() {}

    // Setting 
    void initWaveVectors()
    {
        oscVector1(100);
        oscVector2(100);
        lfoVector1(1);
        lfoVector2(1);
        lfoVector3(1);
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

    
    void loadWaveVectorFromBufferArray(juce::OwnedArray<juce::AudioBuffer<float>> newBufferArray, GaySynth::WaveTableVectors targetWaveTable)
    {
        auto waveTableVectorToChange = getWaveTableVector(targetWaveTable);
        
        waveTableVectorToChange.loadVectorFromBufferArray(newBufferArray);
    }
    
    
    
    WaveTableVector& getWaveTableVector(GaySynth::WaveTableVectors targetWaveTable)
    {
        switch(targetWaveTable)
        {
            case GaySynth::WaveTableVectors::Oscillator1:
                return oscVector1;
                
            case GaySynth::WaveTableVectors::Oscillator2:
                return oscVector2;
                
            case GaySynth::WaveTableVectors::LFO1:
                return lfoVector1;
                
            case GaySynth::WaveTableVectors::LFO2:
                return lfoVector2;
                
            case GaySynth::WaveTableVectors::LFO3:
                return lfoVector3;
        }
    }
    
    
private:
    GayVoice* myVoice; // This is used to check the type of voice being used by the synth
    
    WaveTableVector oscVector1;
    WaveTableVector oscVector2;
    WaveTableVector lfoVector1;
    WaveTableVector lfoVector2;
    WaveTableVector lfoVector3;


    void renderNextSubBlock(juce::AudioBuffer<float>& outputAudio, int startSample, int numSamples) override
    {
        MPESynthesiser::renderNextSubBlock(outputAudio, startSample, numSamples);
    }
};
 
