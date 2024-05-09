//
// Created by arden on 3/14/24.
//

#include "PlaygroundView.h"
#include <numbers> 

void PlaygroundView::resized()
{

}

juce::AffineTransform makeTransform(juce::Point<float> start, juce::Point<float> end, int channel) {
    auto transform = juce::AffineTransform();
    transform = transform.scaled(start.getDistanceFrom(end), channel ? 50 : -50);
    transform = transform.rotated(start.getAngleToPoint(end) - juce::MathConstants<float>::halfPi);
    transform = transform.translated(start);
    return transform;
}


void PlaygroundView::drawLine (juce::Graphics& g, GraphLine* line)
{
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
    for (float p = 0; p < 1; p += 1 / line->start->getDistanceFrom(*line->end)) {
        line->getEnvelope(p, l, r);
        leftLinePath.lineTo(p, l);
        rightLinePath.lineTo(p, r);
    }
    leftLinePath.lineTo(1,0);
    rightLinePath.lineTo(1,0);
    leftLinePath.closeSubPath();
    rightLinePath.closeSubPath();


    g.fillPath(leftLinePath, makeTransform(*line->start + line->start->offset, *line->end + line->end->offset, 0));
    g.fillPath(rightLinePath, makeTransform(*line->start + line->start->offset, *line->end + line->end->offset, 1));
    g.drawLine(line->start->x, line->start->y, line->end->x, line->end->y, 3);
}


void PlaygroundView::drawPoint (juce::Graphics& g, GraphPoint* point)
{
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

void PlaygroundView::drawLineBeingCreated (juce::Graphics& g)
{
    if (delayGraph.interactionState == DelayGraph::creatingLine) {
        g.setColour(juce::Colours::orange);
        if (delayGraph.lineInProgressEndPoint) {
            g.drawLine(delayGraph.activePoint->x, delayGraph.activePoint->y, delayGraph.lineInProgressEndPoint->x, delayGraph.lineInProgressEndPoint->y, 3);
            g.setColour(juce::Colours::green);
            g.fillEllipse(delayGraph.lineInProgressEndPoint->x - 10, delayGraph.lineInProgressEndPoint->y - 10, 20, 20);

        } else {
            g.drawLine(delayGraph.activePoint->x, delayGraph.activePoint->y, delayGraph.lineInProgressEnd.x, delayGraph.lineInProgressEnd.y, 3);
        }
    }
}

void PlaygroundView::paint (juce::Graphics& g)
{
    makeLineEditorIfNeeded();
    g.setColour(juce::Colours::tan);
    g.fillAll();
    for (const auto& line : delayGraph.getLines()) {
        drawLine(g, line.get());
    }

    drawLineBeingCreated(g);

    for (const auto& point : delayGraph.getPoints()) {
        drawPoint(g, point.get());
    }
}

void PlaygroundView::timerCallback()
{
    repaint();
}

void PlaygroundView::makeLineEditorIfNeeded()
{
    if ((!lineEditor) && (delayGraph.interactionState == DelayGraph::editingLine)) {
        lineEditor = std::make_unique<LineEditor>(delayGraph, delayGraph.activeLine->identifier);
        addAndMakeVisible(lineEditor.get());
        lineEditor->setBounds(10,10,400,300);
    } else if ((lineEditor) && (delayGraph.interactionState != DelayGraph::editingLine)) {
        removeChildComponent(lineEditor.get());
        auto ed = lineEditor.release();
        delete ed;
    }
}
const std::unique_ptr<LineEditor>* PlaygroundView::getLineEditor()
{
    return &lineEditor;
}
