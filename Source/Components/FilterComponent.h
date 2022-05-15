/*
  ==============================================================================

    FilterComponent.h
    Created: 4 Jan 2022 6:41:36am
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Processor/PluginProcessor.h"
#include "../Components/OscSlider.h"

//==============================================================================
/*
*/
class FilterComponent  : public juce::Component, Button::Listener
{
public:
    FilterComponent(GayPolyCommunistAudioProcessor& p, StringRef fMode,
        StringArray filtParams, StringArray resParams, StringArray driveParams) 
        : audioProcessor(p), filterMode(fMode)
    {
        sliderColor = Colours::aquamarine;

        filtSlider = std::make_unique<OscSlider>(filtParams, audioProcessor, true);
        filtSlider->setSliderColor(Colours::aquamarine);
        addAndMakeVisible(filtSlider.get());

        resonSlider = std::make_unique<OscSlider>(resParams, audioProcessor, true);
        resonSlider->setSliderColor(Colours::aquamarine);
        addAndMakeVisible(resonSlider.get());

        driveSlider = std::make_unique<OscSlider>(driveParams, audioProcessor, true);
        driveSlider->setSliderColor(Colours::aquamarine);
        addAndMakeVisible(driveSlider.get());

        hpfButton = std::make_unique<TextButton>("HPF");
        hpfButton->setColour(TextButton::buttonColourId, sliderColor.darker().withAlpha(0.4f));
        hpfButton->setClickingTogglesState(true);
        hpfButton->setRadioGroupId(1);
        hpfButton->addListener(this);
        addAndMakeVisible(hpfButton.get());

        lpfButton = std::make_unique<TextButton>("LPF");
        lpfButton->setColour(TextButton::buttonColourId, sliderColor.darker().withAlpha(0.4f));
        lpfButton->setClickingTogglesState(true);
        lpfButton->setRadioGroupId(1);
        lpfButton->addListener(this);
        addAndMakeVisible(lpfButton.get());

        bypassButton = std::make_unique<TextButton>("BYPASS");
        bypassButton->setColour(TextButton::buttonColourId, sliderColor.darker().withAlpha(0.4f));
        bypassButton->setClickingTogglesState(true);
        bypassButton->setRadioGroupId(1);
        bypassButton->addListener(this);
        addAndMakeVisible(bypassButton.get());
    }

    ~FilterComponent() override
    {    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(sliderColor);
        Rectangle<float> frame(0, 0, getWidth(), getHeight());
        g.drawRoundedRectangle(frame, 2.f, 1.f);
        g.setColour(sliderColor.darker(0.8f).withAlpha(0.5f));
        g.fillRoundedRectangle(frame, 2.f);
    }

    void resized() override
    {
        filtSlider->setBoundsRelative(0.05f, 0.1f, 0.9f, 0.29f);
        resonSlider->setBoundsRelative(0.05f, 0.4f, 0.9f, 0.29f);
        driveSlider->setBoundsRelative(0.05f, 0.7f, 0.9f, 0.29f);

        hpfButton->setBoundsRelative(0.05f, 0.02f, 0.3f, 0.05f);
        lpfButton->setBoundsRelative(0.35f, 0.02f, 0.3f, 0.05f);
        bypassButton->setBoundsRelative(0.65f, 0.02f, 0.3f, 0.05f);
    }

    void buttonClicked(Button* b) override
    {

    }

    void buttonStateChanged(Button* b) override
    {
        if (hpfButton->getToggleState())
        {
           
            RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(filterMode);

            pParam->beginChangeGesture();
            pParam->setValueNotifyingHost(0);
            pParam->endChangeGesture();
        }

        if (lpfButton->getToggleState())
        {
            RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(filterMode);

            pParam->beginChangeGesture();
            pParam->setValueNotifyingHost(1.f);
            pParam->endChangeGesture();
        }

        if (bypassButton->getToggleState())
        {
            RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(filterMode);

            pParam->beginChangeGesture();
            pParam->setValueNotifyingHost(2);
            pParam->endChangeGesture();
        }
    }

private:
    std::unique_ptr<OscSlider> filtSlider;
    std::unique_ptr<OscSlider> resonSlider;
    std::unique_ptr<OscSlider> driveSlider;

    std::unique_ptr<TextButton> hpfButton;
    std::unique_ptr<TextButton> lpfButton;
    std::unique_ptr<TextButton> bypassButton;

    StringRef filterMode; // used to attach to apvts param for filter mode

    GayPolyCommunistAudioProcessor& audioProcessor;
    Colour sliderColor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterComponent)
};
