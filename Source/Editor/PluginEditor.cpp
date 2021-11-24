/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "../Processor/PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GayPolyCommunistAudioProcessorEditor::GayPolyCommunistAudioProcessorEditor (GayPolyCommunistAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    setLookAndFeel(&artieFeel);
    color1 = Colours::yellow;
    color2 = Colours::blue;

    modLabel = std::make_unique<Label>();
    modLabel->setText("|     Amp       ||     Filter     ||      Pitch      ||      Wave    |", NotificationType::dontSendNotification);
    addAndMakeVisible(modLabel.get());

    wavetableVisualizer = std::make_unique<WavetableVisualizer>(audioProcessor);
    addAndMakeVisible(wavetableVisualizer.get());

    envVisual = std::make_unique<EnvelopeVisualizer>(audioProcessor);
    addAndMakeVisible(envVisual.get());

    //======================
    auto modParams1 = StringArray({ "Amp LFO 1", "Filt LFO 1", "Pitch LFO 1", "Wave LFO 1" });
    modSliders1 = std::make_unique<ModSliders>(audioProcessor, modParams1);
    modSliders1->setColor(color1);
    addAndMakeVisible(modSliders1.get());

    auto modParams2 = StringArray({ "Amp LFO 2", "Filt LFO 2", "Pitch LFO 2", "Wave LFO 2" });
    modSliders2 = std::make_unique<ModSliders>(audioProcessor, modParams2);
    modSliders2->setColor(color2);
    addChildComponent(modSliders2.get());

    //======================
    auto envParams1 = StringArray({ "Filt Env 1", "Pitch Env 1", "Wave Env 1" });
    envSliders1 = std::make_unique<ModSliders>(audioProcessor, envParams1);
    envSliders1->setColor(color1);
    addAndMakeVisible(envSliders1.get());

    auto envParams2 = StringArray({ "Amp Env 2", "Filt Env 2", "Pitch Env 2", "Wave Env 2" });
    envSliders2 = std::make_unique<ModSliders>(audioProcessor, envParams2);
    envSliders2->setColor(color2);
    addChildComponent(envSliders2.get());

    //======================
    auto adsrParams1 = StringArray({ "ATTACK 1", "DECAY 1", "SUSTAIN 1", "RELEASE 1" });
    adsrSliders1 = std::make_unique<ParamSliders>(audioProcessor, adsrParams1);
    adsrSliders1->setColor(color1);
    addAndMakeVisible(adsrSliders1.get());

    auto adsrParams2 = StringArray({ "ATTACK 2", "DECAY 2", "SUSTAIN 2", "RELEASE 2" });
    adsrSliders2 = std::make_unique<ParamSliders>(audioProcessor, adsrParams2);
    adsrSliders2->setColor(color2);
    addChildComponent(adsrSliders2.get());

    //======================
    auto lfoParams1 = StringArray({ "LFO 1 Rate", "LFO 1 Depth" });
    lfoSliders1 = std::make_unique<ParamSliders>(audioProcessor, lfoParams1);
    lfoSliders1->setColor(color1);
    addAndMakeVisible(lfoSliders1.get());

    auto lfoParams2 = StringArray({ "LFO 2 Rate", "LFO 2 Depth" });
    lfoSliders2 = std::make_unique<ParamSliders>(audioProcessor, lfoParams2);
    lfoSliders2->setColor(color2);
    addChildComponent(lfoSliders2.get());

    //======================
    auto filtParams = StringArray({ "Filter Freq", "Filter Res" });
    filtSliders = std::make_unique<ParamSliders>(audioProcessor, filtParams);
    filtSliders->setColor(Colours::red);
    addAndMakeVisible(filtSliders.get());



    initSliders();
    initButtons();

    setSize(800, 600);


    startTimerHz(60);
}

GayPolyCommunistAudioProcessorEditor::~GayPolyCommunistAudioProcessorEditor()
{
}

void GayPolyCommunistAudioProcessorEditor::initSliders()
{
    waveSlider = std::make_unique<Slider>();
    waveSlider->setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    waveSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    addAndMakeVisible(waveSlider.get());

    waveAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getValueTree(), "Wavetable", *waveSlider.get());
}

