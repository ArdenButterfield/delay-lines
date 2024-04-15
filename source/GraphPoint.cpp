//
// Created by arden on 3/19/24.
//

#include "GraphPoint.h"
juce::XmlElement GraphPoint::exportToXml (juce::XmlElement* parent)
{
    auto element = parent->createNewChildElement(identifier);
    element->setAttribute("x", x);
    element->setAttribute("y", y);
    element->setAttribute("type", pointType);
}

bool GraphPoint::importFromXml (juce::XmlElement* parent)
{
    auto element = parent->getChildByName(identifier);
    if (element) {
        x = element->getDoubleAttribute("x", x);
        y = element->getDoubleAttribute("y", y);
        return true;
    } else {
        return false;
    }
}
