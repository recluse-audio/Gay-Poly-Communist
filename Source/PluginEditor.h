/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ArtieFeel.h"
#include "WavetableVisualizer.h"
#include "ModSliders.h"
#include "ParamSliders.h"
#include "EnvelopeVisualizer.h"



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

private:
    ArtieFeel artieFeel;
    Colour  color1, color2;

    std::unique_ptr<Label> modLabel;
    std::unique_ptr<WavetableVisualizer> wavetableVisualizer;

    std::unique_ptr<ModSliders> modSliders1;
    std::unique_ptr<ModSliders> modSliders2;

    std::unique_ptr<ModSliders> envSliders1;
    std::unique_ptr<ModSliders> envSliders2;

    std::unique_ptr<ParamSliders> adsrSliders1;
    std::unique_ptr<ParamSliders> adsrSliders2;

    std::unique_ptr<ParamSliders> lfoSliders1;
    std::unique_ptr<ParamSliders> lfoSliders2;

    std::unique_ptr<ParamSliders> filtSliders;

    std::unique_ptr<Slider> filtSlider;
    std::unique_ptr<Slider> waveSlider;

    //==============================================================================

    std::unique_ptr<TextButton> modButtonLFO1; // which lfo is being controlled in matrix
    std::unique_ptr<TextButton> modButtonLFO2;

    std::unique_ptr<TextButton> modButtonEnv1; // which env in matrix
    std::unique_ptr<TextButton> modButtonEnv2;

    std::unique_ptr<TextButton> buttonADSR1; // which env slider
    std::unique_ptr<TextButton> buttonADSR2;

    std::unique_ptr<TextButton> buttonLFO1; // which LFO slider
    std::unique_ptr<TextButton> buttonLFO2;

    std::unique_ptr<EnvelopeVisualizer> envVisual;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> waveAttach;


    GayPolyCommunistAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GayPolyCommunistAudioProcessorEditor)
};
