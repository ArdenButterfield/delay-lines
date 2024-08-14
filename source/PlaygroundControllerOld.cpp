//
// Created by arden on 3/14/24.
//

#include "PlaygroundControllerOld.h"

void PlaygroundControllerOld::mouseMove (const juce::MouseEvent& event)
{
    setHoveredPoint(event.position);
}

void PlaygroundControllerOld::mouseEnter (const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
}

void PlaygroundControllerOld::mouseExit (const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
}

void PlaygroundControllerOld::mouseDown (const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown()) {
        return;
    }
    switch (delayGraph.interactionState) {
        case DelayGraph::innerSelected:

            delayGraph.activePoint->draggingOffset = true;
            if (event.mods.isShiftDown()) {
                delayGraph.interactionState = DelayGraph::stretchingPoint;
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
        case DelayGraph::none:
        case DelayGraph::stretchingPoint:
        case DelayGraph::movingPoint:
        case DelayGraph::creatingLine:
        case DelayGraph::editingLine:
        case DelayGraph::lineHover:
        default: break;
    }
}

void PlaygroundControllerOld::mouseDrag (const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown()) {
        return;
    }

    switch (delayGraph.interactionState) {
        case DelayGraph::creatingLine:
            delayGraph.lineInProgressEnd = event.position;
            delayGraph.lineInProgressEndPoint = nullptr;
            for (const auto& point : delayGraph.getPoints()) {
                if  ((point.get() != delayGraph.activePoint) && (point->getDistanceSquaredFrom(delayGraph.lineInProgressEnd) < static_cast<float>(outerHoverDistance * outerHoverDistance))) {
                    delayGraph.lineInProgressEndPoint = point.get();
                }
            }
            break;
        case DelayGraph::movingPoint:
        case DelayGraph::stretchingPoint:
            delayGraph.activePoint->offset = event.position - juce::Point<float>(delayGraph.activePoint->x, delayGraph.activePoint->y);
            break;
        case DelayGraph::none:
        case DelayGraph::editingLine:
        case DelayGraph::innerSelected:
        case DelayGraph::outerSelected:
        case DelayGraph::lineHover:
        default: break;
    }

    if (delayGraph.interactionState == DelayGraph::creatingLine) {
        delayGraph.lineInProgressEnd = event.position;
        delayGraph.lineInProgressEndPoint = nullptr;
        for (const auto& point : delayGraph.getPoints()) {
            if ((point.get() != delayGraph.activePoint) && (point->getDistanceSquaredFrom(delayGraph.lineInProgressEnd) < static_cast<float>(outerHoverDistance * outerHoverDistance))) {
                delayGraph.lineInProgressEndPoint = point.get();
            }
        }
    }
}

void PlaygroundControllerOld::mouseUp (const juce::MouseEvent& event)
{
    switch (delayGraph.interactionState) {
        case DelayGraph::none:
            if (event.mouseWasClicked()) {
                delayGraph.addPoint(event.position);
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
                delayGraph.addPoint(event.position, true);
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
        case DelayGraph::movingPoint:
            delayGraph.interactionState = DelayGraph::none;
            delayGraph.bakeOffsets();
            break;
        case DelayGraph::outerSelected:
        default: break;
    }

    setHoveredPoint(event.position);
}

void PlaygroundControllerOld::mouseDoubleClick (const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    switch (delayGraph.interactionState) {
        case DelayGraph::innerSelected:
            if (delayGraph.activePoint->pointType == GraphPoint::inner) {
                delayGraph.deletePoint(delayGraph.activePoint);
            }
            break;
        case DelayGraph::lineHover:
        case DelayGraph::editingLine:
            delayGraph.deleteLine(delayGraph.activeLine);
            delayGraph.interactionState = DelayGraph::none;
            break;
        case DelayGraph::none:
        case DelayGraph::outerSelected:
        case DelayGraph::movingPoint:
        case DelayGraph::stretchingPoint:
        case DelayGraph::creatingLine:
        default: break;
    }
}

void PlaygroundControllerOld::setHoveredPoint (const juce::Point<float>& mousePoint)
{
    if (delayGraph.interactionState == DelayGraph::editingLine) {
        return;
    }
    GraphPoint* closestPoint = nullptr;
    auto squareDistance = outerHoverDistance * outerHoverDistance;
    for (const auto& point : delayGraph.getPoints()) {
        int dist = static_cast<int>(point->getDistanceSquaredFrom(mousePoint));
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
        if (l.getDistanceFromPoint(mousePoint, pointOnLine) < static_cast<float>(lineHoverDistance)) {
            delayGraph.activeLine = line.get();
            delayGraph.interactionState = DelayGraph::lineHover;
            return;
        }
    }
    delayGraph.activePoint = nullptr;
    delayGraph.interactionState = DelayGraph::none;
}