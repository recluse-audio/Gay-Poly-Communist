/*
  ==============================================================================

    ModSliders.h
    Created: 23 Sep 2021 2:29:42pm
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
class ModSliders  : public juce::Component
{
public:
    /*
        Pass params here so I can use this class variadically?
        e.g. Creating a set for both env and lfo
    */
    ModSliders(GayPolyCommunistAudioProcessor& audioProcessor, StringArray& params)
    {
        setLookAndFeel(&artieFeel);

        for (int i = 0; i < params.size(); i++)
        {
            sliderArray.add(new Slider());
            sliderArray[i]->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
            sliderArray[i]->setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 20);
            addAndMakeVisible(sliderArray[i]);
            

            attachments.add(new AudioProcessorValueTreeState::SliderAttachment(
                audioProcessor.getValueTree(), params[i], *sliderArray[i]));
        }

    }

    ~ModSliders() override
    {

    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colours::transparentBlack.withAlpha(0.f));
    }

    void resized() override
    {
        auto width = 1.f / (float)sliderArray.size();
        for (int i = 0; i < sliderArray.size(); i++)
        {
            sliderArray[i]->setBoundsRelative(i * width, 0.f, width, 1.f);
            // skip one if this is an amp env (this env will always modulate at 100%)
            if (sliderArray.size() == 3)
            {
                int index = i + 1;
                width = 1.f / (float)(sliderArray.size() + 1);
                sliderArray[i]->setBoundsRelative(index * width, 0.f, width, 1.f);

            } 
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
    OwnedArray<AudioProcessorValueTreeState::SliderAttachment> attachments;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModSliders)
};
