//
// Created by arden on 3/14/24.
//

#include "DelayGraph.h"
#include <string>
#include <algorithm>
#include <vector>

DelayGraph::DelayGraph()
{
    activePoint = nullptr;
    interactionState = none;

    points.push_back(std::make_unique<GraphPoint>(juce::Point<float>(0,0), GraphPoint::start, findUniquePointId()));
    startPoint = points.back().get();
    points.push_back(std::make_unique<GraphPoint>(juce::Point<float>(100,100), GraphPoint::end, findUniquePointId()));
    endPoint = points.back().get();
    addLine(points[0].get(), points[1].get());
}

DelayGraph::DelayGraph (juce::XmlElement* element)
{
    activePoint = nullptr;
    interactionState = none;

    auto pointsElement = element->getChildByName("points");
    if (!pointsElement) {
        DBG( "no points element found" );
        return;
    }

    for (int i = 0; i < pointsElement->getNumChildElements(); ++i) {
        auto p = pointsElement->getChildElement(i);
        points.push_back(std::make_unique<GraphPoint>(p));
    }

    auto linesElement = element->getChildByName("lines");
    if (!linesElement) {
        DBG( "no lines element found" );
        return;
    }

    for (int i = 0; i < linesElement->getNumChildElements(); ++i) {
        auto l = linesElement->getChildElement(i);
        auto start = getPoint(l->getIntAttribute("start"));
        auto end = getPoint(l->getIntAttribute("end"));
        lines.push_back(std::make_unique<GraphLine>(start, end, l));
    }

    for (auto& point : points) {
        if (!point->importFromXml(pointsElement)) {
            deletePoint(point.get());
        }
    }


    for (auto& line : lines) {
        if (!line->importFromXml(this, linesElement)) {
            deleteLine(line.get());
        }
    }

}

int DelayGraph::findUniqueLineId()
{
    auto id = 0;
    auto idConflict = true;
    while (idConflict) {
        idConflict = false;
        ++id;
        for (auto& line : lines) {
            if (line->identifier == id) {
                idConflict = true;
            }
        }
    }

    return id;
}

int DelayGraph::findUniquePointId()
{
    auto id = 0;
    auto idConflict = true;
    while (idConflict) {
        idConflict = false;
        ++id;
        for (auto& point : points) {
            if (point->identifier == id) {
                idConflict = true;
            }
        }
    }

    return id;
}


void DelayGraph::addPoint(const juce::Point<float>& point, bool connectToSelected) {
    criticalSection.enter();

    points.push_back(std::make_unique<GraphPoint>(point, GraphPoint::inner, findUniquePointId()));
    points.back()->prepareToPlay(processSpec.get());
    if (connectToSelected) {
        auto added = points[points.size() - 1].get();
        addLine(activePoint, added);
        activePoint = added;
        interactionState = creatingLine;
    }
    criticalSection.exit();

}

const std::vector<std::unique_ptr<GraphPoint>>& DelayGraph::getPoints() {
    return points;
}

void DelayGraph::addLine (GraphPoint* start, GraphPoint* end)
{
    criticalSection.enter();

    lines.push_back(std::make_unique<GraphLine>(start, end, findUniqueLineId()));
    if (processSpec) {
        lines.back()->prepareToPlay(processSpec.get());
    }
    criticalSection.exit();
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

    setRealOutputs();
    startTimerHz(60);
}

void DelayGraph::processSample (std::vector<float>& sample)
{
    criticalSection.enter();
    for (auto& point : points) {
        std::fill(point->samples.begin(), point->samples.end(), 0);
    }

    for (unsigned channel = 0; channel < processSpec->numChannels; ++channel) {
        for (auto point : realGlobalInputs) {
            point->samples[channel] += sample[channel];
        }
    }

    for (auto& line : lines) {
        line->pushSample(line->start->samples);
    }

    for (auto& line : lines) {
        line->popSample();
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

GraphPoint* DelayGraph::getPoint (const int& id)
{
    for (auto& point : points) {
        if (point->identifier == id) {
            return point.get();
        }
    }
    return nullptr;
}


GraphLine* DelayGraph::getLine (const int& id)
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
        criticalSection.enter();
        std::set<GraphPoint*> potentialOutputs;
        line->realOutputs.clear();
        potentialOutputs.insert(line->end);
        while (!potentialOutputs.empty()) {
            auto point = *potentialOutputs.begin();
            potentialOutputs.erase(potentialOutputs.begin());
            line->realOutputs.insert(point);
            for (auto& l : lines) {
                if ((l->start == point)
                    && l->parameters.isBypassed()
                    && (potentialOutputs.find(l->end) == potentialOutputs.end())
                    && (line->realOutputs.find(l->end) == line->realOutputs.end())) {
                    potentialOutputs.insert(l->end);
                }
            }
        }
        criticalSection.exit();
    }

    criticalSection.enter();
    realGlobalInputs.clear();
    realGlobalInputs.insert(startPoint);
    for (auto& line : lines) {
        if ((line->start == startPoint) && (line->parameters.isBypassed())) {
            realGlobalInputs.insert(line->realOutputs.begin(), line->realOutputs.end());
        }
    }
    criticalSection.exit();
}

void DelayGraph::timerCallback()
{
    setRealOutputs();
}

void DelayGraph::exportToXml (juce::XmlElement* parent)
{
    auto element = parent->createNewChildElement("graph");

    auto pointsElement = element->createNewChildElement("points");

    for (auto& point : points) {
        point->exportToXml(pointsElement);
    }
    auto linesElement = element->createNewChildElement("lines");

    for (auto& line : lines) {
        line->exportToXml(linesElement);
    }
}

bool DelayGraph::importFromXml (juce::XmlElement* parent)
{
    auto element = parent->getChildByName("graph");

    if (!element) {
        return false;
    }

    auto linesElement = element->getChildByName("lines");
    auto pointsElement = element->getChildByName("points");
    if (!linesElement || !pointsElement) {
        return false;
    }

    std::cout << "attempting to import points\n";
    std::vector<GraphPoint*> pointsToDelete;
    for (auto& point : points) {
        if (!point->importFromXml(pointsElement)) {
            pointsToDelete.push_back(point.get());
        }
    }
    for (auto point : pointsToDelete) {
        std::cout << "delete point\n";
        deletePoint(point);
    }

    std::cout << "making new points as needed\n";
    for (int i = 0; i < pointsElement->getNumChildElements(); ++i) {
        auto p = pointsElement->getChildElement(i);
        if (!getPoint(p->getIntAttribute("id"))) {
            points.push_back(std::make_unique<GraphPoint>(p));
        }
    }

    std::vector<GraphLine*> linesToDelete;
    std::cout << "attempting to import to lines\n";

    for (auto& line : lines) {
        if (!line->importFromXml(this, linesElement)) {
            linesToDelete.push_back(line.get());
        }
    }

    for (auto line : linesToDelete) {
        std::cout << "delete line\n";
        deleteLine(line);
    }

    std::cout << "making new lines as needed\n";
    for (int i = 0; i < linesElement->getNumChildElements(); ++i) {
        auto l = linesElement->getChildElement(i);
        if (!getLine(l->getIntAttribute("id"))) {
            auto start = getPoint(l->getIntAttribute("start"));
            auto end = getPoint(l->getIntAttribute("end"));
            lines.push_back(std::make_unique<GraphLine>(start, end, l));
        }
    }
    std::cout << "done\n";
    return true;
}
