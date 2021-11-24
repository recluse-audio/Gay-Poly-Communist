/*
  ==============================================================================

    ParamSliders.h
    Created: 23 Sep 2021 2:32:56pm
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ArtieFeel.h"
//==============================================================================
/*
*/
class ParamSliders  : public juce::Component
{
public:
    ParamSliders(GayPolyCommunistAudioProcessor& audioProcessor, StringArray& params)
    {
        setLookAndFeel(&artieFeel);

        for (int i = 0; i < params.size(); i++)
        {
            labelArray.add(new Label());
            labelArray[i]->setText(params[i], NotificationType::dontSendNotification);
            addAndMakeVisible(labelArray[i]);

            sliderArray.add(new Slider());
            sliderArray[i]->setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
            sliderArray[i]->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 40, 20);
            addAndMakeVisible(sliderArray[i]);

            attachments.add(new AudioProcessorValueTreeState::SliderAttachment(
                audioProcessor.getValueTree(), params[i], *sliderArray[i]));
        }

    }

    ~ParamSliders() override
    {
    }

    void paint (juce::Graphics& g) override
    {
       // g.setColour(color);
       // g.fillAll();

    }

    void resized() override
    {
        auto width = 1.f / (float)sliderArray.size();
        for (int i = 0; i < sliderArray.size(); i++)
        {
            sliderArray[i]->setBoundsRelative(i * width, 0.f, width * 0.9f, 0.9f);
            labelArray[i]->setBoundsRelative(i * width + width/4.f, 0.f, width * 0.9f, 0.9f);
        }
    }

    void setColor(Colour color)
    {
        for (int i = 0; i < sliderArray.size(); i++)
        {
            sliderArray[i]->setColour(Slider::backgroundColourId, color);
            sliderArray[i]->repaint();
        }
    }


private:
    ArtieFeel artieFeel;
    
    OwnedArray<Slider> sliderArray;
    OwnedArray<Label> labelArray;
    OwnedArray<AudioProcessorValueTreeState::SliderAttachment> attachments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamSliders)
};
