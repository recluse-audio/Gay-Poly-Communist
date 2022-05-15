/*
  ==============================================================================

    MainMenuComponent.h
    Created: 12 Jan 2022 10:37:58am
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class MainMenuComponent  : public juce::Component
{
public:
    MainMenuComponent()
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.

    }

    ~MainMenuComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {

    }

    void resized() override
    {


    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainMenuComponent)
};
