//
// Created by arden on 3/14/24.
//

#include "PlaygroundController.h"

void PlaygroundController::mouseMove (const juce::MouseEvent& event)
{
    setHoveredPoint(event.position);
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
    switch (delayGraph.interactionState) {
        case DelayGraph::innerSelected:
            if (event.mods.isShiftDown()) {
                delayGraph.interactionState = DelayGraph::stretchingPoint;
                delayGraph.activePoint->draggingOffset = true;
            } else {
                delayGraph.interactionState = DelayGraph::movingPoint;
            }
            break;
        case DelayGraph::outerSelected:
            if (event.mods.isShiftDown())
            {
                delayGraph.interactionState = DelayGraph::stretchingPoint;
                delayGraph.activePoint->draggingOffset = true;
            } else {
                delayGraph.interactionState = DelayGraph::creatingLine;
                delayGraph.lineInProgressEnd = event.position;
                delayGraph.lineInProgressEndPoint = nullptr;
            }
            break;
        default: break;
    }
}

void PlaygroundController::mouseDrag (const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown()) {
        return;
    }

    switch (delayGraph.interactionState) {
        case DelayGraph::movingPoint:
            delayGraph.activePoint->setXY(event.position.x, event.position.y);
            break;
        case DelayGraph::creatingLine:
            delayGraph.lineInProgressEnd = event.position;
            delayGraph.lineInProgressEndPoint = nullptr;
            for (const auto& point : delayGraph.getPoints()) {
                if  ((point.get() != delayGraph.activePoint) && (point->getDistanceSquaredFrom(delayGraph.lineInProgressEnd) < outerHoverDistance * outerHoverDistance)) {
                    delayGraph.lineInProgressEndPoint = point.get();
                }
            }
            break;
        case DelayGraph::stretchingPoint:
            delayGraph.activePoint->offset = event.position - *delayGraph.activePoint;

    }

    if (delayGraph.interactionState == DelayGraph::movingPoint) {
        delayGraph.activePoint->setXY(event.position.x, event.position.y);
    } else if (delayGraph.interactionState == DelayGraph::creatingLine) {
        delayGraph.lineInProgressEnd = event.position;
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
    switch (delayGraph.interactionState) {
        case DelayGraph::none:
            if (event.mouseWasClicked()) {
                delayGraph.addPoint(InnerPoint(event.position));
            }
            break;
        case DelayGraph::innerSelected:
            if (event.mouseWasClicked() && event.mods.isRightButtonDown()) {
                delayGraph.deletePoint(delayGraph.activePoint);
            }
            break;
        case DelayGraph::lineHover:
            if (event.mouseWasClicked() && event.mods.isRightButtonDown()) {
                delayGraph.activeLine->toggleEnabled();
            } else if (event.mouseWasClicked()) {
                delayGraph.interactionState = DelayGraph::editingLine;
            }
            break;
        case DelayGraph::creatingLine:
            if (delayGraph.lineInProgressEndPoint) {
                delayGraph.addLine(delayGraph.activePoint, delayGraph.lineInProgressEndPoint);
            } else {
                delayGraph.addPoint(InnerPoint(event.position), true);
            }
            break;
        case DelayGraph::editingLine:
            if (event.mouseWasClicked()) {
                delayGraph.interactionState = DelayGraph::none;
            }
            break;
        case DelayGraph::stretchingPoint:
            delayGraph.interactionState = DelayGraph::none;
            delayGraph.activePoint->draggingOffset = false;
            break;
        default: break;
    }

    setHoveredPoint(event.position);
}

void PlaygroundController::mouseDoubleClick (const juce::MouseEvent& event)
{
    switch (delayGraph.interactionState) {
        case DelayGraph::innerSelected:
            if (delayGraph.activePoint->pointType == GraphPoint::inner) {
                delayGraph.deletePoint(delayGraph.activePoint);
            }
            break;
        case DelayGraph::lineHover:
        case DelayGraph::editingLine:
            delayGraph.deleteLine(delayGraph.activeLine);
            break;
        default: break;
    }
}

void PlaygroundController::setHoveredPoint (const juce::Point<float>& mousePoint)
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
        auto l = juce::Line<float>(*line->start, *line->end);
        auto pointOnLine = juce::Point<float>();
        if (l.getDistanceFromPoint(mousePoint, pointOnLine) < lineHoverDistance) {
            delayGraph.activeLine = line.get();
            delayGraph.interactionState = DelayGraph::lineHover;
            return;
        }
    }
    delayGraph.activePoint = nullptr;
    delayGraph.interactionState = DelayGraph::none;
}

