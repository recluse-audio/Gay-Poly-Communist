/*
  ==============================================================================

    WaveTable.cpp
    Created: 27 May 2021 7:42:15am
    Author:  ryand

  ==============================================================================
*/

//#include "WaveTable.h"

WaveTable::WaveTable()
 : sineTable(1, 1024), sincTable(1, 1024), triTable(1, 1024), sawTable(1, 1024)
{
    createTables();
    formatManager.registerBasicFormats();

    loadTableFromDisk();
}

WaveTable::~WaveTable()
{
}

void WaveTable::prepare(double sRate)
{
    sampleRate = sRate;
    
}

/*
    TO DO: Handle varied size wavetables.
*/

void WaveTable::loadTableFromDisk()
{
    auto waveFolders = juce::File("D:/WaveTables/Echo Sound Works Core Tables/Artie/").findChildFiles(2, true, "*.wav");

    auto waveFile = juce::File(waveFolders[1].getFullPathName());
    std::unique_ptr<juce::AudioFormatReader> formatReader{ formatManager.createReaderFor(waveFile) };
    formatReader->read(&sineTable, 0, tableSize, 0, true, false);
    DBG(formatReader->lengthInSamples);


    //for (int i = 0; i < waveFolders.size(); i++)
    //{
    //    auto waveFile = juce::File(waveFolders[i].getFullPathName());
    //    std::unique_ptr<juce::AudioFormatReader> formatReader{ formatManager.createReaderFor(waveFile) };
    //    formatReader->read(&sineTable, 0, tableSize, 0, true, false);
    //}
}

void WaveTable::createTables()
{
    createSineTable();
    createSincTable();
    createTriTable();
    createSawTable();
}

void WaveTable::setTable(float selection)
{
    normalWaveIndex = selection; // 0  to 1 version of waveIndex
    floatWaveIndex = NormalisableRange<float>(0.f, 3.f, 0.001f, 1.f).convertFrom0to1(selection); // converted to size of array
    intWaveIndex = floatWaveIndex; // cast to int
    waveFraction = floatWaveIndex - intWaveIndex; // overlap
}

juce::AudioBuffer<float>& WaveTable::getTable()
{
    switch (intWaveIndex)
    {
    case 0:  return sineTable;
        break;
    case 1: return triTable;
        break;
    case 2: return sawTable;
        break;
    case 3: return sincTable;
        break;
    default: return sineTable;
        break;
    }
}

juce::AudioBuffer<float>& WaveTable::getNextTable()
{
    switch (intWaveIndex + 1)
    {
    case 0:  return sineTable;
        break;
    case 1: return triTable;
        break;
    case 2: return sawTable;
        break;
    case 3: return sincTable;
        break;
    case 4: return sineTable;
        break;

    default: return sineTable;
        break;
    }
}

float WaveTable::getWaveFraction() // used to calculate interpolation between waveshapes
{
    return waveFraction;
}

float WaveTable::getWaveIndex()
{
    return 0.0f;
}

void WaveTable::setFreq(float freq)
{
    auto tableSizeOverSampleRate = (float)tableSize / sampleRate;
    /*
        I call it 'readDelta' here
    */
    readDelta = freq * tableSizeOverSampleRate;
}

float WaveTable::getNextSample()
{
    auto index0 = (unsigned int)readIndex;
    auto index1 = index0 + 1;

    auto frac = readIndex - (float)index0;

    auto* table1 = getTable().getReadPointer(0);
    auto value0 = table1[index0];
    auto value1 = table1[index1];

    auto* table2 = getNextTable().getReadPointer(0);
    auto value2 = table2[index0];
    auto value3 = table2[index1];

    auto waveSample1 = value0 + (frac * (value1 - value0));
    waveSample1 *= 1.0 - waveFraction; // waveTable.getWaveFraction() should equal 0.f if no interp should occur

    auto waveSample2 = value2 + (frac * (value3 - value2));
    waveSample2 *= waveFraction;

    auto currentSample = waveSample1 + waveSample2;
    readIndex += readDelta;

    if (readIndex >= tableSize)
    {
        readIndex = readIndex - tableSize;
    }
    return currentSample;
}

void WaveTable::incrementReadIndex()
{
    readIndex += readDelta;
    if (readIndex >= tableSize)
    {
        readIndex = readIndex - tableSize;
    }
}

void WaveTable::createSineTable()
{
    sineTable.setSize(1, (int)tableSize + 1);
    sineTable.clear();

    auto* buffWrite = sineTable.getWritePointer(0);

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

void WaveTable::createSincTable()
{
    sincTable.setSize(1, (int)tableSize + 1);
    sincTable.clear();

    auto* buffWrite = sincTable.getWritePointer(0);

    auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
    auto pi = juce::MathConstants<double>::pi;
    double currentAngle = -pi;

    for (unsigned int i = 0; i < tableSize; ++i)
    {
        float sample;

        if (currentAngle == 0)
        {
            sample = 1;
        }
        else
        {
            sample = std::sin(pi * currentAngle) / (pi * currentAngle);
            buffWrite[i] += sample;

        }

        currentAngle += angleDelta;
    }

    buffWrite[tableSize] = buffWrite[0];
}

void WaveTable::createTriTable()
{
    triTable.setSize(1, (int)tableSize + 1);
    triTable.clear();
    auto* buffWrite = triTable.getWritePointer(0);


    int harmonics[] = { 1, 3, 5, 7, 9, 11, 13, 15 };

    for (auto harmonic = 0; harmonic < juce::numElementsInArray(harmonics); harmonic += 2)
    {
        auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1) * harmonics[harmonic];
        auto currentAngle = 0.0;

        float harmonicAmp = 0.9f / (harmonics[harmonic] * harmonics[harmonic]);

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            auto sample = std::sin(currentAngle);
            buffWrite[i] += (float)sample * harmonicAmp; //harmonicWeights[harmonic];
            currentAngle += angleDelta;
        }
    }

    for (auto harmonic = 1; harmonic < juce::numElementsInArray(harmonics); harmonic += 2)
    {
        auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1) * harmonics[harmonic];
        auto pi = juce::MathConstants<double>::pi;
        auto currentAngle = pi;

        float harmonicAmp = 0.9f / (harmonics[harmonic] * harmonics[harmonic]);

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            auto sample = std::sin(currentAngle);
            buffWrite[i] += (float)sample * harmonicAmp;
            currentAngle += angleDelta;
        }
    }

    buffWrite[tableSize] = buffWrite[0];
}

void WaveTable::createSawTable()
{
    sawTable.setSize(1, (int)tableSize + 1);
    sawTable.clear();
    auto* buffWrite = sawTable.getWritePointer(0);

    int harmonics = 32;

    for (auto i = 0; i < harmonics; i++)
    {
        float harmonic = i + 1;
        auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1) * harmonic;
        auto currentAngle = 0.0;

        float harmonicAmp = 0.9f / (harmonic * harmonic);

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            auto sample = std::sin(currentAngle);
            buffWrite[i] += (float)sample * harmonicAmp; //harmonicWeights[harmonic];
            currentAngle += angleDelta;
        }
    }

    buffWrite[tableSize] = buffWrite[0];
}
