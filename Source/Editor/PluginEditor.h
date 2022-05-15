/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Processor/PluginProcessor.h"
#include "../LookAndFeel/ArtieFeel.h"
#include "../LookAndFeel/NameFeel.h"

#include "../Components/WavetableVisualizer.h"
#include "../Components/LFOSlider.h"
#include "../Components/EnvSliders.h"
#include"../Components/GPCSlider.h"
#include "../Components/WaveMenu.h"
#include "../Components/FilterComponent.h"

#include"../Components/OscSlider.h"
#include "../Components/OscillatorComponent.h"
#include "../Components/EnvelopeComponent/EnvelopeVisualizer.h"

#include "../Components/MainMenu/MainMenuButton.h"



//==============================================================================
/**
*/
class GayPolyCommunistAudioProcessorEditor  : public juce::AudioProcessorEditor,
   Timer, Button::Listener
{
public:
    GayPolyCommunistAudioProcessorEditor (GayPolyCommunistAudioProcessor&);
    ~GayPolyCommunistAudioProcessorEditor() override;

    void initSliders();

    void initButtons();


    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

    void buttonClicked(Button*) override;
    void buttonStateChanged(Button*) override;

    
private:
    ArtieFeel artieFeel;
    NameFeel  nameFeel;
    Colour  color1, color2;
    float lineHeight = 0.5f;

    std::unique_ptr<LFOSlider> lfoSlider1;
    std::unique_ptr<LFOSlider> lfoSlider2;
    std::unique_ptr<LFOSlider> lfoSlider3;

    std::unique_ptr<EnvSliders> envSlider1;
    std::unique_ptr<EnvSliders> envSlider2;
    std::unique_ptr<EnvSliders> envSlider3;

    std::unique_ptr<OscillatorComponent> oscillator1;
    std::unique_ptr<OscillatorComponent> oscillator2;

    std::unique_ptr<FilterComponent> filterComponent;

    std::unique_ptr<TextButton> noteOn;
    std::unique_ptr<TextButton> noteOff;

    std::unique_ptr<MainMenuButton> menuButton;
    //============================================================================

    std::unique_ptr<Label> nameLabel;

    GayPolyCommunistAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GayPolyCommunistAudioProcessorEditor)
};
