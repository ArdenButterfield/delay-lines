//
// Created by arden on 5/3/24.
//

#ifndef DELAYLINES_MODULATABLEKEY_H
#define DELAYLINES_MODULATABLEKEY_H

#include "juce_core/juce_core.h"

struct ModulatableKey
{
    enum Type {
        point,
        line
    };

    Type type;

    int pointOrLineId;

    juce::Identifier parameterId;

    juce::NormalisableRange<double> range;

    [[nodiscard]] float get(float modPosition) const {
        return range.convertFrom0to1(modPosition);
    }

    [[nodiscard]] juce::String getName() const {
        auto name = juce::String();
        name += (type == point) ? "point" : "line";
        name += " ";
        name += juce::String(pointOrLineId);
        name += ": ";
        name += parameterId;
        return name;
    }

    void exportToXml(juce::XmlElement* parent) const {
        auto element = parent->createNewChildElement("key");
        element->setAttribute("type", static_cast<int>(type));
        element->setAttribute("pointOrLineId", pointOrLineId);
        element->setAttribute("paramId", parameterId.toString());
        element->setAttribute("range-start", range.start);
        element->setAttribute("range-end", range.end);
        element->setAttribute("range-skew", range.skew);
    }

    void importFromXml(juce::XmlElement* parent) {
        auto element = parent->getChildByName("key");
        if (element == nullptr) {
            return;
        }
        type = static_cast<Type> (element->getIntAttribute ("type"));
        pointOrLineId = element->getIntAttribute("pointOrLineId");
        parameterId = element->getStringAttribute("paramId");
        range.start = element->getDoubleAttribute("range-start");
        range.end = element->getDoubleAttribute("range-end");
        range.skew = element->getDoubleAttribute("range-skew");
    }
};

#endif //DELAYLINES_MODULATABLEKEY_H
