//
// Created by arden on 3/14/24.
//

#include "DelayGraph.h"

void DelayGraph::addPoint(const GraphPoint& point, bool connectToSelected) {
    points.push_back(std::make_unique<GraphPoint>(point));
    if (connectToSelected) {
        auto added = points[points.size() - 1].get();
        addLine(activePoint, added);
        activePoint = added;
        interactionState = creatingLine;
    }
}
const std::vector<std::unique_ptr<GraphPoint>>& DelayGraph::getPoints() {
    return points;
}

void DelayGraph::addLine (GraphPoint* start, GraphPoint* end)
{
    lines.push_back(std::make_unique<GraphLine>(start, end));
    if (processSpec) {
        lines[lines.size() - 1]->prepareToPlay(processSpec.get());
    }
}

std::vector<std::unique_ptr<GraphLine>>& DelayGraph::getLines()
{
    return lines;
}

void DelayGraph::deletePoint (const GraphPoint* point)
{
    for (auto iter = lines.begin(); iter != lines.end(); ) {
        if (iter->get()->start == point || iter->get()->end == point) {
            iter = lines.erase(iter);
        } else {
            ++iter;
        }
    }
    for (auto iter = points.begin(); iter != points.end(); ) {
        if (&(*iter->get()) == point) {
            iter = points.erase(iter);
        } else {
            ++iter;
        }
    }
}

void DelayGraph::deleteLine (const GraphLine* line)
{
    for (auto iter = lines.begin(); iter != lines.end(); ) {
        if (iter->get() == line) {
            iter = lines.erase(iter);
        } else {
            ++iter;
        }
    }
}

void DelayGraph::prepareToPlay (juce::dsp::ProcessSpec& spec)
{
    processSpec = std::make_unique<juce::dsp::ProcessSpec>(spec);
    for (auto& line : lines) {
        line->prepareToPlay(processSpec.get());
    }
}