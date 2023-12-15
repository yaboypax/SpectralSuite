#pragma once

#include <JuceHeader.h>
#include "Resource/Timmana.h"

using namespace juce;

class SSLookAndFeel : public LookAndFeel_V4
{
public:

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

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
        auto fill = slider.findColour(Slider::rotarySliderFillColourId);

        auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

        auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin(8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        g.setColour(outline);
        g.drawEllipse(bounds, 5);

        if (slider.isEnabled())
        {
            Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius,
                arcRadius,
                0.0f,
                rotaryStartAngle,
                toAngle,
                true);
        }

        float halfLineLength = 10.0f;

        Point<float> midPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
            bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
        Point<float> lineStart(midPoint.x - halfLineLength * std::cos(toAngle - MathConstants<float>::halfPi),
            midPoint.y - halfLineLength * std::sin(toAngle - MathConstants<float>::halfPi));
        Point<float> lineEnd(midPoint.x + halfLineLength * std::cos(toAngle - MathConstants<float>::halfPi),
            midPoint.y + halfLineLength * std::sin(toAngle - MathConstants<float>::halfPi));

        g.drawLine(Line<float>(lineStart, lineEnd), lineW);
    }

    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const Slider::SliderStyle style, Slider& slider)
    {
        auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

        Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : (float)y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(Colours::black);
        g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
        
        minPoint = startPoint;
        maxPoint = { kx, ky };

        auto thumbWidth = getSliderThumbRadius(slider);

        valueTrack.startNewSubPath(minPoint);
        g.setColour(Colours::black);
        g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        auto halfLineLength = 10.f;
        Point<float> lineStart, lineEnd;

        if (slider.isHorizontal())
        {
            
            lineStart = {maxPoint.x, maxPoint.y - halfLineLength};
            lineEnd = {maxPoint.x, maxPoint.y + halfLineLength};
        }
        else
        {
            lineStart = {maxPoint.x - halfLineLength, maxPoint.y};
            lineEnd = { maxPoint.x + halfLineLength , maxPoint.y};
        }

        g.setColour(Colours::black);
        g.drawLine(Line<float>(lineStart, lineEnd), 5);
    }

    Font loadCustomFont()
    {
        auto typeface = Typeface::createSystemTypefaceFor(Timmana::Timmana_Regular_ttf, Timmana::Timmana_Regular_ttfSize);
        return Font(typeface);
    }
};