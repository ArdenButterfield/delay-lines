//
// Created by arden on 7/28/24.
//

#include "GraphPointComponent.h"
#include "PlaygroundComponent.h"

GraphPointComponent::GraphPointComponent (ModulationMappingEngine& me, DelayGraph& _delayGraph, PlaygroundComponent* _playgroundComponent, int id)
    : delayGraph(_delayGraph),
      playgroundComponent(_playgroundComponent),
      identifier(id),
      mappingEngine(me),
      xMod(mappingEngine, {ModulatableKey::point, id, "x", {0, 1}}),
      yMod(mappingEngine, {ModulatableKey::point, id, "y", {0, 1}}) {
    addChildComponent(xMod);
    addChildComponent(yMod);
}

void GraphPointComponent::paint (juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    auto point = delayGraph.getPoint(identifier);
    if (!point) {
        return;
    }

    auto pointWithOffset = *point + playgroundComponent->getGlobalOffset();

    if (point == delayGraph.lineInProgressEndPoint) {
        g.setColour(juce::Colours::green);
        g.fillEllipse(pointWithOffset.x - 10, pointWithOffset.y - 10, 20, 20);

    }
    if (point == delayGraph.activePoint) {
        if (delayGraph.interactionState == DelayGraph::outerSelected) {
            g.setColour(juce::Colours::blue);
            g.fillEllipse(pointWithOffset.x - 10, pointWithOffset.y - 10, 20, 20);
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
    g.drawEllipse(pointWithOffset.x - 5, pointWithOffset.y - 5, 10, 10, 3);
    if (point->offset.getDistanceSquaredFromOrigin() > 0) {
        g.setColour(juce::Colours::magenta);
        g.drawEllipse(pointWithOffset.x + point->offset.x - 5, pointWithOffset.y + point->offset.y - 5, 10, 10, 3);
    }
    xMod.setBounds(pointWithOffset.x - 20, pointWithOffset.y - 5, 40, 10);
    yMod.setBounds(pointWithOffset.x - 5, pointWithOffset.y - 20, 10, 40);
}

void GraphPointComponent::resized() {
    xMod.modKey.range = {0, static_cast<float>(getWidth())};
    yMod.modKey.range = {0, static_cast<float>(getHeight())};
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
        auto pt = delayGraph.getPoint(identifier);
        if (pt == nullptr) {
            return;
        }

        delayGraph.interactionState = DelayGraph::creatingLine;
        delayGraph.lineInProgressEnd = event.position;
        delayGraph.lineInProgressEndPoint = nullptr;
        ghostComponent = std::make_unique<LineGhostComponent>(*pt, event.position);
        addAndMakeVisible(ghostComponent.get());
        ghostComponent->setBounds(getLocalBounds());
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
    } else if (delayGraph.interactionState == DelayGraph::creatingLine) {
        delayGraph.lineInProgressEnd = event.position;
        delayGraph.lineInProgressEndPoint = nullptr;
        for (const auto& point : delayGraph.getPoints()) {
            if  ((point.get() != delayGraph.activePoint) && (point->getDistanceSquaredFrom(delayGraph.lineInProgressEnd) < static_cast<float>(outerHoverDistance * outerHoverDistance))) {
                delayGraph.lineInProgressEndPoint = point.get();
                delayGraph.lineInProgressEnd = *point;
            }
        }
        ghostComponent->end = delayGraph.lineInProgressEnd;
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
    } else if (delayGraph.interactionState == DelayGraph::creatingLine) {
        removeChildComponent(ghostComponent.get());
        ghostComponent.reset();
        if (delayGraph.lineInProgressEndPoint) {
            delayGraph.addLine(delayGraph.activePoint, delayGraph.lineInProgressEndPoint);
        } else {
            delayGraph.addPoint(event.position, true);
        }
    } else if (event.mouseWasClicked() && event.mods.isRightButtonDown()) {
        auto point = delayGraph.getPoint(identifier);
        delayGraph.deletePoint(point);
    }
}

void GraphPointComponent::mouseDoubleClick (const juce::MouseEvent& event)
{
    auto point = delayGraph.getPoint(identifier);
    delayGraph.deletePoint(point);
}
