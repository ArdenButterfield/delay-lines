//
// Created by arden on 5/16/24.
//

#include "ModKnobs.h"
#include "ModulationMappingEngine.h"
#include "ModulationEngine.h"

ModKnobs::ModKnobs()
{
}

ModKnobs::~ModKnobs()
{
}

void ModKnobs::setMappingEngine (ModulationMappingEngine* me)
{
    for (auto& knob : modKnobs) {
        removeChildComponent(knob.get());
    }

    mappingEngine = me;

    if (mappingEngine != nullptr) {
        int index = 0;
        for (const auto& id : mappingEngine->modulationEngine->paramIds) {
            modKnobs.push_back (std::make_unique<ModKnob> (mappingEngine, id, index));
            ++index;
        }
    }

    for (auto& modKnob : modKnobs) {
        addAndMakeVisible(*modKnob);
    }
}


void ModKnobs::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.fillAll();
}

void ModKnobs::resized()
{
    for (auto i = 0; i < modKnobs.size(); ++i) {
        auto area = getLocalBounds().withWidth(getWidth() / modKnobs.size()).withX(getWidth() * i / modKnobs.size());
        modKnobs[i]->setBounds(area);
    }
}