void GayPolyCommunistAudioProcessorEditor::initButtons()
{
    //======================
    modButtonLFO1 = std::make_unique <TextButton>("LFO 1");
    modButtonLFO1->addListener(this);
    modButtonLFO1->setClickingTogglesState(true);
    modButtonLFO1->setRadioGroupId(1);
    modButtonLFO1->setColour(TextButton::buttonColourId, color1);
    addAndMakeVisible(modButtonLFO1.get());

    modButtonLFO2 = std::make_unique <TextButton>("LFO 2");
    modButtonLFO2->addListener(this);
    modButtonLFO2->setClickingTogglesState(true);
    modButtonLFO2->setRadioGroupId(1);
    modButtonLFO2->setColour(TextButton::buttonColourId, color2);
    addAndMakeVisible(modButtonLFO2.get());
    
    //======================
    modButtonEnv1 = std::make_unique <TextButton>("Amp Env");
    modButtonEnv1->addListener(this);
    modButtonEnv1->setClickingTogglesState(true);
    modButtonEnv1->setRadioGroupId(2);
    modButtonEnv1->setColour(TextButton::buttonColourId, color1);
    addAndMakeVisible(modButtonEnv1.get());

    modButtonEnv2 = std::make_unique <TextButton>("Env 1");
    modButtonEnv2->addListener(this);
    modButtonEnv2->setClickingTogglesState(true);
    modButtonEnv2->setRadioGroupId(2);
    modButtonEnv2->setColour(TextButton::buttonColourId, color2);
    addAndMakeVisible(modButtonEnv2.get());

    //======================
    buttonADSR1 = std::make_unique <TextButton>("ADSR 1");
    buttonADSR1->addListener(this);
    buttonADSR1->setClickingTogglesState(true);
    buttonADSR1->setRadioGroupId(3);
    buttonADSR1->setColour(TextButton::buttonColourId, color1);
    addAndMakeVisible(buttonADSR1.get());

    buttonADSR2 = std::make_unique <TextButton>("ADSR 2");
    buttonADSR2->addListener(this);
    buttonADSR2->setClickingTogglesState(true);
    buttonADSR2->setRadioGroupId(3);
    buttonADSR2->setColour(TextButton::buttonColourId, color2);
    addAndMakeVisible(buttonADSR2.get());

    //======================
    buttonLFO1 = std::make_unique <TextButton>("LFO 1");
    buttonLFO1->addListener(this);
    buttonLFO1->setClickingTogglesState(true);
    buttonLFO1->setRadioGroupId(4);
    buttonLFO1->setColour(TextButton::buttonColourId, color1);
    addAndMakeVisible(buttonLFO1.get());

    buttonLFO2 = std::make_unique <TextButton>("LFO 2");
    buttonLFO2->addListener(this);
    buttonLFO2->setClickingTogglesState(true);
    buttonLFO2->setRadioGroupId(4);
    buttonLFO2->setColour(TextButton::buttonColourId, color2);
    addAndMakeVisible(buttonLFO2.get());

}

//==============================================================================
void GayPolyCommunistAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour({ 106, 135, 127 }).brighter());
}

void GayPolyCommunistAudioProcessorEditor::resized()
{
    adsrSliders1->setBoundsRelative(0.1f, 0.75f, 0.8f, 0.2f);
    adsrSliders2->setBoundsRelative(0.1f, 0.75f, 0.8f, 0.2f);

    lfoSliders1->setBoundsRelative(0.1f, 0.55f, 0.4f, 0.2f);
    lfoSliders2->setBoundsRelative(0.1f, 0.55f, 0.4f, 0.2f);

    filtSliders->setBoundsRelative(0.5f, 0.55f, 0.4f, 0.2f);

    waveSlider->setBoundsRelative(0.55f, 0.01f, 0.04f, 0.33f);
    wavetableVisualizer->setBoundsRelative(0.6f, 0.f, 0.4f, 0.35f);

    //==============================
    modSliders1->setBoundsRelative(0.1f, 0.1f, 0.4f, 0.1f);
    modSliders2->setBoundsRelative(0.1f, 0.1f, 0.4f, 0.1f);

    envSliders1->setBoundsRelative(0.1f, 0.2f, 0.4f, 0.1f); // env in the mod matrix
    envSliders2->setBoundsRelative(0.1f, 0.2f, 0.4f, 0.1f);

    //==============================
    modButtonLFO1->setBoundsRelative(0.01f, 0.12f, 0.07f, 0.03f);
    modButtonLFO2->setBoundsRelative(0.01f, 0.16f, 0.07f, 0.03f);

    modButtonEnv1->setBoundsRelative(0.01f, 0.2f, 0.07f, 0.03f);
    modButtonEnv2->setBoundsRelative(0.01f, 0.25f, 0.07f, 0.03f);

    buttonLFO1->setBoundsRelative(0.01f, 0.575f, 0.07f, 0.03f);
    buttonLFO2->setBoundsRelative(0.01f, 0.625f, 0.07f, 0.03f);

    buttonADSR1->setBoundsRelative(0.01f, 0.775f, 0.07f, 0.03f);
    buttonADSR2->setBoundsRelative(0.01f, 0.825f, 0.07f, 0.03f);

    modLabel->setBoundsRelative(0.1f, 0.05f, 0.4f, 0.05f);

    envVisual->setBoundsRelative(0.1f, 0.35f, 0.4f, 0.33f);

}

void GayPolyCommunistAudioProcessorEditor::timerCallback()
{
    wavetableVisualizer->setAmp(audioProcessor.getRMS());
    repaint();
}

void GayPolyCommunistAudioProcessorEditor::buttonClicked(Button* b)
{
    //// which lfo is being represented in mod matrix
    //if (b == modButtonLFO1.get())
    //{
    //    modSliders1->setVisible(true);
    //    modSliders2->setVisible(false);
    //}

    //if (b == modButtonLFO2.get())
    //{
    //    modSliders1->setVisible(false);
    //    modSliders2->setVisible(true);
    //}

    ////==============================
    //if (b == modButtonEnv1.get())
    //{
    //    envSliders1->setVisible(true);
    //    envSliders2->setVisible(false);
    //}
    //if (b == modButtonEnv2.get())
    //{
    //    envSliders1->setVisible(false);
    //    envSliders2->setVisible(true);
    //}

    ////==============================
    //if (b == buttonADSR1.get())
    //{
    //    adsrSliders1->setVisible(true);
    //    adsrSliders2->setVisible(false);
    //}
    //if (b == buttonADSR2.get())
    //{
    //    adsrSliders1->setVisible(false);
    //    adsrSliders2->setVisible(true);
    //}

    ////==============================
    //if (b == buttonLFO1.get())
    //{
    //    lfoSliders1->setVisible(true);
    //    lfoSliders2->setVisible(false);
    //}
    //if (b == buttonLFO2.get())
    //{
    //    lfoSliders1->setVisible(false);
    //    lfoSliders2->setVisible(true);
    //}
}
