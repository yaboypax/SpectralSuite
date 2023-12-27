
#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace margins
{
    int xMargin = 36;
    int yMargin = 14;
}

//==============================================================================
SpectralSuiteAudioProcessorEditor::SpectralSuiteAudioProcessorEditor (SpectralSuiteAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
  
    setSize (563, 287);

    layoutGainSliders();
    layoutFxSliders();
    layoutButtons();

    addAttachments();
    loadFxMode();

}

SpectralSuiteAudioProcessorEditor::~SpectralSuiteAudioProcessorEditor()
{
    scrambleButton.removeListener(this);
    smearButton.removeListener(this);
    contrastButton.removeListener(this);

    fx1.removeListener(this);
    fx2.removeListener(this);
    fx3.removeListener(this);
    fx4.removeListener(this);

    inputGain.removeListener(this);
    outputGain.removeListener(this);
    pitchShift.removeListener(this);
    
    randomizeButton.removeListener(this);

    inputGainAttachment.reset();
    outputGainAttachment.reset();
    pitchShiftAttachment.reset();
    dryWetAttachment.reset();

    scramblingWidthAttachment.reset();
    smearingWidthAttachment.reset();
    contrastValueAttachment.reset();
}


//==============================================================================
void SpectralSuiteAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(subtleBlack);

    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 23);
    bounds.reduce(5, 5);

    if (fxMode == FxMode::scramble)
    {
        fx2.setVisible(true);
        fx3.setVisible(false);
        fx4.setVisible(false);

        g.setColour(scrambleColor);

    }
    else if (fxMode == FxMode::smear)
    {
        fx2.setVisible(false);
        fx3.setVisible(true);
        fx4.setVisible(false);

        g.setColour(smearColor);
    }
    else if (fxMode == FxMode::contrast)
    {
        fx2.setVisible(false);
        fx3.setVisible(false);
        fx4.setVisible(true);

        g.setColour(contrastColor);
    }
    g.fillRoundedRectangle(bounds.toFloat(), 23);


    displayText(g);

    bounds.reduce(5, 5);
    g.setFont(40.f);
    g.drawText(" SPECTRAL SUITE | YA BOY PAX", bounds, juce::Justification::topLeft);
 }

void SpectralSuiteAudioProcessorEditor::resized()
{    

    layoutGainSliders();
    layoutFxSliders();
    layoutButtons();
   
}

void SpectralSuiteAudioProcessorEditor::displayText(juce::Graphics& g)
{
    Font font = ssLookAndFeel.loadCustomFont().withHeight(24.0f);
    g.setFont(font);
    g.setColour(juce::Colours::black);

    //fx knobs
    if (fx1.isMouseOverOrDragging())
    {
        g.drawText(dryWetText, fx1.getX(), fx1.getY() + 5, fx1.getWidth(), fx1.getHeight(), juce::Justification::centred);
    }
    else
    {
        g.drawText("dry|wet", fx1.getX(), fx1.getY() + 5, fx1.getWidth(), fx1.getHeight(), juce::Justification::centred);
    }

    if (fx2.isMouseOverOrDragging() || fx3.isMouseOverOrDragging() || fx4.isMouseOverOrDragging())
    {
        g.drawText(effectText, fx2.getX(), fx2.getY() + 5, fx2.getWidth(), fx2.getHeight(), juce::Justification::centred);
    }
    else
    {
        g.drawText("effect", fx2.getX(), fx2.getY() + 5, fx2.getWidth(), fx2.getHeight(), juce::Justification::centred);
    }

    //in out sliders
    auto inOutYMargin = 30;
    auto inOutXMargin = 3;
    auto scalar = 1.2f;
    if (inputGain.isMouseOverOrDragging())
    {
        g.drawText(inText, inputGain.getX() - inOutXMargin, inputGain.getY() + inOutYMargin, inputGain.getWidth() * scalar, inputGain.getHeight(), juce::Justification::centredBottom);
    }
    else
    {
        g.drawText("in", inputGain.getX() - inOutXMargin, inputGain.getY() + inOutYMargin, inputGain.getWidth() * scalar, inputGain.getHeight(), juce::Justification::centredBottom);
    }

    if (outputGain.isMouseOverOrDragging())
    {
        g.drawText(outText, outputGain.getX() - inOutXMargin, outputGain.getY() + inOutYMargin, outputGain.getWidth() * scalar, outputGain.getHeight(), juce::Justification::centredBottom);
    }
    else
    {
        g.drawText("out", outputGain.getX() - inOutXMargin, outputGain.getY() + inOutYMargin, outputGain.getWidth() * scalar, outputGain.getHeight(), juce::Justification::centredBottom);
    }

    //pitch slider
    auto pitchTextMargin = inOutYMargin * 2;
    if (pitchShift.isMouseOverOrDragging())
    {
        g.drawText(pitchText, pitchShift.getX() - pitchTextMargin, pitchShift.getY(), pitchShift.getWidth(), pitchShift.getHeight() * scalar, juce::Justification::centredLeft);
    }
    else
    {
        g.drawText("pitch", pitchShift.getX() - pitchTextMargin, pitchShift.getY(), pitchShift.getWidth(), pitchShift.getHeight() * scalar, juce::Justification::centredLeft);
    }
}

