/*
  ==============================================================================

    OscillatorComponent.h
    Created: 7 Dec 2021 6:54:38pm
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Processor/PluginProcessor.h"
#include "../Components/OscSlider.h"
#include "../Components/WavetableVisualizer.h"

//==============================================================================
/*
*/
class OscillatorComponent  : public juce::Component
{
public:
    OscillatorComponent(int oscNum, GayPolyCommunistAudioProcessor& p, StringArray gainParams, StringArray pitchParams, StringArray waveParams) 
        : audioProcessor(p)
    {
        wavetableVisualizer = std::make_unique<WavetableVisualizer>(oscNum, audioProcessor, audioProcessor.getWaveVector(oscNum));
        addAndMakeVisible(wavetableVisualizer.get());

        ampSlider = std::make_unique<OscSlider>(gainParams, audioProcessor, true);
        addAndMakeVisible(ampSlider.get());

        pitchSlider = std::make_unique<OscSlider>(pitchParams, audioProcessor, true);
        addAndMakeVisible(pitchSlider.get());

        waveSlider = std::make_unique<OscSlider>(waveParams, audioProcessor, false);
        addAndMakeVisible(waveSlider.get());

        waveMenu = std::make_unique<WaveMenu>(audioProcessor, oscNum);
        addAndMakeVisible(waveMenu.get());
    }

    ~OscillatorComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        wavetableVisualizer->setAmp(audioProcessor.getRMS());

        g.setColour(Colours::white);
        auto frame = Rectangle<float>(getLocalBounds().toFloat());
        g.drawRect(frame, 1.f);

        g.setColour(color.darker(0.9f).withAlpha(0.3f));
        g.fillAll();
    }

    void setColor(Colour c)
    {
        color = c;
        ampSlider->setSliderColor(color);
        waveSlider->setSliderColor(color);
        pitchSlider->setSliderColor(color);
        waveMenu->setColor(color);
    }

    void resized() override
    {

        wavetableVisualizer->setBoundsRelative(0.05f, 0.05f, 0.65f, 0.6f);
        waveSlider->setBoundsRelative(0.75f, 0.05f, 0.2f, 0.6f);
        ampSlider->setBoundsRelative(0.05f, 0.7f, 0.3f, 0.28f);
        pitchSlider->setBoundsRelative(0.4f, 0.7f, 0.3f, 0.28f);
        waveMenu->setBoundsRelative(0.75f, 0.7f, 0.2f, 0.1f);
        

    }

private:
    std::unique_ptr<OscSlider> ampSlider;
    std::unique_ptr<OscSlider> pitchSlider;
    std::unique_ptr<OscSlider> waveSlider;
    std::unique_ptr<WavetableVisualizer> wavetableVisualizer;
    std::unique_ptr<WaveMenu> waveMenu;

    Colour color{ 10, 10, 10 };

    GayPolyCommunistAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorComponent)
};
