/*
  ==============================================================================

    GPCSlider.h
    Created: 30 Nov 2021 3:58:22pm
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ArtieFeel.h"
//==============================================================================
/*
*   Default slider component for GPC, similar to the param slider but with specifications
*   for a linear bar ADSR(s) and knobs for LFO depth and LFO Rate
*/
class GPCSliders : public juce::Component
{
public:
    GPCSliders(GayPolyCommunistAudioProcessor& audioProcessor, StringArray& sliderParams, StringArray& lfoEnvParams, StringArray& knobParams)
    {
        adsrStrings = StringArray({ "Attack", "Decay", "Sustain", "Release" });
        knobStrings = StringArray({ "Rate", "Depth" });

        setLookAndFeel(&artieFeel);

        for (int i = 0; i < sliderParams.size(); i++)
        {
            envLabels.add(new Label());
            envLabels[i]->setText(adsrStrings[i], NotificationType::dontSendNotification);
            addAndMakeVisible(envLabels[i]);

            envSliders.add(new Slider());
            envSliders[i]->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
            envSliders[i]->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 40, 20);
            addAndMakeVisible(envSliders[i]);

            attachments.add(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getValueTree(), sliderParams[i], *envSliders[i]));
        }

        for (int j = 0; j < lfoEnvParams.size(); j++)
        {
            lfoEnvLabels.add(new Label());
            lfoEnvLabels[j]->setText(adsrStrings[j], NotificationType::dontSendNotification);
            addAndMakeVisible(lfoEnvLabels[j]);

            lfoEnvSliders.add(new Slider());
            lfoEnvSliders[j]->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
            lfoEnvSliders[j]->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 40, 20);
            addAndMakeVisible(lfoEnvSliders[j]);

            attachments.add(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getValueTree(), lfoEnvParams[j], *lfoEnvSliders[j]));
        }

        for (int k = 0; k < knobParams.size(); k++)
        {
            knobLabels.add(new Label());
            knobLabels[k]->setText(knobStrings[k], NotificationType::dontSendNotification);
            addAndMakeVisible(knobLabels[k]);

            knobArray.add(new Slider());
            knobArray[k]->setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
            knobArray[k]->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 40, 20);
            addAndMakeVisible(knobArray[k]);

            attachments.add(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getValueTree(), knobParams[k], *knobArray[k]));
        }
    }

    ~GPCSliders() override
    {
    }

    void paint(juce::Graphics& g) override
    {
        // g.setColour(color);
        // g.fillAll();

    }

    void resized() override
    {

        auto width = 0.85f / (float)envSliders.size(); // divide up 0.8 of full width by number of sliders in row, leaving space for knobs

        for (int i = 0; i < envSliders.size(); i++)
        {
            envSliders[i]->setBoundsRelative(i * width, 0.1f, width * 0.7f, 0.375f);
            envLabels[i]->setBoundsRelative(i * width , 0.1f, width * 0.7f, 0.375f);
        }

        for (int j = 0; j < lfoEnvSliders.size(); j++)
        {
            lfoEnvSliders[j]->setBoundsRelative(j * width, 0.5f, width * 0.7f, 0.375f);
            lfoEnvLabels[j]->setBoundsRelative(j * width, 0.5f, width * 0.7f, 0.375f);
        }

        for (int k = 0; k < knobArray.size(); k++)
        {
            float yPos = k * 0.4f;
            knobArray[k]->setBoundsRelative(0.85f, 0.1f + yPos, 0.125f, 0.4f);
            knobLabels[k]->setBoundsRelative(0.85f, 0.1f + yPos, 0.125f, 0.4f);
        }
    }

    void setColor(Colour color)
    {

    }


private:
    ArtieFeel artieFeel;

    OwnedArray<Slider> envSliders;
    OwnedArray<Slider> lfoEnvSliders;
    OwnedArray<Slider> knobArray;

    OwnedArray<Label> envLabels;
    OwnedArray<Label> lfoEnvLabels;
    OwnedArray<Label> knobLabels;

    StringArray adsrStrings;
    StringArray knobStrings;
    OwnedArray<AudioProcessorValueTreeState::SliderAttachment> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GPCSliders)
};
