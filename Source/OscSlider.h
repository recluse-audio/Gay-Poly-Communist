/*
  ==============================================================================

    OscSlider.h
    Created: 6 Dec 2021 10:25:25am
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "NameFeel.h"

//==============================================================================
/*
*/

class OscSlider  : public juce::Component, Button::Listener
{
public:
    enum ParamStrings
    {
        paramName,
        lfoSource, 
        lfoScale,
        envSource,
        envScale,
        label
    };

    OscSlider(StringArray pLabels, GayPolyCommunistAudioProcessor& p, bool rotary) 
        : audioProcessor(p), isRotary(rotary)
    {
        paramArray = pLabels;

        if (isRotary)
        {
            slider = std::make_unique<Slider>(Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);

            envScaleVisual = std::make_unique<Slider>(Slider::SliderStyle::Rotary, Slider::TextEntryBoxPosition::NoTextBox);
            envScaleVisual->setLookAndFeel(&altFeel);
            addAndMakeVisible(envScaleVisual.get());

            lfoScaleVisual = std::make_unique<Slider>(Slider::SliderStyle::Rotary, Slider::TextEntryBoxPosition::NoTextBox);
            lfoScaleVisual->setLookAndFeel(&altFeel);
            addAndMakeVisible(lfoScaleVisual.get());
        }
        else
        {
            slider = std::make_unique<Slider>(Slider::SliderStyle::LinearBarVertical, Slider::TextEntryBoxPosition::NoTextBox);

            envScaleVisual = std::make_unique<Slider>(Slider::SliderStyle::ThreeValueVertical, Slider::TextEntryBoxPosition::NoTextBox);
            envScaleVisual->setLookAndFeel(&artieFeel);
            addAndMakeVisible(envScaleVisual.get());

            lfoScaleVisual = std::make_unique<Slider>(Slider::SliderStyle::ThreeValueVertical, Slider::TextEntryBoxPosition::NoTextBox);
            lfoScaleVisual->setLookAndFeel(&artieFeel);
            addAndMakeVisible(lfoScaleVisual.get());
        }

        addAndMakeVisible(slider.get());
        sliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.getValueTree(), paramArray[ParamStrings::paramName], *slider.get());

        slider->setNumDecimalPlacesToDisplay(2);


        sliderLabel = std::make_unique<Label>();
        sliderLabel->setText(paramArray[ParamStrings::label], NotificationType::dontSendNotification);
        sliderLabel->attachToComponent(slider.get(), false);
        addAndMakeVisible(sliderLabel.get());
        
        // LFO assignment and scale
        lfoButton = std::make_unique<TextButton>("LFO");
        addAndMakeVisible(lfoButton.get());
        lfoButton->addListener(this);

