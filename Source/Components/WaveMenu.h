/*
  ==============================================================================

    WaveMenu.h
    Created: 8 Dec 2021 2:24:13pm
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Processor/PluginProcessor.h"
#include "../SaveAndLoad/WaveLoader.h"
#include "../LookAndFeel/ArtieFeel.h"
//==============================================================================
/*
*/
class WaveMenu  : public juce::Component, Button::Listener
{
public:
    WaveMenu(GayPolyCommunistAudioProcessor& p, int oscillator) : audioProcessor(p), oscNum(oscillator)
    {
        setLookAndFeel(&artieFeel);

        button = std::make_unique<TextButton>("Wave Vector");
        button->addListener(this);
        addAndMakeVisible(button.get());
        
        menu = std::make_unique<PopupMenu>();
        prepareMenu();
    }

    ~WaveMenu() override
    {
    }

    void paint (juce::Graphics& g) override
    {

    }

    void resized() override
    {
        button->setBoundsRelative(0.f, 0.f, 1.f, 1.f);

    }

    void buttonClicked(Button* b) override
    {
        if (b == button.get())
        {
            auto parentScreen = getParentMonitorArea();
            auto menuArea = Rectangle<int>(getParentWidth() * 0.5f, parentScreen.getY(), getParentWidth() / 2, getParentHeight());
            
            //int selection = menu->showAt(menuArea, 0, 100);
            menu->showMenuAsync(PopupMenu::Options().withTargetScreenArea(menuArea),
           [this](int selection)
           {
                if (selection == 0)
                {
                    menu->dismissAllActiveMenus();
                }
                if (selection > 0)
                {
                    auto path = audioProcessor.getWaveLoader().getPathFromIndex(selection - 1);
                    audioProcessor.loadWaveTables(path, oscNum);
                }
            });
        }
    }

    void prepareMenu()
    {

        // auto menuArea = Rectangle<float>(getParentWidth() / 2, 0, getParentWidth() / 2, getParentHeight());
         //juce::ScopedPointer<juce::PopupMenu> artistsMenu = new juce::PopupMenu();
        menu->addSectionHeader("WaveTable Vectors");
        menu->addSeparator();
        menu->setLookAndFeel(&artieFeel);

        int itemIndex = 1; // used to properly index waves without resetting in the wave loop

        auto waveFolders = audioProcessor.getWaveLoader().getWaveFolders();

        for (size_t i = 0; i < waveFolders.size(); i++)
        {
            std::unique_ptr<PopupMenu> vectorMenu = std::make_unique<PopupMenu>();
            auto folder = waveFolders[i];

            for (size_t j = 0; j < folder->getNumWaves(); j++)
            {
                vectorMenu->addItem(itemIndex, folder->getWaveName(j));
                itemIndex++;
            }

            auto vectorName = waveFolders[i]->getVectorName();
            menu->addSubMenu(vectorName, *vectorMenu);
        }

       // menu.addSubMenu("Wave Tables", *vectorMenu);

    }
    void loadTables(OwnedArray<File>& fileArray)
    {

    }

    void setColor(Colour c)
    {
        button->setColour(TextButton::buttonColourId, c);
    }

private:
    ArtieFeel artieFeel;
    int oscNum = 1;
    std::unique_ptr<WaveLoader> database;
    std::unique_ptr<TextButton> button;
    std::unique_ptr<PopupMenu> menu;
    GayPolyCommunistAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveMenu)
};
