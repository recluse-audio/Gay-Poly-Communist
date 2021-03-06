
/*
 ==============================================================================
 
 LookAndFeel.cpp
 Created: 12 Jul 2020 8:49:21am
 Author:  Billie (Govnah) Jean
 
 ==============================================================================
 */

#include "ArtieFeel.h"

ArtieFeel::ArtieFeel()
{
    setColour(juce::Slider::backgroundColourId, juce::Colours::grey);
    setColour(juce::Slider::trackColourId, juce::Colours::darkred);

    setColour(juce::Slider::rotarySliderFillColourId, Colours::orange);

    setColour(juce::TextButton::textColourOnId, juce::Colours::antiquewhite);
    setColour(juce::TextButton::textColourOffId, juce::Colours::antiquewhite);
    setColour(juce::TextButton::buttonColourId, juce::Colours::black);
    setColour(juce::TextButton::buttonOnColourId, juce::Colours::yellow.withHue(0.2f));

    setColour(juce::PopupMenu::backgroundColourId, juce::Colours::black);

    setColour(juce::SidePanel::backgroundColour, juce::Colours::black);
    setColour(juce::SidePanel::shadowBaseColour, juce::Colours::black);


}

ArtieFeel::~ArtieFeel()
{
    
}



/*=================================================================================*/
/*----------------------------- drawRotarySlider ----------------------------------*/
/*=================================================================================*/
void ArtieFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider)
{
    
    if(slider.getSliderStyle() == juce::Slider::SliderStyle::LinearBarVertical)
    {
        auto color = slider.findColour(Slider::backgroundColourId);

        juce::Rectangle<float> bgFrame(x, y, width, height);

        g.setColour(color.brighter(0.9f));
        g.drawRoundedRectangle(bgFrame, 2.f, 3.f);

        g.setColour(color.withAlpha(0.1f));
        g.fillAll();
       
        juce::Rectangle<float> centerRect(width * 0.01f, sliderPos, width * 0.99f, height * 0.01f);
        g.setColour(color.brighter());
        g.drawRoundedRectangle(centerRect, 2.f, 2.f);

        g.setColour(color.brighter().withAlpha(0.4f));
        g.fillRoundedRectangle(centerRect, 2.f);

    }

    if (slider.getSliderStyle() == juce::Slider::SliderStyle::LinearBar)
    {
        juce::Rectangle<float> bgFrame(x, y, width, height);
        g.setColour(juce::Colours::blanchedalmond);
        g.drawRoundedRectangle(bgFrame, 5.f, 2.f);

        g.setColour(juce::Colours::transparentBlack.withAlpha(0.f));
        g.fillAll();

        juce::Rectangle<float> centerRect(x, sliderPos, width, 6);
        g.setColour(juce::Colours::orange);
        g.drawRoundedRectangle(centerRect, 2.f, 1.f);
       // g.drawRoundedRectangle(centerRect, 1.f);

    }

    if(slider.getSliderStyle() == juce::Slider::SliderStyle::LinearVertical)
    {
        juce::Rectangle<int> bgRect;
        bgRect.setX(x);
        bgRect.setY(y);
        bgRect.setSize(width, height);
        g.setColour(juce::Colours::blanchedalmond);
        g.drawRoundedRectangle(bgRect.toFloat(), 1.f, 1.f);
        
        
        juce::Rectangle<int> centerTick(x, height/2, width, 2);
        
        
        auto pos = sliderPos;
        if (pos >= height - 3)
            pos = height - 3;
        
        juce::Rectangle<int> thumb(0, pos, width, 2);
        g.setColour(juce::Colours::yellow);
        g.fillRoundedRectangle(thumb.toFloat(), 5.f);
        
        
        g.setColour(juce::Colours::blanchedalmond.darker());
        g.fillRoundedRectangle(centerTick.toFloat(), 3.f);
    }

    
    if (slider.getSliderStyle() == juce::Slider::SliderStyle::LinearHorizontal)
    {
        juce::Rectangle<int> bgRect;
        bgRect.setX(x);
        bgRect.setY(y);
        bgRect.setSize(width, height);
        g.setColour(juce::Colours::blanchedalmond);
        g.drawRoundedRectangle(bgRect.toFloat(), 2.f, 2.f);

        juce::Rectangle<int> centerTick(width/2, y, 1, height);
        
        g.setColour(juce::Colours::transparentBlack.withAlpha(0.f));
        g.fillAll();
        
        auto pos = sliderPos;
        if (pos >= width - 3)
            pos = width - 3;
        
        juce::Rectangle<int> thumb(pos, 0, 2, 20);
        g.setColour(juce::Colours::orange);
        g.fillRoundedRectangle(thumb.toFloat(), 2.f);
        
        g.setColour(juce::Colours::blanchedalmond.darker());
        g.fillRoundedRectangle(centerTick.toFloat(), 2.f);
    }
    
    if(slider.getSliderStyle() == Slider::ThreeValueVertical)
    {
        //juce::Rectangle<float> bgFrame(x, y, width, height);
        //g.setColour(juce::Colours::blanchedalmond);
        //g.drawRoundedRectangle(bgFrame, 5.f, 2.f);
        
        float h = jlimit<float>(1, height, minSliderPos - maxSliderPos);
        float y = jlimit<float>(2, height - 2, maxSliderPos);
        juce::Rectangle<float> fillRect(x, y, width, h);

        auto color = slider.findColour(Slider::backgroundColourId);
       // juce::ColourGradient gradient(color.darker(), width / 2, height, color.withHue(0.5f), width / 2, 0, false);
        g.setColour(color);
       // g.setGradientFill(gradient);
        g.fillRect(fillRect);
    }
    
    if(slider.getSliderStyle() == Slider::ThreeValueHorizontal)
    {
        juce::Rectangle<float> bgFrame(x, y, width, height);
        g.setColour(juce::Colours::blanchedalmond);
        g.drawRoundedRectangle(bgFrame, 2.f, 2.f);
        
        juce::Rectangle<float> fillRect(minSliderPos + 2, y, maxSliderPos-minSliderPos, height);

        auto color = slider.findColour(Slider::backgroundColourId);
        juce::ColourGradient gradient(color.darker(), 0, height/2, color.withHue(0.5f), width, height/2, false);
        g.setGradientFill(gradient);
        g.fillRect(fillRect);
    }

    if (slider.getSliderStyle() == Slider::IncDecButtons)
    {
        g.setColour(juce::Colours::red);
        g.fillAll();
    }
    
}
// pass color and amount to this
void ArtieFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, 
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle, Slider& slider)
{
    auto radius = jmin<float>(width / 2.f, height / 2.1f);
    auto centreX = x + width * 0.5f;
    auto centreY = y + height * 0.55f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    auto color = slider.findColour(Slider::backgroundColourId);

    g.setColour(color.withAlpha(0.f));
    g.fillAll();

    if (slider.isEnabled())
        g.setColour(color.withAlpha(isMouseOver ? 1.0f : 0.7f));
    else
        g.setColour(color);

    auto lineThickness = jmin(15.0f, jmin(width, height) * 0.45f) * 0.15f;

    
    g.setColour(color.darker());
    Path outlineArc;
    outlineArc.addArc(rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath(outlineArc, PathStrokeType(lineThickness));

    //g.setColour(slider.findColour(Slider::trackColourId).withAlpha(0.1f));
    //g.fillPath(outlineArc);

    // tick mark
    g.setColour(color.brighter().withRotatedHue(0.1f));
    juce::Path p;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 2.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.fillPath(p);

}

juce::Slider::SliderLayout ArtieFeel::getSliderLayout(juce::Slider& slider)
{
    juce::Slider::SliderLayout layout;
    auto bounds = slider.getLocalBounds();
    auto height = bounds.getHeight();
    layout.sliderBounds = bounds;

    if (slider.getSliderStyle() == Slider::SliderStyle::LinearBarVertical)
    {
        juce::Rectangle<int> textBox(0, height * 0.825, bounds.getWidth(), height * 0.175f);
        layout.textBoxBounds = textBox;
    }
    
    if (slider.getSliderStyle() == Slider::SliderStyle::IncDecButtons)
    {
        juce::Rectangle<int> altBox;
        juce::Rectangle<int> sliderBox;

        sliderBox.setBounds(0, 0, bounds.getWidth(), height * 0.5f);
        altBox.setBounds(0, height * 0.5f, bounds.getWidth(), height * 0.5f);
        layout.textBoxBounds = altBox;
        layout.sliderBounds = sliderBox;
    }


    return layout;
}


juce::Label* ArtieFeel::createSliderTextBox(juce::Slider& slider)
{
    
    juce::Label* l = juce::LookAndFeel_V4::createSliderTextBox(slider);
    // make sure editor text is black (so it shows on white background)
   
    l->setColour(juce::Label::outlineColourId, Colours::transparentBlack.withAlpha(0.0f));
    l->setColour(juce::Slider::textBoxBackgroundColourId, Colours::transparentBlack.withAlpha(0.f));
    l->setColour(Label::backgroundColourId, Colours::white);

    return l;
}

/*=================================== FONT =========================================*/

juce::Font ArtieFeel::getLabelFont(juce::Label& label)
{
   return getFont().withHeight(label.getHeight());
}

void ArtieFeel::drawLabel(Graphics& g, Label& label)
{
    auto color = label.findColour(Label::backgroundColourId);
    g.fillAll(color.withAlpha(0.f));

    g.setColour(color.brighter(0.9f).withRotatedHue(0.1f));
    auto font = getFont().withHeight(label.getHeight() * 0.8f);
    g.setFont(font);
    g.drawText(label.getText(),label.getLocalBounds(), Justification::centred, false);
}

void ArtieFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                                    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::Rectangle<int> bounds = button.getLocalBounds();
    
    auto color = button.findColour(TextButton::buttonColourId);
    g.setColour(color.brighter());
    g.drawRoundedRectangle(bounds.toFloat(), 3.f, 2.f);

    g.setColour(color.withAlpha(0.2f));
    g.fillAll();

    if(button.isDown())
    {
        g.setColour(button.findColour(juce::TextButton::buttonOnColourId).withAlpha(1.f));
        g.fillRoundedRectangle(bounds.toFloat(), 5.f);
    } 
    if(shouldDrawButtonAsHighlighted)
    {
        g.setColour(button.findColour(juce::TextButton::buttonColourId).withAlpha(0.5f));
        g.fillRoundedRectangle(bounds.toFloat(), 5.f);
    }
    if (button.getToggleState())
    {
        g.setColour(button.findColour(juce::TextButton::buttonColourId).withAlpha(0.8f));
        g.fillRoundedRectangle(bounds.toFloat(), 5.f);
    }
    
}



void ArtieFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{   
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(0, 0, width, height, 5.f, 2.f);

    g.setColour(juce::Colours::black);
    g.fillAll();
}

juce::Font ArtieFeel::getPopupMenuFont()
{
    return getFont().withHeight(16.f);
}






