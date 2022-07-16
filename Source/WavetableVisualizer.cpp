/*
  ==============================================================================

    WavetableVisualizer.cpp
    Created: 6 Nov 2020 4:34:21pm
    Author:  Billie (Govnah) Jean

  ==============================================================================
*/

#include "WavetableVisualizer.h"


//==============================================================================
WavetableVisualizer::WavetableVisualizer(int osc, GayPolyCommunistAudioProcessor& p, WaveTableVector& w) 
    : oscNum(osc), processor(p), waveVector(w)
{
    setSize(400, 150);
    setNewWaveColour(juce::Colours::white);

    waveParser = std::make_unique<WavetableParser>(processor);
}

WavetableVisualizer::~WavetableVisualizer()
{
}

void WavetableVisualizer::paint (juce::Graphics& g)
{
    auto w = getLocalBounds().getWidth();
    auto h = getLocalBounds().getHeight();
    auto frameHalf = h / 2;

    juce::Rectangle<int> waveFrame = {0,0, w, h};

    g.setColour({ 179, 88, 100 });
    g.fillRoundedRectangle(waveFrame.toFloat(), 5.f);
   
    g.setColour({ 192, 172, 119 });
    g.drawRoundedRectangle(waveFrame.toFloat(), 5.f, 3.f);

    if (waveVector.isFinishedLoading())
    {
        Path wavePath;

        wavePath.startNewSubPath(0, frameHalf);

        RangedAudioParameter* waveParam;
        if (oscNum == 1)
        {
            waveParam = processor.getValueTree().getParameter("Wave 1 Position");
        }
        else
        {
            waveParam = processor.getValueTree().getParameter("Wave 2 Position");

        }
        auto waveVal = waveParam->getValue();
        auto mappedVal = jmap(waveVal, 0.f, (float)waveVector.getNumberOfWaveTables() - 1);
        // i chose to calc this here as opposed to just doing it in the vector because I couldn't smooth the waveIndices (not sure if this is smart)_
        // They are potentially changing at the sample level so I thought it best to pass the smoothed wavePos value only
        int lowerWaveIndex = (int)mappedVal;
        int upperWaveIndex = lowerWaveIndex + 1;

        if (upperWaveIndex > waveVector.getNumberOfWaveTables())
        {
            upperWaveIndex = 0;
        }
        
        float interp = mappedVal - (float)lowerWaveIndex;

        auto waveLow = waveVector.getLowerWave(lowerWaveIndex)->getBuffer();
        auto waveHigh = waveVector.getUpperWave(upperWaveIndex)->getBuffer();

        auto buffRead0 = waveLow.getReadPointer(0);
        auto buffRead1 = waveHigh.getReadPointer(0);

        float waveIncrement = (float)w / waveVector.getTableSize();

        for (int i = 0; i <= waveVector.getTableSize() - 1; ++i)
        {
            auto x = i * waveIncrement;
            auto value0 = buffRead0[i] * (1.f - interp);
            auto value1 = buffRead1[i] * interp;
            auto interpWave = value0 + value1;
            auto y = frameHalf - (interpWave * frameHalf * 0.9f); // 0.9 meant to keep the wave from ever touching edge of frame
            wavePath.lineTo(x, y);
        }

        wavePath.lineTo(w, frameHalf);

        auto color = Colour{ 217, 205, 151 };
        g.setColour(color.withMultipliedLightness(0.6f + amp));
        auto strokeThickness = 1.f + (10.f * amp);
        PathStrokeType stroke(strokeThickness, juce::PathStrokeType::curved);
        g.strokePath(wavePath, stroke);
    }
    

}

void WavetableVisualizer::resized()
{
    
}

void WavetableVisualizer::setValue()
{
    //valueLabel->setText(String(pulsaretTable.getWaveIndex()), dontSendNotification);
}

bool WavetableVisualizer::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void WavetableVisualizer::filesDropped(const StringArray& files, int x, int y)
{
    processor.shouldProcess(false);

    auto file = File(files[0]);
    waveParser->loadFileToOsc(file, oscNum);
    
   // processor.loadWaveTables(files, oscNum);
    
    processor.shouldProcess(true);
}
