/*
  ==============================================================================

    WavetableVisualizer.cpp
    Created: 6 Nov 2020 4:34:21pm
    Author:  Billie (Govnah) Jean

  ==============================================================================
*/

#include "WavetableVisualizer.h"


//==============================================================================
WavetableVisualizer::WavetableVisualizer(GayPolyCommunistAudioProcessor& p) 
    : processor(p), waveVector(p.getWaveVector())
{
    setSize(400, 150);
    setNewWaveColour(juce::Colours::white);
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

        auto buffRead0 = waveVector.getLowerWave()->getBuffer().getReadPointer(0);
        auto buffRead1 = waveVector.getUpperWave()->getBuffer().getReadPointer(0);

        float waveIncrement = (float)w / waveVector.getTableSize();

        for (int i = 0; i <= waveVector.getTableSize(); ++i)
        {
            auto x = i * waveIncrement;
            auto value0 = buffRead0[i] * (1.f - waveVector.getWaveInterp());
            auto value1 = buffRead1[i] * (waveVector.getWaveInterp());
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

    for (auto file : files)
    {
        for (int voiceNum = 0; voiceNum < processor.getSynth().getNumVoices(); voiceNum++)
        {
            if (auto voice = dynamic_cast<GayVoice*>(processor.getSynth().getVoice(voiceNum)))
            {
                voice->loadTables(file);
            }
        }
    }

    processor.shouldProcess(true);
}
