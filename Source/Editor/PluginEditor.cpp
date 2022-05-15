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
    nameLabel = std::make_unique<Label>();
    nameLabel->setText("GAY POLY COMMUNIST", NotificationType::dontSendNotification);
    nameLabel->setLookAndFeel(&nameFeel);
    nameLabel->setAlwaysOnTop(true);
    addAndMakeVisible(nameLabel.get());

    setLookAndFeel(&artieFeel);
    color1 = Colours::yellow;
    color2 = Colours::blue;

    menuButton = std::make_unique<MainMenuButton>(audioProcessor);
    addAndMakeVisible(menuButton.get());
    // params from value tree passed to osc component and assigned to sliders there
    auto gainParams = StringArray({ "Gain 1", "Gain 1 LFO Source", "Gain 1 LFO Scale", "Gain 1 Env Source", "Gain 1 Env Scale", "Gain" });
    auto pitchParams = StringArray({ "Pitch 1", "Pitch 1 LFO Source", "Pitch 1 LFO Scale", "Pitch 1 Env Source", "Pitch 1 Env Scale", "Pitch" });
    auto waveParams = StringArray({ "Wave 1 Position", "Wave 1 LFO Source", "Wave 1 LFO Scale", "Wave 1 Env Source", "Wave 1 Env Scale", "Wave" });
    oscillator1 = std::make_unique<OscillatorComponent>(1, audioProcessor, gainParams, pitchParams, waveParams);
    addAndMakeVisible(oscillator1.get());
    oscillator1->setColor(Colours::darkorange);

    auto gainParams2 = StringArray({ "Gain 2", "Gain 2 LFO Source", "Gain 2 LFO Scale", "Gain 2 Env Source", "Gain 2 Env Scale", "Gain" });
    auto pitchParams2 = StringArray({ "Pitch 2", "Pitch 2 LFO Source", "Pitch 2 LFO Scale", "Pitch 2 Env Source", "Pitch 2 Env Scale", "Pitch" });
    auto waveParams2 = StringArray({ "Wave 2 Position", "Wave 2 LFO Source", "Wave 2 LFO Scale", "Wave 2 Env Source", "Wave 2 Env Scale", "Wave" });
    oscillator2 = std::make_unique<OscillatorComponent>(2, audioProcessor, gainParams2, pitchParams2, waveParams2);
    addAndMakeVisible(oscillator2.get());
    oscillator2->setColor(Colours::darkorange);

    auto lfoParams1 = StringArray({ "LFO Rate 1", "LFO Rate Env Source 1", "LFO Rate Env Scale 1", 
        "LFO Depth 1", "LFO Depth Env Source 1", "LFO Depth Env Scale 1" });
    lfoSlider1 = std::make_unique<LFOSlider>(lfoParams1, audioProcessor, 1.f);
    lfoSlider1->setColor(Colours::red);
    addAndMakeVisible(lfoSlider1.get());

    auto lfoParams2 = StringArray({ "LFO Rate 2", "LFO Rate Env Source 2", "LFO Rate Env Scale 2",
        "LFO Depth 2", "LFO Depth Env Source 2", "LFO Depth Env Scale 2" });
    lfoSlider2 = std::make_unique<LFOSlider>(lfoParams2, audioProcessor, 2.f);
    lfoSlider2->setColor(Colours::green);
    addAndMakeVisible(lfoSlider2.get());

    auto lfoParams3 = StringArray({ "LFO Rate 3", "LFO Rate Env Source 3", "LFO Rate Env Scale 3",
        "LFO Depth 3", "LFO Depth Env Source 3", "LFO Depth Env Scale 3" });
    lfoSlider3 = std::make_unique<LFOSlider>(lfoParams3, audioProcessor, 3.f);
    lfoSlider3->setColor(Colours::blue);
    addAndMakeVisible(lfoSlider3.get());

    auto envParams1 = StringArray({ "ATTACK 1", "DECAY 1", "SUSTAIN 1", "RELEASE 1" });
    envSlider1 = std::make_unique<EnvSliders>(audioProcessor, envParams1, 1.f);
    envSlider1->setColor(Colours::cyan); // notice the american spelling menaing my class
    addAndMakeVisible(envSlider1.get());

    auto envParams2 = StringArray({ "ATTACK 2", "DECAY 2", "SUSTAIN 2", "RELEASE 2" });
    envSlider2 = std::make_unique<EnvSliders>(audioProcessor, envParams2, 2.f);
    envSlider2->setColor(Colours::magenta);
    addAndMakeVisible(envSlider2.get());

    auto envParams3 = StringArray({ "ATTACK 3", "DECAY 3", "SUSTAIN 3", "RELEASE 3" });
    envSlider3 = std::make_unique<EnvSliders>(audioProcessor, envParams3, 3.f);
    envSlider3->setColor(Colours::yellow);
    addAndMakeVisible(envSlider3.get());


    auto filtParams = StringArray({ "Filter Freq", "Filter LFO Source", "Filter LFO Scale", "Filter Env Source", "Filter Env Scale", "Freq" });
    auto resParams = StringArray({ "Filter Res", "Res LFO Source", "Res LFO Scale", "Res Env Source", "Res Env Scale", "Reson" });
    auto driveParams = StringArray({ "Filter Drive", "Drive LFO Source", "Drive LFO Scale", "Drive Env Source", "Drive Env Scale", "Drive" });

    filterComponent = std::make_unique<FilterComponent>
        (audioProcessor, "Filter Mode", filtParams, resParams, driveParams);
    addAndMakeVisible(filterComponent.get());

    noteOn = std::make_unique<TextButton>("Note On");
    addAndMakeVisible(noteOn.get());
    noteOn->addListener(this);

    noteOff = std::make_unique<TextButton>("Note Off");
    addAndMakeVisible(noteOff.get());
    noteOff->addListener(this);
    //midiTestButton->setClickingTogglesState(true);


