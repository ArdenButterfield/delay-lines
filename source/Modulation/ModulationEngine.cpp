//
// Created by arden on 5/6/24.
//

#include "ModulationEngine.h"

ModulationEngine::ModulationEngine(unsigned int numParameters, DelayGraph& dg) : delayGraph(dg)
{
    for (unsigned i = 0; i < numParameters; ++i) {
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("mod" + juce::String(i + 1), 1),
            "Mod " + juce::String(i + 1),
            0.f,
            1.f,
            0.f));
    }
    mappings.resize(numParameters);
}

ModulationEngine::~ModulationEngine()
{

}

std::vector<std::unique_ptr<juce::AudioParameterFloat>>& ModulationEngine::getParameters()
{
    return parameters;
}

void ModulationEngine::parameterValueChanged (int parameterIndex, float newValue)
{
    for (unsigned i = 0; i < parameters.size(); ++i) {
        if (mappings[i] && parameters[i]->getParameterIndex() == parameterIndex) {
            delayGraph.modulateIfPossible(*mappings[i], newValue);
            return;
        }
    }
}

void ModulationEngine::setParameterValue (unsigned int index, float value)
{
    parameters[index]->beginChangeGesture();
    *parameters[index] = value;
    parameters[index]->endChangeGesture();
}

float ModulationEngine::getParameterValue (unsigned int index)
{
    return parameters[index]->get();
}

void ModulationEngine::setMapping (unsigned int index, ModulatableKey key)
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

