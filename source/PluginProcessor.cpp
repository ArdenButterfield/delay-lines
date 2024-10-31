#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "parameters.h"

juce::AudioProcessorValueTreeState::ParameterLayout makeParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID {MIX_PARAM_ID, 1},
        "mix",
        0.f, 100.f, 50.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID {STRETCH_TIME_ID, 1},
        "stretch time",
        0.f, 2.f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID {CLEAR_PARAM_ID, 1},
        "clear lines",
        0.f, 1.f, 0.f));
    for (unsigned i = 0; i < NUM_MOD_PARAMETERS; ++i) {
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ makeModParamId(i), 1},
            makeModParamName(i),
            0.f, 1.f, 0.f
            ));
    }
    return {parameters.begin(), parameters.end()};
}

std::vector<juce::String> makeModulationIds() {
    auto v = std::vector<juce::String>();

    for (unsigned i = 0; i < NUM_MOD_PARAMETERS; ++i) {
        v.push_back(makeModParamId(i));
    }
    return v;
}

PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
      parameters(*this, nullptr, juce::Identifier("DELAY-LINE-PARAMETERS"), makeParameters()),
        modulationEngine(parameters, makeModulationIds(), delayGraph)
{
    parameters.addParameterListener(MIX_PARAM_ID, this);
    parameters.addParameterListener(STRETCH_TIME_ID, this);
    parameters.addParameterListener(CLEAR_PARAM_ID, this);
    parametersNeedUpdating = true;

}

PluginProcessor::~PluginProcessor() = default;

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec {
        sampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(getTotalNumInputChannels())};
    delayGraph.prepareToPlay(spec);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void PluginProcessor::updateParameters()
{
    parametersNeedUpdating = false;
    mix.setCurrentAndTargetValue(((juce::AudioParameterFloat*)parameters.getParameter(MIX_PARAM_ID))->get() * 0.01);
    delayGraph.setStretchTime(((juce::AudioParameterFloat*)parameters.getParameter(STRETCH_TIME_ID))->get() * getSampleRate());
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    if (parametersNeedUpdating) {
        updateParameters();
    }

    auto pitch = -1;
    for (const auto& metadata : midiMessages) {
        auto message = metadata.getMessage();
        if (message.isNoteOn()) {
            pitch = message.getNoteNumber();
        }
    }
    if (pitch >= 0) {
        delayGraph.setMidiTrackNote(pitch);
    }

    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    auto sample = std::vector<float>();
    sample.resize(static_cast<unsigned long> (getTotalNumInputChannels()));
    for (int s = 0; s < buffer.getNumSamples(); ++s) {
        auto wetGain = mix.getNextValue();
        for (int channel = 0; channel < getTotalNumInputChannels(); ++channel) {
            sample[static_cast<unsigned>(channel)] = buffer.getSample(channel, s);
        }
        delayGraph.processSample(sample);
        for (int channel = 0; channel < getTotalNumInputChannels(); ++channel) {
            auto dry = buffer.getSample(channel, s) * (1 - wetGain);
            auto wet = sample[static_cast<unsigned>(channel)] * wetGain;
            buffer.setSample(channel, s, dry + wet);
        }
    }
    if (auto bpmFromHost = *getPlayHead()->getPosition()->getBpm()) {
        delayGraph.setBpm(bpmFromHost);
    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto xml = juce::XmlElement("plugin-state");
    auto parametersState = parameters.copyState().createXml();
    auto apvts = xml.createTextElement("apvts");
    xml.setAttribute("apvts", parametersState->toString());
    delayGraph.exportToXml(&xml);
    modulationEngine.exportToXml(&xml);
    copyXmlToBinary(xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto xml = getXmlFromBinary(data, sizeInBytes);
    parameters.replaceState(juce::ValueTree::fromXml(xml->getStringAttribute("apvts")));
    modulationEngine.importFromXml(xml.get());
    delayGraph.importFromXml(xml.get());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

juce::AudioProcessorValueTreeState& PluginProcessor::getValueTreeState()
{
    return parameters;
}

void PluginProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == MIX_PARAM_ID || parameterID == STRETCH_TIME_ID) {
        parametersNeedUpdating = true;
    } else if (parameterID == CLEAR_PARAM_ID && newValue > 0.5) {
        delayGraph.clear();
    }
}
