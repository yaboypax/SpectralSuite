
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
SpectralSuiteAudioProcessorEditor::SpectralSuiteAudioProcessorEditor (SpectralSuiteAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
  
    setSize (563, 287);

    layoutGainSliders();
    layoutFxSliders();
    layoutButtons();

    addAttachments();

}

SpectralSuiteAudioProcessorEditor::~SpectralSuiteAudioProcessorEditor()
{
    scrambleButton.removeListener(this);
    smearButton.removeListener(this);
    contrastButton.removeListener(this);
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
 }

void SpectralSuiteAudioProcessorEditor::resized()
{    

    layoutGainSliders();
    layoutFxSliders();
    layoutButtons();
   
}

void SpectralSuiteAudioProcessorEditor::layoutGainSliders()
{
    inputGain.setSliderStyle(juce::Slider::LinearBarVertical);
    inputGain.setRange(0.0f, 1.0f, 1.0f);
    inputGain.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    inputGain.setPopupDisplayEnabled(true, false, this);
    inputGain.setTextValueSuffix(" inVolume");
    inputGain.setValue(0.8f);

    addAndMakeVisible(&inputGain);

    outputGain.setSliderStyle(juce::Slider::LinearBarVertical);
    outputGain.setRange(0.0f, 1.0f, 1.0f);
    outputGain.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    outputGain.setPopupDisplayEnabled(true, false, this);
    outputGain.setTextValueSuffix(" outVolume");
    outputGain.setValue(0.8f);

    addAndMakeVisible(&outputGain);
   
    pitchShift.setSliderStyle(juce::Slider::LinearHorizontal);
    pitchShift.setRange(-24.0f, 24.0f, 0.0f);
    pitchShift.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    pitchShift.setPopupDisplayEnabled(true, false, this);
    pitchShift.setValue(1.0f);

    addAndMakeVisible(&pitchShift);

    inputGain.setBounds(40, 30, 20, getHeight() - 60);
    outputGain.setBounds(80, 30, 20, getHeight() - 60);
    pitchShift.setBounds(120, 30, 120, 20);
}

void SpectralSuiteAudioProcessorEditor::layoutFxSliders()
{
    //drywet
    addAndMakeVisible(&fx1);
    fx1.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fx1.setRange(0.0, 1.0, 0.01);
    fx1.setValue(0.5);
    fx1.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    fx1.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    fx1.setColour(juce::Slider::thumbColourId, juce::Colours::transparentWhite);
    fx1.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fx1.setPopupDisplayEnabled(true, false, this);

    //scramble
    addChildComponent(&fx2);
    fx2.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fx2.setRange(0.0, 1.0, 0.01);
    fx2.setSkewFactorFromMidPoint(0.10);
    fx2.setValue(0.2);
    fx2.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    fx2.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::navajowhite);
    fx2.setColour(juce::Slider::thumbColourId, juce::Colours::transparentWhite);
    fx2.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fx2.setPopupDisplayEnabled(true, false, this);

    //smear
    addChildComponent(&fx3);
    fx3.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fx3.setRange(0.0, 1.0, 0.01);
    fx3.setSkewFactorFromMidPoint(0.25);
    fx3.setValue(0.2);
    fx3.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    fx3.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::pink);
    fx3.setColour(juce::Slider::thumbColourId, juce::Colours::transparentWhite);
    fx3.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fx3.setPopupDisplayEnabled(true, false, this);

    //contrast
    addChildComponent(&fx4);
    fx4.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fx4.setRange(0.0, 1.0, 0.01);
    fx4.setSkewFactor(0.5);
    fx4.setValue(0.2);
    fx4.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    fx4.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::green);
    fx4.setColour(juce::Slider::thumbColourId, juce::Colours::transparentWhite);
    fx4.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fx4.setPopupDisplayEnabled(true, false, this);

    fx1.setBounds(120, 150, getWidth() / 3, getHeight() / 3);
    fx2.setBounds(250, 150, getWidth() / 3, getHeight() / 3);
    fx3.setBounds(250, 150, getWidth() / 3, getHeight() / 3);
    fx4.setBounds(250, 150, getWidth() / 3, getHeight() / 3);
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

    scrambleButton.setBounds(216, 44, 94, 45);
    smearButton.setBounds(316, 44, 94, 45);
    contrastButton.setBounds(421, 44, 94, 45);
}

void SpectralSuiteAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &scrambleButton)
    {
        fxMode = FxMode::scramble;
        repaint();
    }
    else if (button == &smearButton)
    {
        fxMode = FxMode::smear;
        repaint();
    }
    else if (button == &contrastButton)
    {
        fxMode = FxMode::contrast;
        repaint();
    }
}

void SpectralSuiteAudioProcessorEditor::addAttachments()
{
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "inputGain", inputGain);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "outputGain", outputGain);

    pitchShiftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "pitchShift", pitchShift);
    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "dryWet", fx1);

    scramblingWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "scramblingWidth", fx2);
    smearingWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "smearingWidth", fx3);
    contrastValueAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "contrastValue", fx4);

    scrambleButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "scrambleEnabled", scrambleButton);
    smearButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "smearEnabled", smearButton);
    contrastButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.apvts, "contrastEnabled", contrastButton);

    scrambleButton.addListener(this);
    smearButton.addListener(this);
    contrastButton.addListener(this);
}