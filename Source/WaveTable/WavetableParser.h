/*
  ==============================================================================

    WavetableParser.h
    Created: 6 Jan 2022 1:18:25pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include "Yin.h"
#include "../Processor/PluginProcessor.h"

class WavetableParser
{
public:
    WavetableParser(GayPolyCommunistAudioProcessor& p) : audioProcessor(p)
    {
        formatManager.registerBasicFormats();
        yinObject = std::make_unique<PitchYIN>((int)audioProcessor.getSampleRate(), 48000);
        waveBuffer.setSize(1, 480000); // one channel, ten seconds
    }

    ~WavetableParser() {}

    void loadFile(File waveFile)
    {
        std::unique_ptr<AudioFormatReader> formatReader{ formatManager.createReaderFor(waveFile) };
        formatReader->read(&waveBuffer, 0, 480000, 0, true, false); // writes waveFile into our waveBuffer, up to 10 seconds
        calculatePeriod();
    }

    // parses and then passes wavetable vector to processor/synth/oscillators/wavetablevector/wavetable
    void loadFileToOsc(File waveFile, int oscNum)
    {
        std::unique_ptr<AudioFormatReader> formatReader{ formatManager.createReaderFor(waveFile) };
        waveBuffer.clear();
        waveBuffer.setSize(1, formatReader->lengthInSamples);
        formatReader->read(&waveBuffer, 0, formatReader->lengthInSamples - 1, 0, true, false); // writes waveFile into our waveBuffer, up to 10 seconds
        calculatePeriod();

        // find and load 16 chunks of waves
        int numWaves = 16; // number of waves to split into
        int waveChunk = formatReader->lengthInSamples / numWaves; // samples between each 'sampling' of a waveform
        for (int i = 0; i < numWaves; i++)
        {
            int startPos = waveChunk * i;
            fillSingleCycle(startPos);
            audioProcessor.loadTableFromBuffer(singleCycle, oscNum);
        }
        
    }

    void calculatePeriod()
    {
        auto buffRead = waveBuffer.getReadPointer(0);
        auto freq = yinObject->getPitchInHz(buffRead);
        period = audioProcessor.getSampleRate() / freq;
    }

    void fillSingleCycle(int startPos)
    {
        int p = period * 2.f; // getting better results with this instead of just 1 period
        singleCycle.clear();
        singleCycle.setSize(1, p);

        auto buffWrite = singleCycle.getArrayOfWritePointers();
        auto buffRead = waveBuffer.getArrayOfReadPointers();

        for (int i = 0; i < p; i++)
        {
            buffWrite[0][i] = buffRead[0][startPos + i];
        }

        float gain = 1.f + (1.f - singleCycle.getRMSLevel(0, 0, singleCycle.getNumSamples())); // gain increas if rms decreases
        singleCycle.applyGain(gain);
    }

    // replaced by calculate single cycle / probably gonna delete
    AudioBuffer<float>& getWaveBuffer()
    {
        int p = period * 2.f; // getting better results with this instead of just 1 period
        singleCycle.clear();
        singleCycle.setSize(1, p);

        auto buffWrite = singleCycle.getArrayOfWritePointers();
        auto buffRead = waveBuffer.getArrayOfReadPointers();

        int startPos = p * 4; // getting into the 

        for (int i = 0; i < p; i++)
        {
            buffWrite[0][i] = buffRead[0][startPos + i];
        }
       

        return singleCycle;
    }

private:
    AudioFormatManager formatManager;
    AudioBuffer<float> waveBuffer; // for pitch analysis and dissection
    AudioBuffer<float> singleCycle;  // 

    float period = 0.f;
    int tableSize = 2048;

    std::unique_ptr<PitchYIN> yinObject;

    GayPolyCommunistAudioProcessor& audioProcessor;
};