/*
  ==============================================================================

    NameFeel.h
    Created: 26 Nov 2021 11:22:07am
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class NameFeel : public juce::LookAndFeel_V4
{
public:
    NameFeel() 
    {
        setColour(juce::Slider::rotarySliderOutlineColourId, Colours::orange);
        setColour(juce::Slider::rotarySliderFillColourId, Colours::orange);

    }
    ~NameFeel() {}

    // used to draw rotary three value slider
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, Slider& slider) override
    {
        auto radius = jmin<float>(width / 2.5f, height / 2.625f);
        auto centreX = x + width * 0.5f;
        auto centreY = y + height * 0.55f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

        g.setColour(slider.findColour(Slider::textBoxOutlineColourId));

        if (slider.isEnabled())
            g.setColour(slider.findColour(Slider::rotarySliderFillColourId).withAlpha(isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour(Colour(0x80808080));



        auto lineThickness = jmin(15.0f, jmin(width, height) * 0.45f) * 0.2f;


        g.setColour(slider.findColour(Slider::rotarySliderFillColourId));
        Path outlineArc;
        outlineArc.addArc(rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, true);
        g.strokePath(outlineArc, PathStrokeType(lineThickness));

        g.setColour(slider.findColour(Slider::rotarySliderFillColourId).withAlpha(0.1f));
        g.fillPath(outlineArc);
    }
    Font getLabelFont(juce::Label&) override
    {
        return Font("Consolas", "Bold", 36.f);
    }

};