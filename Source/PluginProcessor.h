/*
  ==============================================================================

    Spectral Suite by Paxton Fleming (Ya Boy Pax)

  ==============================================================================
*/

#pragma once



#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "FFTEffect.h"

#include <string>
#include <fstream>
#include <sstream>

class SpectralSuiteAudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    SpectralSuiteAudioProcessor();
    ~SpectralSuiteAudioProcessor() override;

    juce::AudioProcessorValueTreeState apvts;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void randomize() const;

private:
    //==============================================================================
    unsigned int frameLength=1024;
    
    FFTEffect fftEffect[2];

    juce::CriticalSection fftDataCriticalSection;

    float inputGain = 1.0f;
    float outputGain = 1.0f;
    float wetCoefficient = 1.0f;

    float scramblingWidth = 0.2f;
    float smearingWidth = 0.2f;
    float contrastValue = 0.2f;

    juce::dsp::DryWetMixer<float> dryWet;
    juce::dsp::ProcessSpec spec;

    bool hasPreparedToPlay;
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralSuiteAudioProcessor)
};