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
    std::cout << lineEditorsViewport.getViewWidth() << "\n";
    boxOfLineEditors->setSize(lineEditorsViewport.getWidth() - lineEditorsViewport.getScrollBarThickness(), 1000);
}

void ModularInterface::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::pink);
}

void ModularInterface::timerCallback()
{
}
