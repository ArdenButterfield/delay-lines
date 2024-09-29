//
// Created by arden on 9/29/24.
//

#include "GraphLineModule.h"

GraphLineModule::GraphLineModule(ModulationMappingEngine& me, DelayGraph& delayGraph, const int& line) {
    lineEditor = std::make_unique<LineEditor>(me, delayGraph, line);
    addAndMakeVisible(lineEditor.get());
}
GraphLineModule::~GraphLineModule()
{
}
void GraphLineModule::resized()
{
    lineEditor->setBounds(getLocalBounds()
            .withTrimmedTop(10)
            .withTrimmedRight(10)
            .withTrimmedLeft(10)
            .withTrimmedBottom(10));
}

void GraphLineModule::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::tan);
    g.setColour(juce::Colours::brown);
    g.drawRect(getBounds(), 2);
}
