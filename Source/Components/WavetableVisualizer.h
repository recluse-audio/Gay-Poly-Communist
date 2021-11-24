/*
  ==============================================================================

    WavetableVisualizer.h
    Created: 6 Nov 2020 4:34:21pm
    Author:  Billie (Govnah) Jean

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WaveTableVector.h"
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class WavetableVisualizer : public juce::Component,
    public FileDragAndDropTarget //, juce::Timer
{
public:
    WavetableVisualizer(GayPolyCommunistAudioProcessor&);
    ~WavetableVisualizer() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void setValue();
    
    
    void setNewWaveColour(juce::Colour newColour)
    {
        waveColour = newColour;
    }
    
    void setAmp(float a)
    {
       amp = a;
    }

    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;
    
    
private:
    WaveTableVector& waveVector;
    GayPolyCommunistAudioProcessor& processor;
    
    std::unique_ptr<juce::Label> valueLabel;
    juce::String value = "value";

    float waveValue = 0.f;
    
    juce::Colour    waveColour;
    juce::Colour    bgColour;
    float amp = 1.f;
    float waveSat;
    float waveHue;
    float waveVal;
    float bgSat;
    float bgHue = 0.f;
    float bgVal;

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableVisualizer)
};
