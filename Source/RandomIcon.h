#pragma once

#include "SSLookAndFeel.h"

using namespace juce;


class RandomIcon : public ImageButton
{
public:
    RandomIcon()
    {
        setClickingTogglesState(false);
        dice.setImage(ImageCache::getFromMemory(BinaryData::dice_png, BinaryData::dice_pngSize));
    }

    void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        dice.drawWithin(g, getLocalBounds().toFloat(), RectanglePlacement::fillDestination, 1.0f);
    }
private:
    DrawableImage dice;
};