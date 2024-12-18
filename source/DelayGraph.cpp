//
// Created by arden on 3/14/24.
//

#include "DelayGraph.h"
#include <string>
#include <algorithm>
#include <vector>

DelayGraph::DelayGraph() :
                           lineInProgressEndPoint(nullptr),
                           activePoint(nullptr),
                           interactionState(none),
                           activeLine(nullptr),
                           needsClearing(false),
                           bpm(120)
{
    points.push_back(std::make_unique<GraphPoint>(juce::Point<float>(0,0), GraphPoint::start, findUniquePointId()));
    startPoint = points.back().get();
    points.push_back(std::make_unique<GraphPoint>(juce::Point<float>(100,100), GraphPoint::end, findUniquePointId()));
    endPoint = points.back().get();
    addLine(points[0].get(), points[1].get());
}

DelayGraph::DelayGraph (juce::XmlElement* element) : startPoint(nullptr), endPoint(nullptr)
{
    needsClearing = false;
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
        if (points.back()->pointType == GraphPoint::start) {
            startPoint = points.back().get();
        }
        if (points.back()->pointType == GraphPoint::end) {
            endPoint = points.back().get();
        }
    }

    if (startPoint == nullptr) {
        DBG( "constructing from xml with no start point" );
    }
    if (endPoint == nullptr) {
        DBG( "constructing from xml with no end point" );
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
    // IDs start at 1 to avoid issues with juce comboboxes, which don't allow an ID of 0
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
    if (processSpec) {
        points.back()->prepareToPlay(processSpec.get());
    }
    if (connectToSelected) {
        auto added = points[points.size() - 1].get();
        addLine(activePoint, added);
        activePoint = added;
        interactionState = creatingLine;
    }
    for (auto& listener : listeners) {
        listener->pointAdded(points.back()->identifier);
    }

    criticalSection.exit();
}

void DelayGraph::addPoint (juce::XmlElement* xml)
{
    criticalSection.enter();

    points.push_back(std::make_unique<GraphPoint>(xml));
    if (processSpec) {
        points.back()->prepareToPlay(processSpec.get());
    }

    for (auto& listener : listeners) {
        listener->pointAdded(points.back()->identifier);
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
        lines.back()->prepareToPlay(*processSpec);
    }
    for (auto& listener : listeners) {
        listener->lineAdded(lines.back()->identifier);
    }
    criticalSection.exit();
}

