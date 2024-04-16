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
std::string GraphPoint::idToString()
{
    // for xml
    return "point_" + std::to_string(identifier);
}

int GraphPoint::stringToId (std::string s)
{
    // for xml
    return std::stoi(s.erase(0, 6));
}
