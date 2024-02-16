
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>


//==============================================================================

SpectralSuiteAudioProcessor::SpectralSuiteAudioProcessor() :    hasPreparedToPlay(false),
                                                                apvts(*this, nullptr, "PARAMETERS", createParameterLayout()),
#ifndef JucePlugin_PreferredChannelConfigurations
      AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    
{
    apvts.addParameterListener("inputGain", this);
    apvts.addParameterListener("outputGain", this);

    apvts.addParameterListener("pitchShift", this);
    apvts.addParameterListener("dryWet", this);

    apvts.addParameterListener("scramblingWidth", this);
    apvts.addParameterListener("smearingWidth", this);
    apvts.addParameterListener("contrastValue", this);

    apvts.addParameterListener("scrambleEnabled", this);
    apvts.addParameterListener("smearEnabled", this);
    apvts.addParameterListener("contrastEnabled", this);
}

SpectralSuiteAudioProcessor::~SpectralSuiteAudioProcessor()
{
    apvts.removeParameterListener("inputGain", this);
    apvts.removeParameterListener("outputGain", this);

    apvts.removeParameterListener("pitchShift", this);
    apvts.removeParameterListener("dryWet", this);

    apvts.removeParameterListener("scramblingWidth", this);
    apvts.removeParameterListener("smearingWidth", this);
    apvts.removeParameterListener("contrastValue", this);

    apvts.removeParameterListener("scrambleEnabled", this);
    apvts.removeParameterListener("smearEnabled", this);
    apvts.removeParameterListener("contrastEnabled", this);
};

//==============================================================================
const juce::String SpectralSuiteAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpectralSuiteAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpectralSuiteAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpectralSuiteAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpectralSuiteAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpectralSuiteAudioProcessor::getNumPrograms()
{
    return 1;  
}

int SpectralSuiteAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpectralSuiteAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpectralSuiteAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpectralSuiteAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================



void SpectralSuiteAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    dryWet.prepare(spec);
    dryWet.setMixingRule(juce::dsp::DryWetMixingRule::balanced);
    dryWet.setWetMixProportion(0.5);

    if (apvts.getParameter("scrambleEnabled")->getValue() == 1.f)
    {
        fftEffect[0].setFxMode(FxMode::scramble);
        fftEffect[1].setFxMode(FxMode::scramble);
    }
    else if (apvts.getParameter("smearEnabled")->getValue() == 1.f)
    {
        fftEffect[0].setFxMode(FxMode::smear);
        fftEffect[1].setFxMode(FxMode::smear);
    }
    else if (apvts.getParameter("contrastEnabled")->getValue() == 1.f)
    {
        fftEffect[0].setFxMode(FxMode::contrast);
        fftEffect[1].setFxMode(FxMode::contrast);
    }
}   

void SpectralSuiteAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpectralSuiteAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

    void SpectralSuiteAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
    {
        juce::AudioBuffer<float> dryBuffer(buffer.getNumChannels(), buffer.getNumSamples());

        //input gain
        buffer.applyGain(Decibels::decibelsToGain(inputGain));

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            dryBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());

            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                double sample = buffer.getSample(channel, i);
                double output = fftEffect[channel].process(sample);
                buffer.setSample(channel, i, output);
            }
        }

        // dry wet
        dryWet.pushDrySamples(dryBuffer);
        dryWet.mixWetSamples(buffer);
        dryWet.setWetMixProportion(wetCoefficient);

        // output gain
        buffer.applyGain(Decibels::decibelsToGain(outputGain));
    }





//==============================================================================
bool SpectralSuiteAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SpectralSuiteAudioProcessor::createEditor()
{
    return new SpectralSuiteAudioProcessorEditor (*this);
}

//==============================================================================
void SpectralSuiteAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SpectralSuiteAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpectralSuiteAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SpectralSuiteAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("inputGain", "Input Gain", -20.0f, 10.f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("outputGain", "Output Gain", -20.0f, 10.f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitchShift", "Pitch Shift", -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("dryWet", "Dry/Wet", 0.0f, 1.0f, 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("scramblingWidth", "Scramble", 0.0f, 1.0f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("smearingWidth", "Smear", 0.0f, 1.0f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("contrastValue", "Contrast", 0.0f, 1.0f, 0.2f));

    params.push_back(std::make_unique<juce::AudioParameterBool>("scrambleEnabled", "Scramble On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("smearEnabled", "Smear On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("contrastEnabled", "Contrast On", true));

    return { params.begin(), params.end() };
}

void SpectralSuiteAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "inputGain")
    {
        inputGain = newValue;
    }

    if (parameterID == "outputGain")
    {
        outputGain = newValue;
    }

    if (parameterID == "dryWet")
    {
        wetCoefficient = newValue;
    }

    if (parameterID == "scramblingWidth")
    {
        fftEffect[0].setFxValue(newValue);
        fftEffect[1].setFxValue(newValue);
    }

    if (parameterID == "smearingWidth")
    {
        fftEffect[0].setFxValue(newValue);
        fftEffect[1].setFxValue(newValue);
    }

    if (parameterID == "contrastValue")
    {
        fftEffect[0].setFxValue(newValue);
        fftEffect[1].setFxValue(newValue);
    }

    if (parameterID == "scrambleEnabled" && newValue == 1.0f)
    {
        fftEffect[0].setFxMode(FxMode::scramble);
        fftEffect[1].setFxMode(FxMode::scramble);
    }

    if (parameterID == "smearEnabled" && newValue == 1.0f)
    {
        fftEffect[0].setFxMode(FxMode::smear);
        fftEffect[1].setFxMode(FxMode::smear);
    }

    if (parameterID == "contrastEnabled" && newValue == 1.0f)
    {
        fftEffect[0].setFxMode(FxMode::contrast);
        fftEffect[1].setFxMode(FxMode::contrast);
    }

    if (parameterID == "pitchShift")
    {
        fftEffect[0].setPitchShift(newValue);
        fftEffect[1].setPitchShift(newValue);
    }
}

void SpectralSuiteAudioProcessor::randomize() const
{;
    juce::Random random;
    int randomInt = random.nextInt();
    FxMode effect = static_cast<FxMode>(randomInt % 3);

    float effectValue = random.nextFloat();
    float pitchValue = random.nextFloat();

    std::string enabledParamName, valueParamName;

    switch (effect)
    {
    case FxMode::scramble:
        enabledParamName = "scrambleEnabled";
        valueParamName = "scramblingWidth";
        break;
    case FxMode::smear:
        enabledParamName = "smearEnabled";
        valueParamName = "smearingWidth";
        break;
    case FxMode::contrast:
        enabledParamName = "contrastEnabled";
        valueParamName = "contrastValue";
        break;
    }

    if (auto enabledParam = apvts.getParameter(enabledParamName))
    {
        enabledParam->beginChangeGesture();
        enabledParam->setValueNotifyingHost(1.0f);
        enabledParam->endChangeGesture();
    }

    if (auto valueParam = apvts.getParameter(valueParamName))
    {
        valueParam->beginChangeGesture();
        valueParam->setValueNotifyingHost(effectValue);
        valueParam->endChangeGesture();
    }

    if (auto pitchParam = apvts.getParameter("pitchShift"))
    {
        pitchParam->beginChangeGesture();
        pitchParam->setValueNotifyingHost(pitchValue);
        pitchParam->endChangeGesture();
    }


}
