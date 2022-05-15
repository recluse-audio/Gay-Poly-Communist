/*
  ==============================================================================

    GaySynth.h
    Created: 20 Sep 2021 2:36:08pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Processor/PluginProcessor.h"
#include "GayVoice.h"

class GaySynth : public MPESynthesiser
{
public:
    static constexpr size_t maxNumVoices = 4;

    GaySynth() 
    {
        for (size_t i = 0; i <= maxNumVoices; ++i)
        {
            addVoice(new GayVoice());
        }

        setVoiceStealingEnabled(true);
    }
    ~GaySynth() {}

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

private:
    GayVoice* myVoice; // This is used to check the type of voice being used by the synth ( and then to send the apvts to it )

    void renderNextSubBlock(juce::AudioBuffer<float>& outputAudio, int startSample, int numSamples) override
    {
        MPESynthesiser::renderNextSubBlock(outputAudio, startSample, numSamples);
    }
};
 