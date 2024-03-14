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
    if (delayGraph.activePointAction == DelayGraph::innerSelected) {
        delayGraph.activePointAction = DelayGraph::movingPoint;
    } else if (delayGraph.activePointAction == DelayGraph::outerSelected) {
        delayGraph.activePointAction = DelayGraph::creatingLine;
        delayGraph.lineInProgressEnd = event.getPosition();
        delayGraph.lineInProgressEndPoint = nullptr;
    }
}

void PlaygroundController::mouseDrag (const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown()) {
        return;
    }

    if (delayGraph.activePointAction == DelayGraph::movingPoint) {
        delayGraph.activePoint->setXY(event.getPosition().x, event.getPosition().y);
    } else if (delayGraph.activePointAction == DelayGraph::creatingLine) {
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
    if (event.mouseWasClicked() && event.mods.isRightButtonDown() && delayGraph.activePointAction == DelayGraph::innerSelected) {
        std::cout << "delete point\n";
        delayGraph.deletePoint(delayGraph.activePoint);
        return;
    }
    if (delayGraph.activePointAction == DelayGraph::creatingLine) {
        if (delayGraph.lineInProgressEndPoint) {
            delayGraph.addLine({delayGraph.activePoint, delayGraph.lineInProgressEndPoint});
        } else {
            delayGraph.addPoint(GraphPoint(event.getPosition()), true);
        }
    } else if (event.mouseWasClicked()) {
        delayGraph.addPoint(GraphPoint(event.getPosition()));
    }
    setHoveredPoint(event.getPosition());
}

void PlaygroundController::mouseDoubleClick (const juce::MouseEvent& event)
{
    std::cout << "doubleclick\n";
}

void PlaygroundController::setHoveredPoint (const juce::Point<int>& mousePoint)
{
    GraphPoint* closestPoint = nullptr;
    int squareDistance = outerHoverDistance * outerHoverDistance;
    for (const auto& point : delayGraph.getPoints()) {
        int dist = point->getDistanceSquaredFrom(mousePoint);
        if (dist < squareDistance) {
            squareDistance = dist;
            closestPoint = point.get();
        }
    }
    if (closestPoint) {
        delayGraph.activePoint = closestPoint;
        if (squareDistance < innerHoverDistance * innerHoverDistance) {
            delayGraph.activePointAction = DelayGraph::innerSelected;
        } else {
            delayGraph.activePointAction = DelayGraph::outerSelected;
        }
    } else {
        delayGraph.activePoint = nullptr;
        delayGraph.activePointAction = DelayGraph::none;
    }
}

