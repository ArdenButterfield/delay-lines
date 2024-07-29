//
// Created by arden on 7/28/24.
//

#include "GraphLineComponent.h"
#include "../DelayGraph.h"

static juce::AffineTransform makeTransform(juce::Point<float> start, juce::Point<float> end, int channel) {
    auto transform = juce::AffineTransform();
    transform = transform.scaled(start.getDistanceFrom(end), channel ? 50 : -50);
    transform = transform.rotated(start.getAngleToPoint(end) - juce::MathConstants<float>::halfPi);
    transform = transform.translated(start);
    return transform;
}


GraphLineComponent::GraphLineComponent (PlaygroundView* _playgroundView, DelayGraph* _delayGraph, int _id)
    : playgroundView(_playgroundView), delayGraph(_delayGraph), id(_id) {

}

GraphLineComponent::~GraphLineComponent()
{
}

void GraphLineComponent::paint (juce::Graphics& g)
{
    auto line = delayGraph->getLine(id);
    if (!line) {
        return;
    }
    if (delayGraph->interactionState == DelayGraph::lineHover && delayGraph->activeLine == line) {
        g.setColour(juce::Colours::yellow);
        g.drawLine(line->start->x, line->start->y, line->end->x, line->end->y, 10);
    } else if (delayGraph->interactionState == DelayGraph::editingLine && delayGraph->activeLine == line) {
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
    return Component::hitTest (x, y);
}
