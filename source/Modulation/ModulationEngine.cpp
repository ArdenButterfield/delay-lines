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
    startTimerHz(60);
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
    const juce::ScopedLock scopedLock(parameterChangeLock);
//    p->beginChangeGesture();
    p->setValueNotifyingHost(value);
//    p->endChangeGesture();
}

void ModulationEngine::setParameterValue (const ModulatableKey& modKey, float value)
{
    for (auto index = 0; index < paramIds.size(); ++index) {
        if (mappings[index] && mappings[index]->getName() == modKey.getName()) {
            setParameterValue(index, value);
        }
    }
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
    if (index >= mappings.size()) {
        return false;
    }
    return mappings[index].operator bool();
}

void ModulationEngine::timerCallback()
{
    // setParametersToInternalState();
}

void ModulationEngine::setParametersToInternalState()
{
    for (unsigned i = 0; i < paramIds.size(); ++i) {
        if (mappings[i]) {
            float val;
            if (delayGraph.getModulationValue(*mappings[i], val)) {
                setParameterValue(i, val);
            }
        }
    }
}

juce::String ModulationEngine::getNameOfMapping(unsigned int index)
{
    if (index < mappings.size() && mappings[index]) {
        return mappings[index]->getName();
    }
}
juce::Colour ModulationEngine::getColourForMapping(unsigned int index)
{
    if (mappings[index]) {
        if (mappings[index]->type == ModulatableKey::Type::line) {
            auto line = delayGraph.getLine(mappings[index]->pointOrLineId);
            if (line) {
                return line->getColor();
            }
        } else {
            return juce::Colours::lightblue; // TODO: do something with points
        }
    }
    return juce::Colours::black.withAlpha(0.f);
}

void ModulationEngine::exportToXml (juce::XmlElement* parent)
{
    auto element = parent->createNewChildElement("mod-mapping");
    for (unsigned i = 0; i < mappings.size(); ++i) {
        if (mappings[i]) {
            auto mappingElement = element->createNewChildElement("mod-" + juce::String(i));
            mappings[i]->exportToXml(mappingElement);
        }
    }
}
void ModulationEngine::importFromXml (juce::XmlElement* parent)
{
    auto element = parent->getChildByName("mod-mapping");
    if (element == nullptr) {
        return;
    }

    for (unsigned i = 0; i < mappings.size(); ++i) {
        auto mappingElement = element->getChildByName("mod-" + juce::String(i));
        if (mappingElement != nullptr) {
            if (!isMapped(i)) {
                setMapping(i, ModulatableKey());
            }
            mappings[i]->importFromXml(mappingElement);
        } else {
            clearMapping(i);
        }
    }
}
