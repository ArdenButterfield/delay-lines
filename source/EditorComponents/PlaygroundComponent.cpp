//
// Created by arden on 8/13/24.
//

#include "PlaygroundComponent.h"
#include "../DelayGraph.h"

PlaygroundComponent::PlaygroundComponent(DelayGraph& _delayGraph) : delayGraph(_delayGraph)
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
    auto newPoint = std::make_unique<GraphPointComponent>(delayGraph, this, identifier);
    addAndMakeVisible(newPoint.get());
    newPoint->setBounds(this->getLocalBounds());
    pointComponents.insert(std::move(newPoint));
}

void PlaygroundComponent::removePoint (int identifier)
{
    for (auto& p : pointComponents) {
        if (p->getIdentifier() == identifier) {
            removeChildComponent(p.get());
            pointComponents.erase(p);
        }
    }
}

void PlaygroundComponent::addLine (int identifier)
{


}

void PlaygroundComponent::removeLine (int identifier)
{

}
