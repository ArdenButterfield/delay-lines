//
// Created by arden on 5/6/24.
//

#include "ModulationEngine.h"

#include <utility>

ModulationEngine::ModulationEngine(juce::AudioProcessorValueTreeState& _treeState, std::vector<juce::String> _paramIds, DelayGraph& _delayGraph)
    : treeState(_treeState),
      paramIds(std::move(_paramIds)),
      delayGraph(_delayGraph)
{

    for (auto& paramId : paramIds) {
        treeState.addParameterListener(paramId, this);
    }
    mappings.resize(paramIds.size());

}

ModulationEngine::~ModulationEngine()
{

    for (auto& paramId : paramIds) {
        treeState.removeParameterListener(paramId, this);
    }
}

void ModulationEngine::parameterChanged(const juce::String &parameterID, float newValue)
{
    for (unsigned i = 0; i < paramIds.size(); ++i) {
        if (mappings[i] && paramIds[i] == parameterID) {
            delayGraph.modulateIfPossible(*mappings[i], newValue);
        }
    }
}

void ModulationEngine::setParameterValue (unsigned int index, float value)
{
    auto p = treeState.getParameter(paramIds[index]);

    p->beginChangeGesture();
    p->setValue(value);
    p->endChangeGesture();
}

float ModulationEngine::getParameterValue (unsigned int index)
{
    auto p = treeState.getParameter(paramIds[index]);
    return p->getValue();
}

void ModulationEngine::setMapping (unsigned int index, const ModulatableKey& key)
{
    mappings[index] = std::make_unique<ModulatableKey>(key);
}

void ModulationEngine::clearMapping (unsigned int index)
{
    mappings[index].reset(nullptr);
}

bool ModulationEngine::isMapped (unsigned int index)
{
    return mappings[index].operator bool();
}

