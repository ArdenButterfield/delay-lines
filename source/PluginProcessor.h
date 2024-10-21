#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "DelayGraph.h"
#include "Modulation/ModulationEngine.h"

#if (MSVC)
#include "ipps.h"
#endif

juce::AudioProcessorValueTreeState::ParameterLayout makeParameters();
std::vector<juce::String> makeModulationIds();

class PluginProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    DelayGraph delayGraph;
    juce::AudioProcessorValueTreeState parameters;

    ModulationEngine modulationEngine;

    juce::AudioProcessorValueTreeState& getValueTreeState();
private:


    bool parametersNeedUpdating;
    void updateParameters();
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix;
    void parameterChanged (const juce::String &parameterID, float newValue) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
