//
// Created by arden on 3/19/24.
//

#include "GraphPoint.h"

GraphPoint::GraphPoint(const juce::Point<float>& p, PointType pt, const int& id) : juce::Point<float>(p), pointType(pt), draggingOffset(false), identifier(id) {
    offset.setXY(0,0);
    startTimerHz(30);
}


GraphPoint::GraphPoint (juce::XmlElement* element)
    : GraphPoint(juce::Point<float>(
                      static_cast<float>(element->getDoubleAttribute("x")),
                      static_cast<float>(element->getDoubleAttribute("y"))),
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
        x = static_cast<float>(element->getDoubleAttribute("x", x));
        y = static_cast<float>(element->getDoubleAttribute("y", y));
        pointType = static_cast<PointType>(element->getIntAttribute("type", pointType));
        return true;
    } else {
        return false;
    }
}

bool GraphPoint::modulateIfPossible (ModulatableKey& key, float newValue, bool* anythingChanged)
{
    if (anythingChanged) {
        *anythingChanged = false;
    }
    auto newValueScaled = key.get(newValue);
    if (key.parameterId == juce::Identifier("x") && !juce::approximatelyEqual(this->x, newValueScaled)) {
        this->x = newValueScaled;
        if (anythingChanged) {
            *anythingChanged = true;
        }
        return true;
    } else if (key.parameterId == juce::Identifier("y") & !juce::approximatelyEqual(this->y, newValueScaled)) {
        this->y = newValueScaled;
        if (anythingChanged) {
            *anythingChanged = true;
        }
        return true;
    }
    return false;
}

bool GraphPoint::getModulationValue (ModulatableKey& key, float& val)
{
    if (key.parameterId == juce::Identifier("x")) {
        val = key.range.convertTo0to1(this->x);
        return true;
    } else if (key.parameterId == juce::Identifier("y")) {
        val = key.range.convertTo0to1(this->y);
        return true;
    }
    return false;
}
