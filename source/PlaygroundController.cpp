//
// Created by arden on 3/14/24.
//

#include "PlaygroundController.h"

void PlaygroundController::mouseMove (const juce::MouseEvent& event)
{
    setHoveredPoint(event.getPosition());
}

void PlaygroundController::mouseEnter (const juce::MouseEvent& event)
{}

void PlaygroundController::mouseExit (const juce::MouseEvent& event)
{}

void PlaygroundController::mouseDown (const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown()) {
        return;
    }
    if (event.mouseWasClicked() && delayGraph.interactionState == DelayGraph::editingLine) {
        delayGraph.interactionState = DelayGraph::none;
    }
    if (delayGraph.interactionState == DelayGraph::innerSelected) {
        delayGraph.interactionState = DelayGraph::movingPoint;
    } else if (delayGraph.interactionState == DelayGraph::outerSelected) {
        delayGraph.interactionState = DelayGraph::creatingLine;
        delayGraph.lineInProgressEnd = event.getPosition();
        delayGraph.lineInProgressEndPoint = nullptr;
    }
}

void PlaygroundController::mouseDrag (const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown()) {
        return;
    }

    if (delayGraph.interactionState == DelayGraph::movingPoint) {
        delayGraph.activePoint->setXY(event.getPosition().x, event.getPosition().y);
    } else if (delayGraph.interactionState == DelayGraph::creatingLine) {
        delayGraph.lineInProgressEnd = event.getPosition();
        delayGraph.lineInProgressEndPoint = nullptr;
        for (const auto& point : delayGraph.getPoints()) {
            if ((point.get() != delayGraph.activePoint) && (point->getDistanceSquaredFrom(delayGraph.lineInProgressEnd) < outerHoverDistance * outerHoverDistance)) {
                delayGraph.lineInProgressEndPoint = point.get();
            }
        }
    }
}

void PlaygroundController::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasClicked() && event.mods.isRightButtonDown()) {
        if (delayGraph.interactionState == DelayGraph::innerSelected) {
            delayGraph.deletePoint(delayGraph.activePoint);
        } else if (delayGraph.interactionState == DelayGraph::lineHover) {
            delayGraph.deleteLine(delayGraph.activeLine);
        }
    } else if (delayGraph.interactionState == DelayGraph::creatingLine) {
        if (delayGraph.lineInProgressEndPoint) {
            delayGraph.addLine(delayGraph.activePoint, delayGraph.lineInProgressEndPoint);
        } else {
            delayGraph.addPoint(InnerPoint(event.getPosition()), true);
        }
    } else if (event.mouseWasClicked() && delayGraph.interactionState == DelayGraph::none) {
        delayGraph.addPoint(InnerPoint(event.getPosition()));
    } else if (event.mouseWasClicked() && delayGraph.interactionState == DelayGraph::lineHover) {
        delayGraph.interactionState = DelayGraph::editingLine;
    }
    setHoveredPoint(event.getPosition());
}

void PlaygroundController::mouseDoubleClick (const juce::MouseEvent& event)
{
    if (delayGraph.interactionState == DelayGraph::innerSelected) {
        delayGraph.deletePoint(delayGraph.activePoint);
    }
}

void PlaygroundController::setHoveredPoint (const juce::Point<int>& mousePoint)
{
    if (delayGraph.interactionState == DelayGraph::editingLine) {
        return;
    }
    GraphPoint* closestPoint = nullptr;
    auto squareDistance = outerHoverDistance * outerHoverDistance;
    for (const auto& point : delayGraph.getPoints()) {
        auto dist = point->getDistanceSquaredFrom(mousePoint);
        if (dist < squareDistance) {
            squareDistance = dist;
            closestPoint = point.get();
        }
    }
    if (closestPoint) {
        delayGraph.activePoint = closestPoint;
        if (squareDistance < innerHoverDistance * innerHoverDistance) {
            delayGraph.interactionState = DelayGraph::innerSelected;
        } else {
            delayGraph.interactionState = DelayGraph::outerSelected;
        }
        return;
    }
    for (auto& line : delayGraph.getLines()) {
        auto l = juce::Line<int>(*line->start, *line->end);
        auto pointOnLine = juce::Point<int>();
        if (l.getDistanceFromPoint(mousePoint, pointOnLine) < lineHoverDistance) {
            delayGraph.activeLine = line.get();
            delayGraph.interactionState = DelayGraph::lineHover;
            return;
        }
    }
    delayGraph.activePoint = nullptr;
    delayGraph.interactionState = DelayGraph::none;
}

