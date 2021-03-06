/*
  ==============================================================================

    ArtieFeel.h
    Created: 15 Dec 2020 12:26:27pm
    Author:  Billie (Govnah) Jean

  ==============================================================================
*/
#pragma once
#include "JuceHeader.h"

class ArtieFeel : public juce::LookAndFeel_V4
{
public:
    ArtieFeel();
    ~ArtieFeel();
    
    void drawLinearSlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           const juce::Slider::SliderStyle,
        juce::Slider&) override;

    void drawRotarySlider(Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, Slider&) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider&) override;
    juce::Label* createSliderTextBox(juce::Slider&) override;
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    juce::Font getPopupMenuFont() override;
    juce::Font getLabelFont(juce::Label&) override;
    void drawLabel(Graphics& g, Label& l) override;

    Font getTextButtonFont(TextButton&, int buttonHeight) override { return getFont(); }

private:
    juce::Font getFont()
    {
        return Font ("Consolas", "Regular", 10.f);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArtieFeel)
};

