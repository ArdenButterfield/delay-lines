//
// Created by arden on 3/14/24.
//

#include "PlaygroundArea.h"

void PlaygroundArea::resized()
{

}

void PlaygroundArea::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::tan);
    g.fillAll();
    for (const auto& line : delayGraph.getLines()) {
        g.setColour(juce::Colours::brown);
        g.drawLine(line.start->x, line.start->y, line.end->x, line.end->y, 3);
    }

    if (delayGraph.activePointAction == DelayGraph::creatingLine) {
        g.setColour(juce::Colours::orange);
        if (delayGraph.lineInProgressEndPoint) {
            g.drawLine(delayGraph.activePoint->x, delayGraph.activePoint->y, delayGraph.lineInProgressEndPoint->x, delayGraph.lineInProgressEndPoint->y, 3);
            g.setColour(juce::Colours::green);
            g.fillEllipse(delayGraph.lineInProgressEndPoint->x - 10, delayGraph.lineInProgressEndPoint->y - 10, 20, 20);

        } else {
            g.drawLine(delayGraph.activePoint->x, delayGraph.activePoint->y, delayGraph.lineInProgressEnd.x, delayGraph.lineInProgressEnd.y, 3);
        }
    }

    for (const auto& point : delayGraph.getPoints()) {
        if (point.get() == delayGraph.activePoint) {
            if (delayGraph.activePointAction == DelayGraph::outerSelected) {
                g.setColour(juce::Colours::blue);
                g.fillEllipse(point->x - 10, point->y - 10, 20, 20);
                g.setColour(juce::Colours::brown);

            } else if (delayGraph.activePointAction == DelayGraph::movingPoint) {
                g.setColour(juce::Colours::black);
            } else {
                g.setColour(juce::Colours::red);
            }
        } else {
            g.setColour(juce::Colours::brown);
        }
        g.drawEllipse(point->x - 5, point->y - 5, 10, 10, 3);
    }

}

void PlaygroundArea::timerCallback()
{
    repaint();
}