void SpectralSuiteAudioProcessorEditor::layoutGainSliders()
{
    inputGain.setSliderStyle(juce::Slider::LinearVertical);
    inputGain.setLookAndFeel(&ssLookAndFeel);
    inputGain.setRange(0.0f, 1.5f, 1.0f);
    inputGain.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    inputGain.setPopupDisplayEnabled(false, false, this);
    inputGain.setTextValueSuffix(" inVolume");


    addAndMakeVisible(&inputGain);

    outputGain.setSliderStyle(juce::Slider::LinearVertical);
    outputGain.setLookAndFeel(&ssLookAndFeel);
    outputGain.setRange(0.0f, 1.5f, 1.0f);
    outputGain.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    outputGain.setPopupDisplayEnabled(false, false, this);
    outputGain.setTextValueSuffix(" outVolume");

    addAndMakeVisible(&outputGain);
   
    pitchShift.setSliderStyle(juce::Slider::LinearHorizontal);
    pitchShift.setLookAndFeel(&ssLookAndFeel);
    pitchShift.setRange(-24.0f, 24.0f, 0.0f);
    pitchShift.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    pitchShift.setPopupDisplayEnabled(false, false, this);

    addAndMakeVisible(&pitchShift);

    inputGain.setBounds(90 - margins::xMargin, 60 - margins::yMargin, 20, 185);
    outputGain.setBounds(150 - margins::xMargin, 60 - margins::yMargin, 20, 185);
    pitchShift.setBounds(254 - margins::xMargin, 251 - margins::yMargin, 287, 20);
}

void SpectralSuiteAudioProcessorEditor::layoutFxSliders()
{
    //drywet
    addAndMakeVisible(&fx1);
    fx1.setLookAndFeel(&ssLookAndFeel);
    fx1.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fx1.setRange(0.0, 1.0, 0.01);
    fx1.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    fx1.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fx1.setPopupDisplayEnabled(false, false, this);

    //scramble
    addChildComponent(&fx2);
    fx2.setLookAndFeel(&ssLookAndFeel);
    fx2.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fx2.setRange(0.0, 1.0, 0.01);
    fx2.setSkewFactorFromMidPoint(0.10);
    fx2.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    fx2.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fx2.setPopupDisplayEnabled(false, false, this);

    if (auto param = processor.apvts.getParameter("scramblingWidth"))
    {
        fx2.setValue(param->getValue());
    }

    //smear
    addChildComponent(&fx3);
    fx3.setLookAndFeel(&ssLookAndFeel);
    fx3.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fx3.setRange(0.0, 1.0, 0.01);
    fx3.setSkewFactorFromMidPoint(0.25);;
    fx3.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    fx3.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fx3.setPopupDisplayEnabled(false, false, this);


    if (auto param = processor.apvts.getParameter("smearingWidth"))
    {
        fx3.setValue(param->getValue());
    }


    //contrast
    addChildComponent(&fx4);
    fx4.setLookAndFeel(&ssLookAndFeel);
    fx4.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fx4.setRange(0.0, 1.0, 0.01);
    fx4.setSkewFactor(0.5);
    fx4.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    fx4.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fx4.setPopupDisplayEnabled(false, false, this);


    if (auto param = processor.apvts.getParameter("contrastValue"))
    {
        fx4.setValue(param->getValue());
    }


    fx1.setBounds(231, 119, 105, 105);
    fx2.setBounds(380, 119, 105, 105);
    fx3.setBounds(380, 119, 105, 105);
    fx4.setBounds(380, 119, 105, 105);
}

