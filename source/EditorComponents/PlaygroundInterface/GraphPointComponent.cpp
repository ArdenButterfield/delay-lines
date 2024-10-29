//
// Created by arden on 7/28/24.
//

#include "GraphPointComponent.h"
#include "PlaygroundInterface.h"

GraphPointComponent::GraphPointComponent (ModulationMappingEngine& me, DelayGraph& _delayGraph, PlaygroundInterface* _playgroundInterface, int id)
    : delayGraph(_delayGraph),
      playgroundInterface(_playgroundInterface),
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

    auto pointWithOffset = *point + playgroundInterface->getGlobalOffset() + point->offset;

    bool shouldDrawPointBig = false;

    if (delayGraph.interactionState == DelayGraph::creatingLine && point == delayGraph.lineInProgressEndPoint) {
        auto rad = static_cast<float>(juce::Time::currentTimeMillis() % 700) / 700;
        rad = 1 - rad;
        g.setColour(juce::Colours::yellow.withAlpha(0.5f));
        auto coronaWidth = outerHoverDistance - innerHoverDistance;
        g.drawEllipse(pointWithOffset.x - outerHoverDistance,
            pointWithOffset.y - outerHoverDistance,
            outerHoverDistance * 2,
            outerHoverDistance * 2,
            coronaWidth + 1);
        g.setColour(juce::Colours::grey.withAlpha(1.f - rad));
        g.drawEllipse(pointWithOffset.x - innerHoverDistance - coronaWidth * rad,
            pointWithOffset.y - innerHoverDistance - coronaWidth * rad,
            (innerHoverDistance + coronaWidth * rad) * 2,
            (innerHoverDistance + coronaWidth * rad) * 2,
            rad * coronaWidth + 1);

        shouldDrawPointBig = true;
    }

    if ((point == delayGraph.activePoint) && (delayGraph.interactionState == DelayGraph::outerSelected)) {
        auto rad = static_cast<float>(juce::Time::currentTimeMillis() % 700) / 700;
        g.setColour(juce::Colours::grey.withAlpha(1.f - rad));
        g.setColour(juce::Colours::yellow.withAlpha(0.5f));
        auto coronaWidth = outerHoverDistance - innerHoverDistance;
        g.drawEllipse(pointWithOffset.x - outerHoverDistance,
            pointWithOffset.y - outerHoverDistance,
            outerHoverDistance * 2,
            outerHoverDistance * 2,
            coronaWidth + 1);
        g.drawEllipse(pointWithOffset.x - innerHoverDistance - coronaWidth * rad,
             pointWithOffset.y - innerHoverDistance - coronaWidth * rad,
             (innerHoverDistance + coronaWidth * rad) * 2,
             (innerHoverDistance + coronaWidth * rad) * 2,
            rad * coronaWidth + 1);

        shouldDrawPointBig = true;
    }
    g.setColour(juce::Colours::black);
    if (point == delayGraph.activePoint && delayGraph.interactionState == DelayGraph::innerSelected) {
        shouldDrawPointBig = true;
    }
    g.drawEllipse(pointWithOffset.x - innerHoverDistance,
        pointWithOffset.y - innerHoverDistance,
        innerHoverDistance * 2,
        innerHoverDistance * 2, shouldDrawPointBig ? 4 : 2);
    if (point == delayGraph.activePoint &&
        (delayGraph.interactionState == DelayGraph::innerSelected
        || delayGraph.interactionState == DelayGraph::movingPoint
        || delayGraph.interactionState == DelayGraph::stretchingPoint)) {
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        g.fillEllipse(pointWithOffset.x - innerHoverDistance,
            pointWithOffset.y - innerHoverDistance,
            innerHoverDistance * 2,
            innerHoverDistance * 2);
    }

    if (point->pointType == GraphPoint::start || point->pointType == GraphPoint::end) {
        auto color = point->pointType == GraphPoint::start ? juce::Colours::green : juce::Colours::orangered;
        auto rad = static_cast<float>(juce::Time::currentTimeMillis() % 2000) / 2000;
        rad = point->pointType == GraphPoint::start ? rad : 1 - rad;
        g.setColour(color.withAlpha(1 - rad));
        g.fillEllipse(pointWithOffset.x - innerHoverDistance * rad,
            pointWithOffset.y - innerHoverDistance * rad,
            innerHoverDistance * 2 * rad,
            innerHoverDistance * 2 * rad);
    }
    xMod.setBounds(pointWithOffset.x - 20, pointWithOffset.y - 5, 40, 10);
    yMod.setBounds(pointWithOffset.x - 5, pointWithOffset.y - 20, 10, 40);
}

void GraphPointComponent::resized() {
    xMod.modKey.range = {0, static_cast<float>(getWidth())};
    yMod.modKey.range = {0, static_cast<float>(getHeight())};
}

bool GraphPointComponent::hitTest (int x, int y) {
    if ((xMod.isVisible() && xMod.getBounds().contains(x,y)) ||
        (yMod.isVisible() && yMod.getBounds().contains(x,y))) {
        return true;
    }
    auto point = delayGraph.getPoint(identifier);
    if (point != nullptr) {
        auto distance = point->getDistanceSquaredFrom({static_cast<float>(x), static_cast<float>(y)});
        return distance < outerHoverDistance * outerHoverDistance;
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
            if  (/*(point.get() != delayGraph.activePoint) && */(point->getDistanceSquaredFrom(delayGraph.lineInProgressEnd) < static_cast<float>(outerHoverDistance * outerHoverDistance))) {
                delayGraph.lineInProgressEndPoint = point.get();
                delayGraph.lineInProgressEnd = *point;
            }
        }
        ghostComponent->end = delayGraph.lineInProgressEnd;
    }
}

void GraphPointComponent::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasClicked() && event.mods.isRightButtonDown()) {
        auto point = delayGraph.getPoint(identifier);
        delayGraph.deletePoint(point);
        return;
    }
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
    }
}

void GraphPointComponent::mouseDoubleClick (const juce::MouseEvent& event)
{
    auto point = delayGraph.getPoint(identifier);
    delayGraph.deletePoint(point);
}
