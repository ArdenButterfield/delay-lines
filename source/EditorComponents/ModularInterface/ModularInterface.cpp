//
// Created by arden on 9/28/24.
//

#include "ModularInterface.h"
#include "../../Modulation/ModulationMappingEngine.h"

ModularInterface::ModularInterface (ModulationMappingEngine& me, DelayGraph& dg) : mappingEngine(me), delayGraph(dg)
{
    boxOfLineEditors = std::make_unique<BoxOfLineEditors>(mappingEngine, delayGraph);
    addAndMakeVisible(lineEditorsViewport);
    lineEditorsViewport.setViewedComponent(boxOfLineEditors.get(), false);
}

ModularInterface::~ModularInterface()
{
}

void ModularInterface::resized()
{
    lineEditorsViewport.setBounds(getLocalBounds()
                                     .withTrimmedTop(10)
                                     .withTrimmedLeft(10)
                                     .withTrimmedRight(10)
                                     .withTrimmedBottom(10));
    boxOfLineEditors->minimumHeight = lineEditorsViewport.getHeight();
    boxOfLineEditors->setSize(lineEditorsViewport.getWidth() - lineEditorsViewport.getScrollBarThickness(), lineEditorsViewport.getHeight());
}

void ModularInterface::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::lightgrey.withMultipliedBrightness(1.2f));
    g.fillRect(lineEditorsViewport.getBounds()
                                .withWidth(lineEditorsViewport.getScrollBarThickness())
                                .withRightX(lineEditorsViewport.getRight());
}

void ModularInterface::timerCallback()
{
}
