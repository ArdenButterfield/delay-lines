//
// Created by arden on 8/13/24.
//

#include "PlaygroundComponent.h"
#include "../DelayGraph.h"

PlaygroundComponent::PlaygroundComponent(ModulationMappingEngine& me, DelayGraph& _delayGraph) :  globalOffset(0,0), delayGraph(_delayGraph), mappingEngine(me)
{
    delayGraph.addListener(this);
    for (auto id : delayGraph.getAllLineIds()) {
        addLine(id);
    }
    for (auto id : delayGraph.getAllPointIds()) {
        addPoint(id);
    }

    startTimerHz(60);
}
PlaygroundComponent::~PlaygroundComponent()
{
    delayGraph.removeListener(this);
}

void PlaygroundComponent::resized()
{
    for (auto& point : pointComponents) {
        point->setBounds(getLocalBounds());
    }

    for (auto& line : lineComponents) {
        line->setBounds(getLocalBounds());
    }
}

void PlaygroundComponent::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.fillAll();
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void PlaygroundComponent::addPoint (int identifier)
{
    auto newPoint = std::make_unique<GraphPointComponent>(mappingEngine, delayGraph, this, identifier);
    addAndMakeVisible(newPoint.get(), -1);
    newPoint->setBounds(this->getLocalBounds());
    pointComponents.insert(std::move(newPoint));
}

void PlaygroundComponent::removePoint (int identifier)
{
    std::erase_if(pointComponents, [&identifier](auto& p) { return identifier == p->getIdentifier();});
}

void PlaygroundComponent::addLine (int identifier)
{
    auto newLine = std::make_unique<GraphLineComponent>(mappingEngine, delayGraph, this, identifier);
    addAndMakeVisible(newLine.get(), 0); // Lines should be under points
    newLine->setBounds(this->getLocalBounds());
    lineComponents.insert(std::move(newLine));

}

void PlaygroundComponent::removeLine (int identifier)
{
    std::erase_if(lineComponents, [&identifier](auto& l) { return identifier == l->getIdentifier();});
}

void PlaygroundComponent::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasClicked()) {
        delayGraph.addPoint(event.position, false);
    } else {
        delayGraph.applyGlobalOffset(globalOffset);
        globalOffset = {0.f, 0.f};
    }
}

void PlaygroundComponent::mouseDrag (const juce::MouseEvent& event)
{
    globalOffset = {
        static_cast<float>(event.getOffsetFromDragStart().x),
        static_cast<float>(event.getOffsetFromDragStart().y)};
}