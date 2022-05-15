/*
  ==============================================================================

    EnvSliders.h
    Created: 23 Sep 2021 2:32:56pm
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Processor/PluginProcessor.h"
#include "../LookAndFeel/ArtieFeel.h"
//==============================================================================
/*
*/
class EnvSliders  : public juce::Component, public Button::Listener
{
public:
    EnvSliders(GayPolyCommunistAudioProcessor& p, StringArray& params, float eNum) : audioProcessor(p), envNum(eNum)
    {
        setLookAndFeel(&artieFeel);

        adsrStrings = StringArray({ "A", "D", "S", "R" });

        for (int i = 0; i < params.size(); i++)
        {
            labelArray.add(new Label());
            labelArray[i]->setText(adsrStrings[i], NotificationType::dontSendNotification);
            addAndMakeVisible(labelArray[i]);

            sliderArray.add(new Slider());
            sliderArray[i]->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
            sliderArray[i]->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 40, 20);
            addAndMakeVisible(sliderArray[i]);

            attachments.add(new AudioProcessorValueTreeState::SliderAttachment(
                audioProcessor.getValueTree(), params[i], *sliderArray[i]));

            sliderArray[i]->setNumDecimalPlacesToDisplay(1);
            sliderArray[i]->textFromValueFunction = [](double value)
            {
                String text(value, 2);
                return text;
            };
        }

        button = std::make_unique<TextButton>("Map");
        button->addListener(this);
        addAndMakeVisible(button.get());

        if ((int)envNum == 1)
        {
            ampLabel = std::make_unique<Label>();
            ampLabel->setText("Amp", NotificationType::dontSendNotification);
            addAndMakeVisible(ampLabel.get());
        }
    }

    ~EnvSliders() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        button->setColour(TextButton::buttonColourId, color);
        // boundary rect for gui design (remove for final product)
        auto frame = Rectangle<float>(getLocalBounds().toFloat());
        g.setColour(color.brighter(0.8f).withRotatedHue(0.1f));
        g.drawRect(frame, 2.f);

        g.setColour(color.withAlpha(0.2f));
        g.fillAll();
    }

    void resized() override
    {
        float y = 0.3f;

        sliderArray[0]->setBoundsRelative(0.11f, y, 0.15f, 0.65f);
        labelArray[0]->setBoundsRelative(0.11f, 0.4f, 0.15f, 0.2f);

        sliderArray[1]->setBoundsRelative(0.32f, y, 0.15f, 0.65f); // .275
        labelArray[1]->setBoundsRelative(0.32f, 0.4f, 0.15f, 0.2f);

        sliderArray[2]->setBoundsRelative(0.53f, y, 0.15f, 0.65f); // 
        labelArray[2]->setBoundsRelative(0.53f, 0.4f, 0.15f, 0.2f);

        sliderArray[3]->setBoundsRelative(0.74f, y, 0.15f, 0.65f);
        labelArray[3]->setBoundsRelative(0.74f, 0.4f, 0.15f, 0.2f);

        button->setBoundsRelative(0.35f, 0.06f, 0.3f, 0.15f);

        if (envNum == 1.f)
            ampLabel->setBoundsRelative(0.02f, 0.06f, 0.3f, 0.2f);
    }

    void setColor(Colour c)
    {
        color = c;
        
        button->setColour(TextButton::buttonColourId, color);
        for (int i = 0; i < sliderArray.size(); i++)
        {
            sliderArray[i]->setColour(Slider::backgroundColourId, c.withRotatedHue(0.1f));
            sliderArray[i]->repaint();
            labelArray[i]->setColour(Label::backgroundColourId, color.brighter(0.99f));
        }
        if (envNum == 1.f)
        {
            ampLabel->setColour(Label::backgroundColourId, color.brighter(0.99f).withRotatedHue(0.1f));

        }
    }

    void buttonClicked(Button* b) override
    {
        if (b == button.get())
        {
            audioProcessor.setMappingEnv(true, envNum);
        }
    }


private:
    ArtieFeel artieFeel;
    StringArray adsrStrings;

    OwnedArray<Slider> sliderArray;
    OwnedArray<Label> labelArray;
    OwnedArray<AudioProcessorValueTreeState::SliderAttachment> attachments;
    std::unique_ptr<Label> ampLabel; // only for env1

    std::unique_ptr<TextButton> button;

    float envNum = 0.f;  
    Colour color{ 255, 255, 255 };

    GayPolyCommunistAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvSliders)
};
