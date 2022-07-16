/*
  ==============================================================================

    LFOSlider.h
    Created: 6 Dec 2021 10:26:13am
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*   Base class for modulator, designed for an lfo
*   inherit from this for an envSlider
*/
class LFOSlider  : public juce::Component, public Button::Listener
{
public:
    enum Parameters
    {
        rate, rateSource, rateScale, // lfo rate, mod source (envs only), and scaling of modulating env
        depth, depthSource, depthScale // lfo depth, mod source (envs only), and scaling of modulating env
    };

    LFOSlider(StringArray lfoParams, GayPolyCommunistAudioProcessor& p, float lNum) : audioProcessor(p), lfoNum(lNum), paramStrings(lfoParams)
    {
        initSliders();
        initButtons();
        initAttachments();
        initLabels(lfoNum);
        
        rateModColor = Colours::transparentBlack.withAlpha(0.f);
        depthModColor = Colours::transparentBlack.withAlpha(0.f);
    }

    ~LFOSlider() override
    {
    }

    void initSliders()
    {
        rateSlider = std::make_unique<Slider>(Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
        depthSlider = std::make_unique<Slider>(Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);

        rateScaleSlider = std::make_unique<Slider>(Slider::SliderStyle::LinearVertical, Slider::TextEntryBoxPosition::NoTextBox);
        depthScaleSlider = std::make_unique<Slider>(Slider::SliderStyle::LinearVertical, Slider::TextEntryBoxPosition::NoTextBox);

        rateScaleVisual = std::make_unique<Slider>(Slider::SliderStyle::Rotary, Slider::TextEntryBoxPosition::NoTextBox);
        depthScaleVisual = std::make_unique<Slider>(Slider::SliderStyle::Rotary, Slider::TextEntryBoxPosition::NoTextBox);

        rateScaleVisual->setLookAndFeel(&altFeel);
        depthScaleVisual->setLookAndFeel(&altFeel);

        addAndMakeVisible(rateScaleVisual.get());
        addAndMakeVisible(depthScaleVisual.get());

        addAndMakeVisible(rateSlider.get());
        addAndMakeVisible(depthSlider.get());
        addAndMakeVisible(rateScaleSlider.get());
        addAndMakeVisible(depthScaleSlider.get());

    }

    void initButtons()
    {
        mapButton = std::make_unique<TextButton>("Map");
        rateModButton = std::make_unique<TextButton>("Env");
        depthModButton = std::make_unique<TextButton>("Env");

        mapButton->addListener(this);
        rateModButton->addListener(this);
        depthModButton->addListener(this);

        addAndMakeVisible(mapButton.get());
        addAndMakeVisible(rateModButton.get());
        addAndMakeVisible(depthModButton.get());
    }

    void initAttachments()
    {
        rateAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getValueTree(), paramStrings[Parameters::rate], *rateSlider.get());

        depthAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getValueTree(), paramStrings[Parameters::depth], *depthSlider.get());

        rateScaleAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getValueTree(), paramStrings[Parameters::rateScale], *rateScaleSlider.get());

        depthScaleAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getValueTree(), paramStrings[Parameters::depthScale], *depthScaleSlider.get());
    }

    void initLabels(int lNum)
    {
        rateLabel = std::make_unique<Label>("Rate");
        rateLabel->setText("Rate", NotificationType::dontSendNotification);
        rateLabel->attachToComponent(rateSlider.get(), false);
        addAndMakeVisible(rateLabel.get());

        depthLabel = std::make_unique<Label>("Depth");
        depthLabel->setText("Depth", NotificationType::dontSendNotification);
        depthLabel->attachToComponent(depthSlider.get(), false);
        addAndMakeVisible(depthLabel.get());

        String labelString = "LFO"; String lfoNumString(lNum);
        labelString.append(lfoNumString, 4);

        label = std::make_unique<Label>(labelString);
        label->setText(labelString, NotificationType::dontSendNotification);
        addAndMakeVisible(label.get());
    }

    void paint (juce::Graphics& g) override
    {
        checkColors();

        mapButton->setColour(juce::TextButton::buttonColourId, color.darker());
       
        drawRotaryModVisual(rateSlider.get(), rateScaleSlider.get(), rateScaleVisual.get(), (int)rateModNum);
        drawRotaryModVisual(depthSlider.get(), depthScaleSlider.get(), depthScaleVisual.get(), (int)depthModNum);
        // boundary rect for gui design (remove for final product)
        auto frame = Rectangle<float>(getLocalBounds().toFloat());

        g.setColour(color.withRotatedHue(0.1f));
        g.drawRoundedRectangle(frame, 2.f, 3.f);

        g.setColour(color.darker(0.6f).withAlpha(0.2f));
        g.fillAll();
    }

    void drawRotaryModVisual(Slider* slider, Slider* scaleSlider, Slider* visualSlider, int modNum)
    {
        // getting radians and value of the slider
        auto rotaryParams = slider->getRotaryParameters();
        auto val = (float)slider->getValue();

        // This is to account for params like "filter freq" whose slider returns a non-normalized value (surprisingly)
        auto range = NormalisableRange<float>(slider->getRange().getStart(), slider->getRange().getEnd(), 0.1f, slider->getSkewFactor());
        float normalizedVal = range.convertTo0to1(val);
        auto sliderValRadians = jmap(normalizedVal, rotaryParams.startAngleRadians, rotaryParams.endAngleRadians);
        auto radianRange = rotaryParams.endAngleRadians - rotaryParams.startAngleRadians;

        auto envScaleVal = getEnvScaleVal(scaleSlider);
        auto envRadians = radianRange * envScaleVal * getEnvSustain(modNum);
        auto envRotaryEnd = jlimit<float>(rotaryParams.startAngleRadians, rotaryParams.endAngleRadians, sliderValRadians + envRadians);

        rateScaleVisual->setColour(Slider::rotarySliderFillColourId, rateModColor);
        depthScaleVisual->setColour(Slider::rotarySliderFillColourId, depthModColor);

        if (envRotaryEnd > sliderValRadians)
        {
            visualSlider->setRotaryParameters(sliderValRadians, envRotaryEnd, true);
        }
        else // if the env is inverted and going down
        {
            visualSlider->setRotaryParameters(envRotaryEnd, sliderValRadians, true);
        }
    }

    float getEnvSustain(int modNum)
    {
        switch (modNum)
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
    float getEnvScaleVal(Slider* scaleSlider)
    {
        float envVal = (scaleSlider->getValue() - 0.5f) * 2.f;
        return envVal;
    }

    // checking for mod sources that will change colors of mod buttons and modVisuals
    void checkColors()
    {
        switch ((int)rateModNum)
        {
        case 0: rateModColor = color.darker(); break;
        case 1: rateModColor = Colours::cyan; break;
        case 2: rateModColor = Colours::magenta; break;
        case 3: rateModColor = Colours::yellow; break;
        }

        switch ((int)depthModNum)
        {
        case 0: depthModColor = color.darker(); break;
        case 1: depthModColor = Colours::cyan; break;
        case 2: depthModColor = Colours::magenta; break;
        case 3: depthModColor = Colours::yellow; break;
        }

        if (audioProcessor.isMappingEnv())
        {
            rateModColor = Colours::white;
            depthModColor = Colours::white;
        }

        rateModButton->setColour(TextButton::ColourIds::buttonColourId, rateModColor);
        depthModButton->setColour(TextButton::ColourIds::buttonColourId, depthModColor);

    }

    void resized() override
    {
        label->setBoundsRelative(0.25f, 0.02f, 0.5f, 0.25f);

        rateSlider->setBoundsRelative(0.f, 0.02f, 0.45f, 0.96f);
        depthSlider->setBoundsRelative(0.55f, 0.02f, 0.45f, 0.96f);

        rateScaleVisual->setBoundsRelative(0.f, 0.02f, 0.45f, 0.96f);
        // rateScaleVisual->setCentreRelative(0.25f, 0.25f);
        depthScaleVisual->setBoundsRelative(0.55f, 0.02f, 0.45f, 0.96f);

        rateScaleSlider->setBoundsRelative(0.05f, 0.05f, 0.05f, 0.9f);
        depthScaleSlider->setBoundsRelative(0.9f, 0.05f, 0.05f, 0.9f);


        //depthScaleVisual->setCentreRelative(0.75f, 0.25f);

        rateLabel->setBoundsRelative(0.075f, 0.7f, 0.3f, 0.25f);
        depthLabel->setBoundsRelative(0.625f, 0.7f, 0.3f, 0.25f);

        mapButton->setBoundsRelative(0.425f, 0.3f, 0.15f, 0.25f);
        rateModButton->setBoundsRelative(0.375f, 0.73f, 0.125f, 0.2f);
        depthModButton->setBoundsRelative(0.5f, 0.73f, 0.125f, 0.2f);

    }

    void buttonClicked(Button* b) override
    {
        if (b == mapButton.get())
        {
            audioProcessor.setMappingLFO(true, lfoNum);
        }

        if (b == rateModButton.get())
        {
            if (audioProcessor.isMappingEnv()) // sets mapping env for rate
            {
                RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(paramStrings[Parameters::rateSource]);
                rateModNum = audioProcessor.getEnvSource();

                rateIsModulated = true;

                pParam->beginChangeGesture();
                pParam->setValueNotifyingHost(pParam->convertTo0to1(rateModNum));
                pParam->endChangeGesture();
                audioProcessor.setMappingEnv(false, 0.f);
            }
            else // removes mapping for rate
            { 
                RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(paramStrings[Parameters::rateSource]);
                rateModNum = 0.f;
                
                rateIsModulated = false;
                
                pParam->beginChangeGesture();
                pParam->setValueNotifyingHost(pParam->convertTo0to1(rateModNum));
                pParam->endChangeGesture();
                audioProcessor.setMappingEnv(false, 0.f);
            }
        }

        if (b == depthModButton.get())
        {
            if (audioProcessor.isMappingEnv()) // sets mapping for depth
            {
                RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(paramStrings[Parameters::depthSource]);
                depthModNum = audioProcessor.getEnvSource();

                depthIsModulated = true;

                pParam->beginChangeGesture();
                pParam->setValueNotifyingHost(pParam->convertTo0to1(depthModNum));
                pParam->endChangeGesture();
                audioProcessor.setMappingEnv(false, 0.f);
            }
            else // removes mapping for depth
            {
                RangedAudioParameter* pParam = audioProcessor.getValueTree().getParameter(paramStrings[Parameters::depthSource]);
                depthModNum = 0.f;

                depthIsModulated = false;

                pParam->beginChangeGesture();
                pParam->setValueNotifyingHost(pParam->convertTo0to1(depthModNum));
                pParam->endChangeGesture();
                audioProcessor.setMappingEnv(false, 0.f);
            }
        }
    }

    // used to track if a mod is currently being mapped to a param
    void setMappingState(bool currentlyMapping)
    {
        isMapping = currentlyMapping;
    }

    float getLFONum()
    {
        return lfoNum;
    }

    void setColor(Colour c)
    {
        color = c;
        label->setColour(Label::backgroundColourId, color.brighter());
        mapButton->setColour(TextButton::buttonColourId, color.withAlpha(1.0f));
        rateModButton->setColour(TextButton::buttonColourId, color);
        depthModButton->setColour(TextButton::buttonColourId, color);

        rateLabel->setColour(Label::backgroundColourId, color);
        depthLabel->setColour(Label::backgroundColourId, color);
        rateSlider->setColour(Slider::backgroundColourId, color);
        depthSlider->setColour(Slider::backgroundColourId, color);
    }

private:
    NameFeel altFeel;
    bool isMapping = false; // changes color, activated by clicking button, resets when mapped or cancels when clicking outsid of map destinations
    float lfoNum = 0; // referring to THIS lfo's number

    float rateModNum = 0.f, depthModNum = 0.f; // used to determine rateScaleVisual & depthScaleVisual colors to match modulating envelope... see: checkColors()
    bool rateIsModulated = false, depthIsModulated = false;

    GayPolyCommunistAudioProcessor& audioProcessor;

    std::unique_ptr<Slider> rateSlider; // slider that controls rate of an lfo (modulator)
    std::unique_ptr<Slider> depthSlider; // slider that controls depth of an lfo
    std::unique_ptr<Slider> rateScaleSlider;
    std::unique_ptr<Slider> depthScaleSlider;
    std::unique_ptr<Slider> rateScaleVisual;
    std::unique_ptr<Slider> depthScaleVisual;

    std::unique_ptr<TextButton> mapButton;
    std::unique_ptr<TextButton> rateModButton;
    std::unique_ptr<TextButton> depthModButton;

    std::unique_ptr<Label> label;
    std::unique_ptr<Label> rateLabel;
    std::unique_ptr<Label> depthLabel;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> rateAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> depthAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> rateScaleAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> depthScaleAttach;

    StringArray paramStrings;
    Colour color{ 255, 255, 255 }, rateModColor, depthModColor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOSlider)
};
