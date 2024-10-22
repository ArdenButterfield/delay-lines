//
// Created by arden on 5/6/24.
//

#ifndef DELAYLINES_MODULATIONENGINE_H
#define DELAYLINES_MODULATIONENGINE_H

#include "juce_audio_processors/juce_audio_processors.h"
#include "../DelayGraph.h"
#include "ModulatableKey.h"

class DelayGraph;

class ModulationEngine : juce::AudioProcessorValueTreeState::Listener, juce::Timer
{
public:
    ModulationEngine(juce::AudioProcessorValueTreeState& treeState, std::vector<juce::String> paramIds, DelayGraph& delayGraph);
    ~ModulationEngine() override;

    float getParameterValue(unsigned int index);
    juce::String getNameOfMapping(unsigned int index);
    void setParameterValue(unsigned int index, float value);
    void setMapping(unsigned int index, const ModulatableKey& key);
    void clearMapping(unsigned int index);
    bool isMapped(unsigned int index);
    void parameterChanged(const juce::String &parameterID, float newValue) override;
    juce::Colour getColourForMapping(unsigned int index);
    const std::vector<juce::String> paramIds;
    juce::AudioProcessorValueTreeState& treeState;
    DelayGraph& delayGraph;
private:
    std::vector<std::unique_ptr<ModulatableKey>> mappings;
    void timerCallback() override;
    void setParametersToInternalState();

    juce::CriticalSection parameterChangeLock;
};

#endif //DELAYLINES_MODULATIONENGINE_H
