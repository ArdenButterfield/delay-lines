//
// Created by arden on 7/28/24.
//

#include "GraphPointComponent.h"
#include "PlaygroundComponent.h"

GraphPointComponent::GraphPointComponent (DelayGraph& _delayGraph, PlaygroundComponent* _playgroundComponent, int id)
    : delayGraph(_delayGraph), playgroundComponent(_playgroundComponent), identifier(id) {

}

void GraphPointComponent::paint (juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    auto point = delayGraph.getPoint(identifier);
    if (!point) {
        return;
    }
    if (point == delayGraph.activePoint) {
        if (delayGraph.interactionState == DelayGraph::outerSelected) {
            g.setColour(juce::Colours::blue);
            g.fillEllipse(point->x - 10, point->y - 10, 20, 20);
            g.setColour(juce::Colours::brown);

        } else if (delayGraph.interactionState == DelayGraph::movingPoint) {
            g.setColour(juce::Colours::black);
        } else {
            g.setColour(juce::Colours::orangered);
        }
    } else if (point->pointType == GraphPoint::start) {
        g.setColour(juce::Colours::green);
    } else if (point->pointType == GraphPoint::end)
    {
        g.setColour (juce::Colours::red);
    } else {
        g.setColour(juce::Colours::brown);
    }
    g.drawEllipse(point->x - 5, point->y - 5, 10, 10, 3);
    if (point->offset.getDistanceSquaredFromOrigin() > 0) {
        g.setColour(juce::Colours::magenta);
        g.drawEllipse(point->x + point->offset.x - 5, point->y + point->offset.y - 5, 10, 10, 3);
    }
}

void GraphPointComponent::resized() {

}

bool GraphPointComponent::hitTest (int x, int y) {
    auto point = delayGraph.getPoint(identifier);
    if (point != nullptr) {
        auto distance = point->getDistanceSquaredFrom({static_cast<float>(x), static_cast<float>(y)});
        return distance < 10 * 10;
    }
    return false;
}
void GraphPointComponent::mouseEnter (const juce::MouseEvent& event)
{
    updateFocus(event.position);
}

void GraphPointComponent::mouseExit (const juce::MouseEvent& event)
{
    auto point = delayGraph.getPoint(identifier);
    if (point == delayGraph.activePoint) {
        delayGraph.activePoint = nullptr;
    }
}

void GraphPointComponent::mouseMove (const juce::MouseEvent& event)
{
    updateFocus(event.position);
}

void GraphPointComponent::updateFocus (const juce::Point<float>& mousePoint)
{
    auto point = delayGraph.getPoint(identifier);
    if (point != nullptr) {
        delayGraph.activePoint = point;
        auto distanceSquared = point->getDistanceSquaredFrom(mousePoint);
        if (distanceSquared < innerHoverDistance * innerHoverDistance) {
            delayGraph.interactionState = DelayGraph::innerSelected;
        } else {
            delayGraph.interactionState = DelayGraph::outerSelected;
        }
    }
}
void GraphPointComponent::mouseDown (const juce::MouseEvent& event)
{
    if (delayGraph.interactionState == DelayGraph::outerSelected) {
        delayGraph.interactionState = DelayGraph::creatingLine;
        delayGraph.lineInProgressEnd = event.position;
        delayGraph.lineInProgressEndPoint = nullptr;
    } else if (delayGraph.interactionState == DelayGraph::innerSelected) {
        delayGraph.activePoint->draggingOffset = true;
        if (event.mods.isShiftDown()) {
            delayGraph.interactionState = DelayGraph::stretchingPoint;
        } else {
            delayGraph.interactionState = DelayGraph::movingPoint;
        }
    }
}
void GraphPointComponent::mouseDrag (const juce::MouseEvent& event)
{
    if (delayGraph.interactionState == DelayGraph::movingPoint || delayGraph.interactionState == DelayGraph::stretchingPoint) {
        delayGraph.activePoint->offset = event.position - juce::Point<float>(delayGraph.activePoint->x, delayGraph.activePoint->y);
    }
}
void GraphPointComponent::mouseUp (const juce::MouseEvent& event)
{
    if (delayGraph.interactionState == DelayGraph::movingPoint) {
        delayGraph.interactionState = DelayGraph::none;
        delayGraph.bakeOffsets();
    } else if (delayGraph.interactionState == DelayGraph::stretchingPoint) {
        delayGraph.interactionState = DelayGraph::none;
        delayGraph.activePoint->draggingOffset = false;
    }
}
void GraphPointComponent::mouseDoubleClick (const juce::MouseEvent& event)
{
    auto point = delayGraph.getPoint(identifier);
    delayGraph.deletePoint(point);
}
