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
        Font font = loadCustomFont().withHeight(32.f);
        g.setFont(font);
        g.setColour(Colours::black);

        const int yIndent = jmin(4, button.proportionOfHeight(2.0f));
        const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

        const int fontHeight = roundToInt(font.getHeight());
        const int leftIndent = jmin(fontHeight, cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = jmin(fontHeight, cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText(button.getButtonText(),
                leftIndent, yIndent + 5, textWidth, button.getHeight() - yIndent * 2,
                Justification::centred, 2);
    }

private:
};