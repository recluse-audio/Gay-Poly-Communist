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
        waveBuffer.setSize(1, (int)tableSize + 1);
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

    float getNextSample()
    {
        auto index0 = (unsigned int)currentIndex;

        auto frac = currentIndex - (float)index0;
        auto* table = waveBuffer.getReadPointer(0);

        auto value0 = table[index0];
        auto value1 = table[index0 + 1];

        auto waveSample = value0 + (frac * (value1 - value0));

        currentIndex += tableDelta;

        if (currentIndex > (float)tableSize)
        {
            currentIndex = 0;
        }

        return waveSample;
    }

    void setFrequency(float freq)
    {
        auto tableSizeOverSampleRate = (float)waveBuffer.getNumSamples() / mSampleRate;
       // auto tableScale = // scaling according to how 
        tableDelta = freq * tableSizeOverSampleRate;
    }

    AudioBuffer<float>& getBuffer()
    {
        return waveBuffer;
    }

    void passBuffer(AudioBuffer<float>& newTable)
    {
        waveBuffer = newTable;
    }

private:
    juce::AudioBuffer<float> waveBuffer;
    int tableSize = -1;
    double mSampleRate = -1;
    float tableDelta = -1, currentIndex = 0;

};