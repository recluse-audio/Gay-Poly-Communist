/*
  ==============================================================================

    WaveTable.h
    Created: 27 May 2021 7:42:15am
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class WaveTable
{
public:
    /*
        TO DO: run this as a []() function*
    */
    WaveTable(int lengthInSamples = 2048) : waveBuffer(1, lengthInSamples)
    {
        tableSize = lengthInSamples;
    }



    ~WaveTable() {}

    void prepare(double sampleRate)
    {
        mSampleRate = sampleRate;
    }


    void createSineTable()
    {
        waveBuffer.setSize(1, (int)tableSize);
        waveBuffer.clear();

        auto* buffWrite = waveBuffer.getWritePointer(0);

        auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
        auto pi = juce::MathConstants<double>::pi;
        double currentAngle = -pi;

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            float sample;
            sample = std::sin(currentAngle);

            buffWrite[i] += sample;
            currentAngle += angleDelta;
        }


        buffWrite[tableSize] = buffWrite[0];
    }

    void setFrequency(float freq)
    {
        // 
        auto tableSizeOverSampleRate = (float)waveBuffer.getNumSamples() / mSampleRate;

        // table increment per sample
        phaseIncrement = freq * tableSizeOverSampleRate;
    }

    float getNextSample()
    {
        auto index0 = (unsigned int)currentIndex; // cast to int

        auto frac = currentIndex - (float)index0; // overlap to next index
        auto* table = waveBuffer.getReadPointer(0); // read from sample array

        auto value0 = table[index0];
        auto value1 = table[index0 + 1];

        currentSample = value0 + (frac * (value1 - value0)); // simple interpolation

        currentIndex += phaseIncrement;

        if (currentIndex > (float)tableSize)
        {
            currentIndex = 0;
        }

        return currentSample;
    }

    // way of getting sample without increment
    float getCurrentSample()
    {
        return currentSample;
    }



    AudioBuffer<float>& getBuffer()
    {
        return waveBuffer;
    }

    void passBuffer(AudioBuffer<float>& newTable)
    {
        waveBuffer = newTable;
    }

    void setGain(float gainVal)
    {
        gain = gainVal;
    }
private:
    juce::AudioBuffer<float> waveBuffer;
    int tableSize = 2048;
    double mSampleRate = 48000;
    float phaseIncrement = 0.f, currentIndex = 0.f, currentSample = 0.f;
    float gain = 1.f;
};