//
// Created by arden on 5/6/24.
//

#ifndef DELAYLINES_MODULATIONENGINE_H
#define DELAYLINES_MODULATIONENGINE_H

#include "juce_audio_processors/juce_audio_processors.h"
#include "../DelayGraph.h"
#include "ModulatableKey.h"

class ModulationEngine : juce::AudioProcessorValueTreeState::Listener
{
public:
    ModulationEngine(juce::AudioProcessorValueTreeState& treeState, std::vector<juce::String> paramIds, DelayGraph& delayGraph);
    ~ModulationEngine() override;

    float getParameterValue(unsigned int index);
    void setParameterValue(unsigned int index, float value);
    void setMapping(unsigned int index, ModulatableKey key);
    void clearMapping(unsigned int index);
    bool isMapped(unsigned int index);
    void parameterChanged(const juce::String &parameterID, float newValue) override;
private:
    std::vector<std::unique_ptr<ModulatableKey>> mappings;
    juce::AudioProcessorValueTreeState& treeState;
    const std::vector<juce::String> paramIds;
    DelayGraph& delayGraph;

};

#endif //DELAYLINES_MODULATIONENGINE_H
