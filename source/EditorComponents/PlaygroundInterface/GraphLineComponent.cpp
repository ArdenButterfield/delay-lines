//
// Created by arden on 7/28/24.
//

#include "GraphLineComponent.h"
#include "../../DelayGraph.h"
#include "../LineEditor/LineEditor.h"
#include "PlaygroundInterface.h"

static juce::AffineTransform makeTransform(juce::Point<float> start, juce::Point<float> end, int channel) {
    auto transform = juce::AffineTransform();
    transform = transform.scaled(start.getDistanceFrom(end), channel ? 50 : -50);
    transform = transform.rotated(start.getAngleToPoint(end) - juce::MathConstants<float>::halfPi);
    transform = transform.translated(start);
    return transform;
}


GraphLineComponent::GraphLineComponent (ModulationMappingEngine& me, DelayGraph& _delayGraph, PlaygroundInterface* _playgroundInterface, int _id)
    : playgroundInterface(_playgroundInterface), delayGraph(_delayGraph), id(_id), mappingEngine(me) {
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

    auto linesSharingSpace = delayGraph.getAllLinesBetweenPoints(line->start, line->end);
    bool isHovered = false;
    auto startPoint = *(line->start) + playgroundInterface->getGlobalOffset();
    auto endPoint = *(line->end) + playgroundInterface->getGlobalOffset();
    if (delayGraph.interactionState == DelayGraph::lineHover && delayGraph.activeLine == line) {
        isHovered = true;
        g.setColour(juce::Colours::yellow);
        g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 10);
    } else if (delayGraph.interactionState == DelayGraph::editingLine && delayGraph.activeLine == line) {
        g.setColour(juce::Colours::pink);
        g.drawLine(line->start->x, startPoint.y, endPoint.x, endPoint.y, 10);
    }

    if (line->parameters.isMuted()) {
        g.setColour(juce::Colours::brown.withAlpha(0.1f));
        g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 3);
        return;
    }

    if (line->parameters.isBypassed()) {
        g.setColour(juce::Colours::brown.withAlpha(0.5f));
        g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 3);
        return;
    }

    g.setColour(line->getColor());

    if (isHovered) {
        g.setColour(line->getColor().withMultipliedLightness(1.4));
    }

    auto leftLinePath = juce::Path();
    auto rightLinePath = juce::Path();
    leftLinePath.startNewSubPath(0,0);
    rightLinePath.startNewSubPath(0,0);
    float l,r;
    auto numSteps = static_cast<int>(startPoint.getDistanceFrom({endPoint.x, startPoint.x}));
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

    if (linesSharingSpace.size() == 1) {
        g.fillPath(leftLinePath, makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 0));
        g.fillPath(rightLinePath, makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 1));
    } else if (linesSharingSpace.size() == 2) {
        if ((id == linesSharingSpace[0]) == line->isGoingBackwards()) {
            g.fillPath(leftLinePath, makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 0));
        } else {
            g.fillPath(rightLinePath, makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 1));
        }
    }
    g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 3);
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

    if (juce::approximatelyEqual(line->start->getDistanceSquaredFrom(*line->end), 0.f)) {
        return false;
    }

    auto linesSharingSpace = delayGraph.getAllLinesBetweenPoints(line->start, line->end);
    auto startPoint = *(line->start) + playgroundInterface->getGlobalOffset() + line->start->offset;
    auto endPoint = *(line->end) + playgroundInterface->getGlobalOffset() + line->end->offset;
    auto backwards = line->isGoingBackwards();
    auto reverseTransform = makeTransform(backwards ? endPoint : startPoint, backwards ? startPoint : endPoint, 0).inverted();

    auto transx = static_cast<float>(x);
    auto transy = static_cast<float>(y);
    reverseTransform.transformPoint(transx, transy);
    if (0 < transx && transx < 1) {
        auto above = false;
        auto below = false;
        if (0 <= transy && transy < 1) {
            above = true;
        } else if (-1 < transy && transy <= 0) {
            below = true;
        }
        auto linesSharingSpace = delayGraph.getAllLinesBetweenPoints(line->start, line->end);
        if (linesSharingSpace.size() < 2) {
            return above || below;
        } else if (linesSharingSpace.size() == 2) {
            auto shouldBeAbove = (id == linesSharingSpace[0]) == line->isGoingBackwards();
            return (above && shouldBeAbove) || (below && !shouldBeAbove);
        }
    }
    return false;
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
        playgroundInterface->removeChildComponent(lineEditor.get());
        lineEditor.reset();
    } else if (event.mouseWasClicked() && event.mods.isShiftDown()) {
        delayGraph.activeLine->toggleEnabled();
    } else if (event.mouseWasClicked() && event.mods.isRightButtonDown()) {
        delayGraph.interactionState = DelayGraph::none;
        delayGraph.deleteLine(delayGraph.activeLine);
    } else if (event.mouseWasClicked()) {
        lineEditor = std::make_unique<LineEditor>(mappingEngine, delayGraph, id);
        playgroundInterface->addAndMakeVisible(*lineEditor);
        lineEditor->setBounds(LineEditor::getDesiredBounds());
    }
}