void DelayGraph::addLine (juce::XmlElement* xml)
{
    criticalSection.enter();

    auto start = getPoint(xml->getIntAttribute("start"));
    auto end = getPoint(xml->getIntAttribute("end"));
    lines.push_back(std::make_unique<GraphLine>(start, end, xml));
    auto newLine = lines.back().get();
    if (processSpec) {
        newLine->prepareToPlay(*processSpec);
    }
    for (auto& listener : listeners) {
        listener->lineAdded(lines.back()->identifier);
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
    auto id = point->identifier;
    criticalSection.enter();
    if (point == activePoint) {
        activePoint = nullptr;
    }
    for (auto iter = lines.begin(); iter != lines.end(); ) {
        if (iter->get()->start == point || iter->get()->end == point) {
            auto lineId = iter->get()->identifier;
            for (auto& listener : listeners) {
                listener->lineRemoved(lineId);
            }
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
    for (auto& listener : listeners) {
        listener->pointRemoved(id);
    }
    criticalSection.exit();
}

void DelayGraph::deleteLine (const GraphLine* line)
{
    auto id = line->identifier;
    criticalSection.enter();
    if (line == activeLine) {
        activeLine = nullptr;
    }
    for (auto iter = lines.begin(); iter != lines.end(); ) {
        if (iter->get() == line) {
            iter = lines.erase(iter);
        } else {
            ++iter;
        }
    }
    for (auto& listener : listeners) {
        listener->lineRemoved(id);
    }
    criticalSection.exit();
}

void DelayGraph::prepareToPlay (juce::dsp::ProcessSpec& spec)
{
    processSpec = std::make_unique<juce::dsp::ProcessSpec>(spec);
    for (auto& line : lines) {
        line->prepareToPlay(*processSpec);
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

    // FLOOD
    for (unsigned channel = 0; channel < processSpec->numChannels; ++channel) {
        for (auto point : realGlobalInputs) {
            point->samples[channel] += sample[channel];
        }
    }

    // POP
    for (auto& line : lines) {
        line->popSample();
    }

    // RETURN
    for (unsigned channel = 0; channel < processSpec->numChannels; ++channel) {
        sample[channel] = endPoint->samples[channel];
    }

    // PUSH
    for (auto& line : lines) {
        line->pushSample(line->start->samples);
    }

    // CLEAR
    for (auto& point : points) {
        std::fill(point->samples.begin(), point->samples.end(), 0);
    }


    if (needsClearing) {
        if (clearingFadeoutCounter > 0) {
            clearingFadeoutCounter -= 1.f / clearingFadeoutLength;
            for (auto& v : sample) {
                v *= clearingFadeoutCounter;
            }
        } else {
            for (auto& line : lines) {
                line->clear();
            }
            needsClearing = false;
        }
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

bool DelayGraph::copyPoint (const int id, std::unique_ptr<GraphPoint>& out) {
    auto p = getPoint(id);
    if (p == nullptr) {
        return false;
    }

    out = std::make_unique<GraphPoint>(*p);
    return true;
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

    std::vector<GraphPoint*> pointsToDelete;
    for (auto& point : points) {
        if (!point->importFromXml(pointsElement)) {
            pointsToDelete.push_back(point.get());
        }
    }
    for (auto point : pointsToDelete) {
        deletePoint(point);
    }

    for (int i = 0; i < pointsElement->getNumChildElements(); ++i) {
        auto p = pointsElement->getChildElement(i);
        if (!getPoint(p->getIntAttribute("id"))) {
            addPoint(p);
        }
    }

    std::vector<GraphLine*> linesToDelete;

    for (auto& line : lines) {
        if (!line->importFromXml(this, linesElement)) {
            linesToDelete.push_back(line.get());
        }
    }

    for (auto line : linesToDelete) {
        deleteLine(line);
    }

    for (int i = 0; i < linesElement->getNumChildElements(); ++i) {
        auto l = linesElement->getChildElement(i);
        if (!getLine(l->getIntAttribute("id"))) {
            addLine(l);
        }
    }
    setRealOutputs();
    return true;
}

bool DelayGraph::modulateIfPossible (ModulatableKey& key, float newValue)
{
    if (key.type == ModulatableKey::point) {
        for (auto& point : points) {
            if (point->identifier == key.pointOrLineId) {
                return point->modulateIfPossible(key, newValue);
            }
        }
    } else if (key.type == ModulatableKey::line) {
        for (auto& line : lines) {
            if (line->identifier == key.pointOrLineId) {
                return line->modulateIfPossible(key, newValue);
            }
        }
    }
    return false;
}

bool DelayGraph::getModulationValue (ModulatableKey& key, float& val)
{
    if (key.type == ModulatableKey::point) {
        for (auto& point : points) {
            if (point->identifier == key.pointOrLineId) {
                return point->getModulationValue(key, val);
            }
        }
    } else if (key.type == ModulatableKey::line) {
        for (auto& line : lines) {
            if (line->identifier == key.pointOrLineId) {
                return line->getModulationValue(key, val);
            }
        }
    }
    return false;
}


void DelayGraph::setMidiTrackNote (int pitch)
{
    for (auto& line : lines) {
        line->setMidiTrackNote(pitch);
    }
}
void DelayGraph::addListener (DelayGraph::Listener* listener)
{
    listeners.insert(listener);
}

void DelayGraph::removeListener (DelayGraph::Listener* listener)
{
    if (listeners.contains(listener)) {
        listeners.erase(listener);
    }
}

std::vector<int> DelayGraph::getAllPointIds()
{
    auto out = std::vector<int>();
    for (auto& point : points) {
        out.push_back(point->identifier);
    }
    return out;
}

std::vector<int> DelayGraph::getAllLineIds()
{
    auto out = std::vector<int>();
    for (auto& line : lines) {
        out.push_back(line->identifier);
    }
    return out;
}

void DelayGraph::applyGlobalOffset (juce::Point<float> offset)
{
    for (auto& point : points) {
        *point += offset;
    }
}
void DelayGraph::clear()
{
    needsClearing = true;
    clearingFadeoutCounter = 1.f;
}
std::vector<int> DelayGraph::getAllLinesBetweenPoints (GraphPoint* pointA, GraphPoint* pointB)
{
    auto linesIds = std::vector<int>();
    for (auto& line : lines) {
        if ((line->start == pointA && line->end == pointB) || (line->start == pointB && line->end == pointA)) {
            linesIds.push_back(line->identifier);
        }
    }
    return linesIds;
}

std::vector<int> DelayGraph::getAllLinesStartingAndEndingAtSamePoint (GraphPoint* point)
{
    auto linesIds = std::vector<int>();
    for (auto& line : lines) {
        if (line->start == point && line->end == point) {
            linesIds.push_back(line->identifier);
        }
    }
    return linesIds;
}
void DelayGraph::setStretchTime (float newStretchTime)
{
    for (auto& line : lines) {
        line->setStretchTime(newStretchTime);
    }
}

void DelayGraph::noteCallback (const juce::MidiMessage& message)
{
    if (message.isNoteOn()) {
        for (auto& line : lines) {
            line->setMidiTrackNote(message.getNoteNumber());
        }
    }
}