        lfoScaleSlider = std::make_unique<Slider>(Slider::SliderStyle::LinearBarVertical, Slider::TextEntryBoxPosition::NoTextBox);
        addAndMakeVisible(lfoScaleSlider.get());
        lfoScaleAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.getValueTree(), paramArray[ParamStrings::lfoScale], *lfoScaleSlider.get());

        // Env assignment and scale
        envButton = std::make_unique<TextButton>("Env");
        addAndMakeVisible(envButton.get());
        envButton->addListener(this);

        envScaleSlider = std::make_unique<Slider>(Slider::SliderStyle::LinearBarVertical, Slider::TextEntryBoxPosition::NoTextBox);
        addAndMakeVisible(envScaleSlider.get());
        envScaleAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.getValueTree(), paramArray[ParamStrings::envScale], *envScaleSlider.get());

        lfoColor = Colours::transparentBlack.withAlpha(0.f);
        envColor = Colours::transparentBlack.withAlpha(0.f);
    }
    
    ~OscSlider() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        checkModColors();
        resetSliderColors();

        lfoButton->setColour(TextButton::buttonColourId, lfoColor);
        envButton->setColour(TextButton::buttonColourId, envColor);

        drawModVisual();

        // boundary rect for gui design (remove for final product)
        auto frame = Rectangle<float>(getLocalBounds().toFloat());

        g.setColour(sliderColor.brighter(0.8f));
       // g.drawRoundedRectangle(getLocalBounds().toFloat(), 1.f, 1.f);

    }

    void resetSliderColors()
    {
        slider->setColour(Slider::backgroundColourId, sliderColor);
        lfoScaleSlider->setColour(Slider::backgroundColourId, sliderColor);
        envScaleSlider->setColour(Slider::backgroundColourId, sliderColor);
        sliderLabel->setColour(Label::backgroundColourId, sliderColor);
    }

    void setSliderColor(Colour c)
    {
        sliderColor = c;
    }
    // a visual representation of lfo depth that draws a rotary line fill with a start/end position 
    // determined by lfoDepth(global) and lfoScale(local to each param)
    void drawModVisual()
    {
        if (isRotary)
        {
            drawRotaryModVisual();
        }
        else
        {
            drawVerticalModVisual();
        }

    }

    void drawRotaryModVisual()
    {
        // getting radians and value of the slider
        auto rotaryParams = slider->getRotaryParameters();
        auto val = (float)slider->getValue();

        // This is to account for params like "filter freq" whose slider returns a non-normalized value (surprisingly)
        auto range = NormalisableRange<float>(slider->getRange().getStart(), slider->getRange().getEnd(), 0.1f, slider->getSkewFactor());
        float normalizedVal = range.convertTo0to1(val);
        auto sliderValRadians = jmap(normalizedVal, rotaryParams.startAngleRadians, rotaryParams.endAngleRadians);

        // drawing 
        auto radianRange = rotaryParams.endAngleRadians - rotaryParams.startAngleRadians;
        auto lfoRadians = radianRange * lfoScaleSlider->getValue() * getLFODepth();

        // limiting the rotary because the 'stopAtEnd bool is not working how I think it would
        auto rotaryStart = jlimit<float>(rotaryParams.startAngleRadians, rotaryParams.endAngleRadians, sliderValRadians - lfoRadians);
        auto rotaryEnd = jlimit<float>(rotaryParams.startAngleRadians, rotaryParams.endAngleRadians, sliderValRadians + lfoRadians);

        lfoScaleVisual->setRotaryParameters(rotaryStart, rotaryEnd, true);
        lfoScaleVisual->setColour(Slider::rotarySliderFillColourId, lfoColor);
        envScaleVisual->setColour(Slider::rotarySliderFillColourId, envColor);


        auto envScaleVal = getEnvScaleVal();
        auto envRadians = radianRange * envScaleVal * getEnvSustain();
        auto envRotaryEnd = jlimit<float>(rotaryParams.startAngleRadians, rotaryParams.endAngleRadians, sliderValRadians + envRadians);

        if (envRotaryEnd > sliderValRadians)
        {
            envScaleVisual->setRotaryParameters(sliderValRadians, envRotaryEnd, true);
        }
        else // if the env is inverted and going down
        {
            envScaleVisual->setRotaryParameters(envRotaryEnd, sliderValRadians, true);
        }
    }

    void drawVerticalModVisual()
    {
        lfoScaleVisual->setColour(Slider::backgroundColourId, lfoColor);
        envScaleVisual->setColour(Slider::backgroundColourId, envColor);

        auto val = slider->getValue();

        auto range = Range<double>(slider->getRange().getStart(), slider->getRange().getEnd());

        auto lfoVal = getLFODepth() * lfoScaleSlider->getValue();
        auto envVal = val + (getEnvSustain() * getEnvScaleVal());

        lfoScaleVisual->setRange(range, 0.01);
        envScaleVisual->setRange(range, 0.01);


        lfoScaleVisual->setSkewFactor(1);
        lfoScaleVisual->setValue(val);
        envScaleVisual->setValue(val);

        lfoScaleVisual->setMinAndMaxValues(val - lfoVal, val + lfoVal);

        if (envVal >= val)
        {
            envScaleVisual->setMinAndMaxValues(val, envVal);

        }
        else
        {
            envScaleVisual->setMinAndMaxValues(envVal, val);
        }

    }
    // gets the depth of an LFO based on which lfo is assigned
    float getLFODepth()
    {
        switch ((int)lfoNum)
        {
            case 0:
            {
                return 0.f;
            }
            break;
            case 1:
            {
                return audioProcessor.getValueTree().getParameter("LFO Depth 1")->getValue();
            }
            break;
            case 2:
            {
                return audioProcessor.getValueTree().getParameter("LFO Depth 2")->getValue();
            }
            break;
            case 3:
            {
                return audioProcessor.getValueTree().getParameter("LFO Depth 3")->getValue();
            }
            break;
        }
    }

    float getEnvSustain()
    {
        switch ((int)envNum)
        {
            case 0:
            {
                return 0.f;
            }
            break;
            case 1:
            {
                return audioProcessor.getValueTree().getParameter("SUSTAIN 1")->getValue();
            }
            break;
            case 2:
            {
                return audioProcessor.getValueTree().getParameter("SUSTAIN 2")->getValue();
            }
            break;
            case 3:
            {
                return audioProcessor.getValueTree().getParameter("SUSTAIN 3")->getValue();
            }
            break;
        }
    }

    // returns 0 - 1 if envVal is above 0.5, returns negative values if below 0.5
    float getEnvScaleVal()
    {
        float envVal = (envScaleSlider->getValue() - 0.5f) * 2.f;
        return envVal;
    }

    void checkModColors()
    {
        lfoNum = audioProcessor.getValueTree().getRawParameterValue(paramArray[ParamStrings::lfoSource])->load();
        envNum = audioProcessor.getValueTree().getRawParameterValue(paramArray[ParamStrings::envSource])->load();

        switch ((int)lfoNum)
        {
        case 0: lfoColor = sliderColor.darker(); break;
        case 1: lfoColor = Colours::red; break;
        case 2: lfoColor = Colours::green; break;
        case 3: lfoColor = Colours::blue; break;
        }
        switch((int)envNum)
        {
        case 0: envColor = sliderColor.darker(); break;
        case 1: envColor = Colours::cyan; break;
        case 2: envColor = Colours::magenta; break;
        case 3: envColor = Colours::yellow; break;
        }
        if (audioProcessor.isMappingLFO())
        {
            lfoColor = Colours::white;
        }
        if (audioProcessor.isMappingEnv())
        {
            envColor = Colours::white;
        }

    }

    void resized() override
    {
        slider->setBoundsRelative(0.1f, 0.02f, 0.8f, 0.775f);
        sliderLabel->setBoundsRelative(0.2f, 0.6f, 0.6f, 0.2f);

        lfoButton->setBoundsRelative(0.f, 0.8f, 0.5f, 0.175f);
        lfoScaleSlider->setBoundsRelative(0.f, 0.14f, 0.1f, 0.575f);

        envButton->setBoundsRelative(0.5f, 0.8f, 0.5f, 0.175f);
        envScaleSlider->setBoundsRelative(0.9f, 0.14f, 0.1f, 0.575f);

        lfoScaleVisual->setBoundsRelative(0.1f, 0.14f, 0.8f, 0.575f);
        envScaleVisual->setSize(lfoScaleVisual->getWidth() / 1.5f, lfoScaleVisual->getHeight() / 1.5f); 
        envScaleVisual->setCentreRelative(0.5f, 0.5f);

        if (slider->getSliderStyle() == Slider::SliderStyle::LinearBarVertical)
        {
            sliderLabel->setBoundsRelative(0.125f, 0.f, 0.8f, 0.05f);
            slider->setBoundsRelative(0.25f, 0.1f, 0.5f, 0.775f);

            lfoButton->setBoundsRelative(0.f, 0.9f, 0.5f, 0.1f);
            envButton->setBoundsRelative(0.5f, 0.9f, 0.5f, 0.1f);

            lfoScaleSlider->setBoundsRelative(0.f, 0.1f, 0.1f, 0.775f);
            envScaleSlider->setBoundsRelative(0.9f, 0.1f, 0.1f, 0.775f);

            lfoScaleVisual->setBoundsRelative(0.25f, 0.1f, 0.1f, 0.775f);
            envScaleVisual->setBoundsRelative(0.64f, 0.1f, 0.1f, 0.775f);
        }
    }

    void buttonClicked(Button* b) override
    {
        // assign or remove a modulating source based on if audioProcessor is in "mapping state"
        if (b == lfoButton.get())
        {
            if (audioProcessor.isMappingLFO()) // 'isMapping' set by LFO button
            {
                RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(paramArray[ParamStrings::lfoSource]);
                
                pParam->beginChangeGesture();
                pParam->setValueNotifyingHost(pParam->convertTo0to1(audioProcessor.getLFOSource()));
                pParam->endChangeGesture();
                audioProcessor.setMappingLFO(false, 0.f);
            }
            else
            {
                RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(paramArray[ParamStrings::lfoSource]);
                lfoNum = 0.f;

                pParam->beginChangeGesture();
                pParam->setValueNotifyingHost(pParam->convertTo0to1(lfoNum));
                pParam->endChangeGesture();
                audioProcessor.setMappingLFO(false, 0.f);
            }
        }
        

        if (b == envButton.get())
        {
            if (audioProcessor.isMappingEnv()) // assign or remove a envelope from mapping
            {
                RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(paramArray[ParamStrings::envSource]);

                pParam->beginChangeGesture();
                pParam->setValueNotifyingHost(pParam->convertTo0to1(audioProcessor.getEnvSource()));
                pParam->endChangeGesture();
                audioProcessor.setMappingEnv(false, 0.f);
            }
            else
            {
                RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(paramArray[ParamStrings::envSource]);
                envNum = 0.f;

                pParam->beginChangeGesture();
                pParam->setValueNotifyingHost(pParam->convertTo0to1(envNum));
                pParam->endChangeGesture();
                audioProcessor.setMappingEnv(false, 0.f);
            }
        }
    }

private:
    NameFeel altFeel;
    ArtieFeel artieFeel;

    std::unique_ptr<Slider> slider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sliderAttach;
    std::unique_ptr<Label> sliderLabel;

    
    std::unique_ptr<Slider> lfoScaleSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoScaleAttach;
    std::unique_ptr<Slider> lfoScaleVisual;

    std::unique_ptr<Slider> envScaleSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> envScaleAttach;
    std::unique_ptr<Slider> envScaleVisual;



    std::unique_ptr<TextButton> lfoButton;
    std::unique_ptr < AudioProcessorValueTreeState::ButtonAttachment> lfoAttach;

    std::unique_ptr<TextButton> envButton;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> envAttach;

    StringArray paramArray;
    Colour lfoColor, envColor, sliderColor;
    float lfoNum=0, envNum=0; // used to recall what modulator is modifying this param (if any)

    bool isRotary = true;
    GayPolyCommunistAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscSlider)
};
