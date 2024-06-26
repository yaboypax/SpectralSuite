

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/SSLookAndFeel.h"
#include "GUI/RandomIcon.h"


class SpectralSuiteAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Button::Listener, public juce::Slider::Listener
{ 
public:
    SpectralSuiteAudioProcessorEditor (SpectralSuiteAudioProcessor&);
    ~SpectralSuiteAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void displayText(juce::Graphics& g);

    void layoutGainSliders();
    void layoutFxSliders();
    void layoutButtons();
    
    void buttonClicked(juce::Button* button) override;

    void sliderValueChanged(juce::Slider* slider) override;
    void sliderDragEnded(juce::Slider* slider) override;

    void addAttachments();
    void loadFxMode();
    void updateValues();
    

private:
    SpectralSuiteAudioProcessor& processor;
    SSLookAndFeel ssLookAndFeel;
    
    FxMode fxMode;

    juce::Slider inputGain;
    juce::Slider outputGain;

    juce::Slider pitchShift;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchShiftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> scramblingWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> smearingWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> contrastValueAttachment;

    juce::Slider fx1, fx2, fx3, fx4;
    juce::String dryWetText, effectText, pitchText, inText, outText;

    juce::TextButton smearButton, scrambleButton, contrastButton;
    RandomIcon randomizeButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> scrambleButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> smearButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> contrastButtonAttachment;

    const juce::Colour subtleBlack = juce::Colour(0xFF404040);
    const juce::Colour contrastColor = juce::Colour(0xFF88928A);
    const juce::Colour smearColor = juce::Colour(0xFFB496BC);
    const juce::Colour scrambleColor = juce::Colour(0xFFF4F4D0);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectralSuiteAudioProcessorEditor)
};
