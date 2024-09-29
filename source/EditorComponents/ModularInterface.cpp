//
// Created by arden on 9/28/24.
//

#include "ModularInterface.h"
#include "../Modulation/ModulationMappingEngine.h"

ModularInterface::ModularInterface (ModulationMappingEngine& me, DelayGraph& dg) : mappingEngine(me), delayGraph(dg)
{
    for (auto& lineID : delayGraph.getAllLineIds()) {
        lineModules.push_back(std::make_unique<GraphLineModule>(mappingEngine, delayGraph, lineID));
    }
    for (auto& module : lineModules) {
        addAndMakeVisible(module.get());
    }
    delayGraph.addListener(this);
}

ModularInterface::~ModularInterface()
{
}

void ModularInterface::resized()
{
    juce::FlexBox fb;
    fb.flexWrap = juce::FlexBox::Wrap::wrap;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    for (auto& module : lineModules) {
        fb.items.add(juce::FlexItem(*module).withMinWidth(200).withMinHeight(200));
    }
    fb.performLayout(getLocalBounds());
}

void ModularInterface::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::pink);
}
void ModularInterface::addPoint (int identifier)
{
}

void ModularInterface::removePoint (int identifier)
{
}

void ModularInterface::addLine (int identifier)
{
    lineModules.push_back(std::make_unique<GraphLineModule>(mappingEngine, delayGraph, identifier));
    addAndMakeVisible(lineModules.back().get());
    resized();
}

void ModularInterface::removeLine (int identifier)
{
}

void ModularInterface::timerCallback()
{
}
