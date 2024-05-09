//
// Created by arden on 3/19/24.
//

#include "GraphPoint.h"

GraphPoint::GraphPoint(const juce::Point<float>& p, PointType pt, const int& id) : juce::Point<float>(p), pointType(pt), draggingOffset(false), identifier(id) {
    offset.setXY(0,0);
    startTimerHz(30);
}


GraphPoint::GraphPoint (juce::XmlElement* element)
    : GraphPoint(juce::Point<float>(element->getDoubleAttribute("x"), element->getDoubleAttribute("y")),
        static_cast<const PointType> (element->getIntAttribute ("type")),
        element->getIntAttribute("id"))
{
}

void GraphPoint::prepareToPlay(juce::dsp::ProcessSpec* spec) {
    samples.resize(spec->numChannels);
}

void GraphPoint::timerCallback() {
    if (!draggingOffset) {
        offset *= 0.9;
        if (offset.getDistanceSquaredFromOrigin() < 0.2) {
            offset *= 0;
        }
    }
}

void GraphPoint::bakeOffset() {
    this->x += offset.x;
    this->y += offset.y;
    offset.x = 0;
    offset.y = 0;
}


void GraphPoint::exportToXml (juce::XmlElement* parent)
{
    auto element = parent->createNewChildElement("point");
    element->setAttribute("id", std::to_string(identifier));
    element->setAttribute("x", x);
    element->setAttribute("y", y);
    element->setAttribute("type", pointType);
}

bool GraphPoint::importFromXml (juce::XmlElement* parent)
{
    auto element = parent->getChildByAttribute("id", std::to_string(identifier));
    if (element) {
        x = element->getDoubleAttribute("x", x);
        y = element->getDoubleAttribute("y", y);
        pointType = static_cast<PointType>(element->getIntAttribute("type", pointType));
        return true;
    } else {
        return false;
    }
}
bool GraphPoint::modulateIfPossible (ModulatableKey& key, float newValue)
{
    if (key.parameterId == juce::Identifier("x")) {
        this->x = newValue;
        return true;
    } else if (key.parameterId == juce::Identifier("y")) {
        this->y = newValue;
        return true;
    }
    return false;
}
