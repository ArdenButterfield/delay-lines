//
// Created by arden on 7/28/24.
//

#include "GraphLineComponent.h"
#include "PlaygroundComponent.h"
#include "LineEditor.h"
#include "../DelayGraph.h"

static juce::AffineTransform makeTransform(juce::Point<float> start, juce::Point<float> end, int channel) {
    auto transform = juce::AffineTransform();
    transform = transform.scaled(start.getDistanceFrom(end), channel ? 50 : -50);
    transform = transform.rotated(start.getAngleToPoint(end) - juce::MathConstants<float>::halfPi);
    transform = transform.translated(start);
    return transform;
}


GraphLineComponent::GraphLineComponent (DelayGraph& _delayGraph, PlaygroundComponent* _playgroundComponent, int _id)
    : playgroundComponent(_playgroundComponent), delayGraph(_delayGraph), id(_id) {
}

GraphLineComponent::~GraphLineComponent()
{
}

void GraphLineComponent::paint (juce::Graphics& g)
{
    auto line = delayGraph.getLine(id);
    if (!line) {
        return;
    }
    if (delayGraph.interactionState == DelayGraph::lineHover && delayGraph.activeLine == line) {
        g.setColour(juce::Colours::yellow);
        g.drawLine(line->start->x, line->start->y, line->end->x, line->end->y, 10);
    } else if (delayGraph.interactionState == DelayGraph::editingLine && delayGraph.activeLine == line) {
        g.setColour(juce::Colours::pink);
        g.drawLine(line->start->x, line->start->y, line->end->x, line->end->y, 10);
    }

    if (line->parameters.isMuted()) {
        g.setColour(juce::Colours::brown.withAlpha(0.1f));
        g.drawLine(line->start->x, line->start->y, line->end->x, line->end->y, 3);
        return;
    }

    if (line->parameters.isBypassed()) {
        g.setColour(juce::Colours::brown.withAlpha(0.5f));
        g.drawLine(line->start->x, line->start->y, line->end->x, line->end->y, 3);
        return;
    }

    g.setColour(juce::Colours::brown);
    auto leftLinePath = juce::Path();
    auto rightLinePath = juce::Path();
    leftLinePath.startNewSubPath(0,0);
    rightLinePath.startNewSubPath(0,0);
    float l,r;
    auto numSteps = static_cast<int>(line->start->getDistanceFrom({line->end->x, line->start->x}));
    for (auto step = 0; step < numSteps; step += 1) {
        auto proportion = static_cast<float>(step) / static_cast<float>(numSteps);
        line->getEnvelope(proportion, l, r);
        leftLinePath.lineTo(proportion, l);
        rightLinePath.lineTo(proportion, r);
    }
    leftLinePath.lineTo(1,0);
    rightLinePath.lineTo(1,0);
    leftLinePath.closeSubPath();
    rightLinePath.closeSubPath();

    g.fillPath(leftLinePath, makeTransform(*line->start + line->start->offset, *line->end + line->end->offset, 0));
    g.fillPath(rightLinePath, makeTransform(*line->start + line->start->offset, *line->end + line->end->offset, 1));
    g.drawLine(line->start->x, line->start->y, line->end->x, line->end->y, 3);
}

void GraphLineComponent::resized()
{
    Component::resized();
}

bool GraphLineComponent::hitTest (int x, int y)
{
    if (lineEditor && lineEditor->isVisible()) {
        return true;
    }
    auto line = delayGraph.getLine(id);
    if (line == nullptr) {
        return false;
    }
    auto l = juce::Line<float>(*line->start, *line->end);
    auto pointOnLine = juce::Point<float>();
    return (l.getDistanceFromPoint({static_cast<float>(x), static_cast<float>(y)}, pointOnLine)
            < static_cast<float>(lineHoverDistance));
}

void GraphLineComponent::mouseEnter (const juce::MouseEvent& event)
{
    delayGraph.activeLine = delayGraph.getLine(id);
    delayGraph.interactionState = DelayGraph::lineHover;
}

void GraphLineComponent::mouseExit (const juce::MouseEvent& event)
{
    delayGraph.interactionState = DelayGraph::none;
}

void GraphLineComponent::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasClicked() && lineEditor && lineEditor->isVisible()) {
        playgroundComponent->removeChildComponent(lineEditor.get());
        lineEditor.reset();
    } else if (event.mouseWasClicked() && event.mods.isShiftDown()) {
        delayGraph.activeLine->toggleEnabled();
    } else if (event.mouseWasClicked() && event.mods.isRightButtonDown()) {
        delayGraph.deleteLine(delayGraph.activeLine);
        delayGraph.interactionState = DelayGraph::none;
    } else if (event.mouseWasClicked()) {
        lineEditor = std::make_unique<LineEditor>(delayGraph, id);
        playgroundComponent->addAndMakeVisible(*lineEditor);
        lineEditor->setBounds(10,10,350,250);
    }
}