void SpectralSuiteAudioProcessorEditor::layoutButtons()
{   
    addAndMakeVisible(&scrambleButton);
    scrambleButton.setRadioGroupId(1);
    scrambleButton.setClickingTogglesState(true);
    scrambleButton.setLookAndFeel(&ssLookAndFeel);
    scrambleButton.setButtonText("scramble");
    scrambleButton.setColour(juce::TextButton::buttonColourId, scrambleColor);
    scrambleButton.setColour(juce::TextButton::buttonOnColourId, scrambleColor);

    addAndMakeVisible(&smearButton);
    smearButton.setRadioGroupId(1);
    smearButton.setClickingTogglesState(true);
    smearButton.setLookAndFeel(&ssLookAndFeel);
    smearButton.setButtonText("smear");
    smearButton.setColour(juce::TextButton::buttonColourId, smearColor);
    smearButton.setColour(juce::TextButton::buttonOnColourId, smearColor);

    addAndMakeVisible(&contrastButton);
    contrastButton.setRadioGroupId(1);
    contrastButton.setClickingTogglesState(true);
    contrastButton.setLookAndFeel(&ssLookAndFeel);
    contrastButton.setButtonText("contrast");
    contrastButton.setColour(juce::TextButton::buttonColourId, contrastColor);
    contrastButton.setColour(juce::TextButton::buttonOnColourId, contrastColor);


    int buttonY = 52;
    scrambleButton.setBounds(216, buttonY, 94, 45);
    smearButton.setBounds(316, buttonY, 94, 45);
    contrastButton.setBounds(421, buttonY, 94, 45);


    randomizeButton.setColour(juce::TextButton::buttonColourId, contrastColor);
    
    if (fxMode == FxMode::scramble)
    {
        randomizeButton.setColour(juce::TextButton::buttonColourId, scrambleColor);
    }
    else if (fxMode == FxMode::smear)
    {
        randomizeButton.setColour(juce::TextButton::buttonColourId, smearColor);
    }
    else if (fxMode == FxMode::contrast)
    {
        randomizeButton.setColour(juce::TextButton::buttonColourId, contrastColor);
    }
    auto x = 512;
    auto y = 11;
    auto size = 30;

    randomizeButton.setBounds(x, y, size, size);
    addAndMakeVisible(&randomizeButton);
}

void SpectralSuiteAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &scrambleButton)
    {
        fxMode = FxMode::scramble;
        effectText = juce::String(fx2.getValue());
    }
    else if (button == &smearButton)
    {
        fxMode = FxMode::smear;
        effectText = juce::String(fx3.getValue());
    }
    else if (button == &contrastButton)
    {
        fxMode = FxMode::contrast;
        effectText = juce::String(fx4.getValue());
    }
    else if (button == &randomizeButton)
    {
        processor.randomize();
    }

    layoutButtons();
    repaint();
}

void SpectralSuiteAudioProcessorEditor::addAttachments()
{
    scrambleButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "scrambleEnabled", scrambleButton);
    smearButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "smearEnabled", smearButton);
    contrastButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "contrastEnabled", contrastButton);

    inputGain.addListener(this);
    outputGain.addListener(this);
    pitchShift.addListener(this);

    scrambleButton.addListener(this);
    smearButton.addListener(this);
    contrastButton.addListener(this);

    randomizeButton.addListener(this);

    fx1.addListener(this);
    fx2.addListener(this);
    fx3.addListener(this);
    fx4.addListener(this);

    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "inputGain", inputGain);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "outputGain", outputGain);

    pitchShiftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "pitchShift", pitchShift);
    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "dryWet", fx1);

    scramblingWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "scramblingWidth", fx2);
    smearingWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "smearingWidth", fx3);
    contrastValueAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "contrastValue", fx4);
}

void SpectralSuiteAudioProcessorEditor::loadFxMode()
{
    if (processor.apvts.getParameter("scrambleEnabled")->getValue() == 1.f)
    {
        fxMode = FxMode::scramble;
        effectText = juce::String(fx2.getValue());
    }
    else if (processor.apvts.getParameter("smearEnabled")->getValue() == 1.f)
    {
        fxMode = FxMode::smear;
        effectText = juce::String(fx3.getValue());
    }
    else if (processor.apvts.getParameter("contrastEnabled")->getValue() == 1.f)
    {
        fxMode = FxMode::contrast;
        effectText = juce::String(fx4.getValue());
    }
    repaint();
}

void SpectralSuiteAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &fx1)
    {
        dryWetText = juce::String(slider->getValue());
        repaint();
    }

    if (slider == &fx2 || slider == &fx3 || slider == &fx4)
    {
        effectText = juce::String(slider->getValue());
        repaint();
    }

    if (slider == &inputGain)
    {
        inText = juce::String(slider->getValue());
        repaint();
    }

    if (slider == &outputGain)
    {
        outText = juce::String(slider->getValue());
        repaint();
    }

    if (slider == &pitchShift)
    {
        pitchText = juce::String(slider->getValue());
        repaint();
    }
}

void SpectralSuiteAudioProcessorEditor::sliderDragEnded(juce::Slider* slider)
{
    if (slider == &inputGain)
    {
        inText = "in";
        repaint();
    }

    if (slider == &outputGain)
    {
        outText = "out";
        repaint();
    }

    if (slider == &pitchShift)
    {
        pitchText = "pitch";
        repaint();
    }
}