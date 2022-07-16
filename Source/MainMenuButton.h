/*
  ==============================================================================

    MainMenuButton.h
    Created: 12 Jan 2022 10:33:39am
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


//==============================================================================
/*
*/
class MainMenuButton  : public juce::Component, Button::Listener
{
public:
    MainMenuButton(AudioProcessor& p) : audioProcessor(p)
    {

        button = std::make_unique<TextButton>("");
        button->addListener(this);
        button->setColour(TextButton::buttonColourId, Colours::transparentBlack.withAlpha(0.f));
        
        auto svg_xml = XmlDocument::parse(BinaryData::LOGO_SVG_svg); // GET THE SVG AS A XML
        jassert(svg_xml != nullptr);

        if (svg_xml != nullptr)
        {
            logo = Drawable::createFromSVG(*svg_xml.get());
            logo->setAlwaysOnTop(true);
        }
        
        
        

    }

    ~MainMenuButton() override
    {
    }

    void prepareMenu()
    {
        menu.clear();
        //menu.setLookAndFeel(&pulsarFeel);

        menu.addSectionHeader("Presets");
        menu.addSeparator();

        int itemIndex = 1; // used to properly index artist romples without resetting in the artist loop

//        for (int artistIndex = 0; artistIndex < database.artists.size(); artistIndex++) // Artists - just a section header
//        {
//            auto artist = database.artists[artistIndex];
//            menu.addSectionHeader(artist->getArtistName());
//
//            for (int categoryIndex = 0; categoryIndex < artist->getNumCategories(); categoryIndex++)
//            {
//
//                auto categoryMenu = new PopupMenu();
//                auto category = artist->getCategory(categoryIndex);
//                auto romples = category.getRompleArray();
//
//                for (int rompleIndex = 0; rompleIndex < romples.size(); rompleIndex++)
//                {
//                    categoryMenu->addItem(itemIndex, romples[rompleIndex]); // item index keeps track of the different categories
//                    itemIndex++;
//                }
//
//                menu.addSubMenu(category.getName(), *categoryMenu);
//            }
//
//        }
    }
    
    void paint (juce::Graphics& g) override
    {
        g.setColour(Colours::white);
        auto rect = Rectangle<float>(0, 0, getWidth(), getHeight());
        g.drawRoundedRectangle(rect, 1.f, 2.f);

        logo->setTransformToFit(getLocalBounds().toFloat().reduced(2.f), RectanglePlacement::centred);
        logo->draw(g, 1.f);

        //g.fillAll (Colours::black);   // clear the background
    }

    void resized() override
    {
        button->setBoundsRelative(0.f, 0.f, 1.f, 1.f);
        
       // logo->setBoundsRelative(0.f, 0.f, 1.f, 1.f);

    }

    void buttonClicked(Button* b) override
    {
        if (b == button.get())
        {
            DBG("test");
        }
    }
private:
    std::unique_ptr<TextButton> button;
    std::unique_ptr<Drawable> logo;
    juce::PopupMenu menu;

    AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainMenuButton)
};
