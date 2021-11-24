/*
  ==============================================================================

    GayOscillator.h
    Created: 20 Sep 2021 2:35:36pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../WaveTable/WaveTableVector.h"


//==============================================================================
template <typename Type>
class GayOscillator
{
public:
    //==============================================================================
    // each oscillator
    GayOscillator(){}

    ~GayOscillator(){}

    void prepare(const dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);
        sampleRate = spec.sampleRate;
        waveVector.prepare(sampleRate);
    }

    /*
        Normalized value that represents interpolation of wavetable vector
    */
    void setWaveform(float waveform)
    {
        waveVector.setWave(waveform);
    }

    //==============================================================================
    void setFrequency(Type newValue, bool force = false)
    {
        waveVector.setFrequency(newValue);
        // processorChain.template get<oscIndex>().setFrequency (newValue, force);
    }

    void setLevel(Type newValue)
    {
        // processorChain.template get<gainIndex>().setGainLinear (newValue);
    }

    void reset() noexcept
    {
        processorChain.reset();
    }

    //==============================================================================
    /*
    *   This part is for use in a processor chain, here we are just writing directly, but could utilize a processor chain
    *   for each osc
    */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto&& outBlock = context.getOutputBlock();
        auto blockToUse = tempBlock.getSubBlock(0, outBlock.getNumSamples());

        for (size_t i = 0; i < outBlock.getNumSamples(); i++)
        {
            // write to sample here because wavetable is mono
            auto sample = waveVector.getNextSample();

            for (size_t j = 0; j < outBlock.getNumChannels(); j++)
            {
                auto* dst = outBlock.getChannelPointer(j);
                dst[i] += sample;
            }
        }
    }

    float getNextSample()
    {
        return waveVector.getNextSample();
    }

    /*
        Used to down sample lfo's and such, I could just use the wavetable but I want to 
        bve consistent and use this for all oscillators including the lfo0's
    */
    float getSampleFromIncrement(int increment)
    {
        return waveVector.incrementReadIndex(increment);
    }

    //==============================================================================

    WaveTableVector& getWaveVector()
    {
        return waveVector;
    }

    void loadTables(StringRef filePath)
    {
        waveVector.loadTables(filePath);
    }
private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    //WaveTable waveTable;
    WaveTableVector waveVector;
    double sampleRate = -1;


};