//    setSize(800, 500);
    setSize(1000, 650);

    startTimerHz(60);
}

GayPolyCommunistAudioProcessorEditor::~GayPolyCommunistAudioProcessorEditor()
{
}


//==============================================================================
void GayPolyCommunistAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour({ 20, 5, 7 }));

    auto frame = getLocalBounds().reduced(getWidth() * 0.025f, getHeight() * 0.06f);
    frame.setPosition(getWidth() * 0.025, getHeight() * 0.09f);
    g.setColour(Colours::antiquewhite);
    g.drawRoundedRectangle(frame.toFloat(), 2.f, 2.f);

    auto largeFrame = getLocalBounds();
    g.drawRoundedRectangle(largeFrame.toFloat(), 2.f, 2.f);

    auto midPath = Path();
    midPath.startNewSubPath(getWidth() * 0.05f, lineHeight);
    midPath.lineTo(getWidth() * 0.95f, lineHeight);
    midPath.closeSubPath();

    auto stroke = PathStrokeType(2.f);
    g.strokePath(midPath, stroke);

}

void GayPolyCommunistAudioProcessorEditor::resized()
{
    menuButton->setBoundsRelative(0.85f, 0.01f, 0.075f, 0.07f);

    lineHeight = getHeight() * 0.6f;
    nameLabel->setBoundsRelative(0.05f, 0.f, 0.4f, 0.1f);

    oscillator1->setBoundsRelative(0.05f, 0.115, 0.385f, 0.46f);
    oscillator2->setBoundsRelative(0.46f, 0.115, 0.385f, 0.46f);

    noteOn->setBoundsRelative(0.6f, 0.05f, 0.1f, 0.03f);
    noteOff->setBoundsRelative(0.7f, 0.05f, 0.1f, 0.03f);

    float lfoY = 0.625f, envY = 0.75f, lfoH = 0.1f, envH = 0.2f, lfoW = 0.29f, envW = 0.29f;

    lfoSlider1->setBoundsRelative(0.05f, lfoY, lfoW, lfoH);
    lfoSlider2->setBoundsRelative(0.355f, lfoY, lfoW, lfoH);
    lfoSlider3->setBoundsRelative(0.66f, lfoY, lfoW, lfoH);

    envSlider1->setBoundsRelative(0.05f, envY, envW, envH);
    envSlider2->setBoundsRelative(0.355f, envY, envW, envH);
    envSlider3->setBoundsRelative(0.66f, envY, envW, envH);

    filterComponent->setBoundsRelative(0.86f, 0.115f, 0.1f, 0.46f);
    //filtSlider->setBoundsRelative(0.86f, 0.115f, 0.1f, 0.14f);
    //resonSlider->setBoundsRelative(0.86f, 0.265f, 0.1f, 0.14f);
    //driveSlider->setBoundsRelative(0.86f, 0.415f, 0.1f, 0.14f);

}

void GayPolyCommunistAudioProcessorEditor::timerCallback()
{
    repaint();
}

void GayPolyCommunistAudioProcessorEditor::buttonClicked(Button* b)
{
    if (b == noteOn.get())
    {
        audioProcessor.triggerMidi(true);
    }
    if (b == noteOff.get())
    {
        audioProcessor.triggerMidi(false);
    }
}

void GayPolyCommunistAudioProcessorEditor::buttonStateChanged(Button* b)
{

}



