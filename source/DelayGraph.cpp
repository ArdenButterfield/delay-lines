//
// Created by arden on 3/14/24.
//

#include "DelayGraph.h"
#include <string>
#include <algorithm>

DelayGraph::DelayGraph()
{
    activePoint = nullptr;
    interactionState = none;

    points.push_back(std::make_unique<StartPoint>(juce::Point<float>(0,0)));
    startPoint = points.back().get();
    points.push_back(std::make_unique<EndPoint>(juce::Point<float>(100,100)));
    endPoint = points.back().get();
    addLine(points[0].get(), points[1].get());
}

void DelayGraph::addPoint(const juce::Point<float>& point, bool connectToSelected) {
    points.push_back(std::make_unique<InnerPoint>(point));
    points.back()->prepareToPlay(processSpec.get());
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
    lines.push_back(std::make_unique<GraphLine>(start, end, juce::Identifier(std::to_string(line_id++))));
    if (processSpec) {
        lines.back()->prepareToPlay(processSpec.get());
    }
}

std::vector<std::unique_ptr<GraphLine>>& DelayGraph::getLines()
{
    return lines;
}

void DelayGraph::deletePoint (const GraphPoint* point)
{
    if (point->pointType == GraphPoint::start || point->pointType == GraphPoint::end) {
        return;
    }
    criticalSection.enter();
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
    criticalSection.exit();
}

void DelayGraph::deleteLine (const GraphLine* line)
{
    criticalSection.enter();
    for (auto iter = lines.begin(); iter != lines.end(); ) {
        if (iter->get() == line) {
            iter = lines.erase(iter);
        } else {
            ++iter;
        }
    }
    criticalSection.exit();
}

void DelayGraph::prepareToPlay (juce::dsp::ProcessSpec& spec)
{
    processSpec = std::make_unique<juce::dsp::ProcessSpec>(spec);
    for (auto& line : lines) {
        line->prepareToPlay(processSpec.get());
    }
    for (auto& point : points) {
        point->prepareToPlay(processSpec.get());
    }
}

void DelayGraph::processSample (std::vector<float>& sample)
{
    for (unsigned channel = 0; channel < processSpec->numChannels; ++channel) {
        startPoint->samples[channel] += sample[channel];
    }
    criticalSection.enter();
    for (auto& line : lines) {
        line->pushSample(line->start->samples);
    }

    for (auto& point : points) {
        std::fill(point->samples.begin(), point->samples.end(), 0);
    }
    for (auto& line : lines) {
        line->popSample(line->end->samples);
    }
    for (unsigned channel = 0; channel < processSpec->numChannels; ++channel) {
        sample[channel] = endPoint->samples[channel];
    }
    criticalSection.exit();
}

void DelayGraph::bakeOffsets()
{
    for (auto& line : lines) {
        line->bakeOffset();
    }
    for (auto& point : points) {
        point->bakeOffset();
    }

}
GraphLine* DelayGraph::getLine (const juce::Identifier& id)
{
    for (auto& line : lines) {
        if (line->identifier == id) {
            return line.get();
        }
    }
    return nullptr;
}

void DelayGraph::setRealOutputs()
{
    // the real outputs of a line are the points that can be reached from its endpoint traversing only
    // bypassed lines. We also need to treat any point reachable from the start point through bypassed
    // lines as a start point, and give it the input values.
    for (auto& line : lines) {
        std::set<GraphPoint*> potentialOutputs;
        line->realOutputs.clear();
        potentialOutputs.insert(line->end);
        while (!potentialOutputs.empty()) {
            auto point = *potentialOutputs.begin();
            potentialOutputs.erase(potentialOutputs.begin());
            line->realOutputs.insert(point);
            for (auto& l : lines) {
                if ((l->start == point)
                    && l->parameters.bypass
                    && (potentialOutputs.find(l->end) == potentialOutputs.end())
                    && (line->realOutputs.find(l->end) == line->realOutputs.end())) {
                    potentialOutputs.insert(l->end);
                }
            }
        }
    }

    realGlobalInputs.clear();
    realGlobalInputs.insert(startPoint);
    for (auto& line : lines) {
        if ((line->start == startPoint) && (line->parameters.bypass)) {
            realGlobalInputs.insert(line->realOutputs.begin(), line->realOutputs.end());
        }
    }
}
