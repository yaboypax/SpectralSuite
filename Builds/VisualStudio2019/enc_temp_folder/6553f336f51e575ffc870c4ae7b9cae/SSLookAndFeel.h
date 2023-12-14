#pragma once

#include <JuceHeader.h>
#include "Resource/Timmana.h"

using namespace juce;

class SSLookAndFeel : public LookAndFeel_V4
{
public:


    Font loadCustomFont()
    {
        auto typeface = Typeface::createSystemTypefaceFor(Timmana::Timmana_Regular_ttf, Timmana::Timmana_Regular_ttfSize);
        return Font(typeface);
    }

	void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
	{
            auto cornerSize = 9.0f;
            auto bounds = button.getLocalBounds();
            g.setColour(juce::Colours::black);
            g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
            bounds.reduce(5, 5);

            g.setColour(backgroundColour);
            g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
        
	}

    void drawButtonText(Graphics& g, TextButton& button, bool, bool) override
    {
        Font font = loadCustomFont();
        g.setFont(font);
        g.setColour(Colours::black);

        g.drawFittedText(button.getButtonText(), button.getBounds(), Justification::centred, 2);
    }

private:
};