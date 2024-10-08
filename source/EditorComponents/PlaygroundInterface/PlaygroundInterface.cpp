//
// Created by arden on 8/13/24.
//

#include "PlaygroundInterface.h"
#include "../../DelayGraph.h"

PlaygroundInterface::PlaygroundInterface (ModulationMappingEngine& me, DelayGraph& _delayGraph) :  globalOffset(0,0), delayGraph(_delayGraph), mappingEngine(me)
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
PlaygroundInterface::~PlaygroundInterface()
{
    delayGraph.removeListener(this);
}

void PlaygroundInterface::resized()
{
    for (auto& point : pointComponents) {
        point->setBounds(getLocalBounds());
    }

    for (auto& line : lineComponents) {
        line->setBounds(getLocalBounds());
    }
}

void PlaygroundInterface::paint (juce::Graphics& g)
{
    paintBackground(g);
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void PlaygroundInterface::addPoint (int identifier)
{
    auto newPoint = std::make_unique<GraphPointComponent>(mappingEngine, delayGraph, this, identifier);
    addAndMakeVisible(newPoint.get(), -1);
    newPoint->setBounds(this->getLocalBounds());
    pointComponents.insert(std::move(newPoint));
}

void PlaygroundInterface::removePoint (int identifier)
{
    std::erase_if(pointComponents, [&identifier](auto& p) { return identifier == p->getIdentifier();});
}

void PlaygroundInterface::addLine (int identifier)
{
    auto newLine = std::make_unique<GraphLineComponent>(mappingEngine, delayGraph, this, identifier);
    addAndMakeVisible(newLine.get(), 0); // Lines should be under points
    newLine->setBounds(this->getLocalBounds());
    lineComponents.insert(std::move(newLine));

}

void PlaygroundInterface::removeLine (int identifier)
{
    std::erase_if(lineComponents, [&identifier](auto& l) { return identifier == l->getIdentifier();});
}

void PlaygroundInterface::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasClicked()) {
        delayGraph.addPoint(event.position, false);
    } else {
        delayGraph.applyGlobalOffset(globalOffset);
        auto gx = std::fmod(globalOffset.x, gridSize);
        auto gy = std::fmod(globalOffset.y, gridSize);
        gridOffset = {gx < 0 ? gx + gridSize : gx, gy < 0 ? gy + gridSize : gy};
        globalOffset = {0,0};
    }
}

void PlaygroundInterface::mouseDrag (const juce::MouseEvent& event)
{
    globalOffset = {
        static_cast<float>(event.getOffsetFromDragStart().x),
        static_cast<float>(event.getOffsetFromDragStart().y)
    };
}
void PlaygroundInterface::paintBackground (juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.fillAll();
    g.setColour(juce::Colours::lightblue.withAlpha(0.2f));
    for (auto y = gridOffset.y + std::fmod(globalOffset.y, gridSize) - gridSize; y < getHeight() + gridSize; y += gridSize) {
        g.drawHorizontalLine(y, 0, getWidth());
    }
    for (auto x = gridOffset.x + std::fmod(globalOffset.x, gridSize) - gridSize; x < getWidth() + gridSize; x += gridSize) {
        g.drawVerticalLine(x, 0, getHeight());
    }
}
