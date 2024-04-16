//
// Created by arden on 4/15/24.
//
#include "GraphLineParameters.h"

DelayLength::DelayLength (juce::XmlElement* parent) : DelayLength()
{
    importFromXml(parent);
}


void DelayLength::exportToXml (juce::XmlElement* parent)
{
    auto delayLength = parent->createNewChildElement("delayLength");
    delayLength->setAttribute("mode", mode);
    delayLength->setAttribute("samples", samplesLength);
    delayLength->setAttribute("milliseconds", millisecondsLength);
    delayLength->setAttribute("hertz", hertz);
    delayLength->setAttribute("pitch", midiNote);
    delayLength->setAttribute("numerator", beatLength[0]);
    delayLength->setAttribute("denominator", beatLength[1]);
}

bool DelayLength::importFromXml (juce::XmlElement* parent)
{
    auto delayLength = parent->getChildByName("delayLength");
    if (delayLength) {
        mode = static_cast<Mode>(delayLength->getIntAttribute("mode"), mode);
        samplesLength = delayLength->getDoubleAttribute("samples", samplesLength);
        millisecondsLength = delayLength->getDoubleAttribute("milliseconds", millisecondsLength);
        hertz = delayLength->getDoubleAttribute("hertz", hertz);
        midiNote = delayLength->getDoubleAttribute("pitch", midiNote);
        beatLength[0] = delayLength->getDoubleAttribute("numerator", beatLength[0]);
        beatLength[1] = delayLength->getDoubleAttribute("denominator", beatLength[1]);
        return true;
    }
    return false;
}


Parameters::Parameters (juce::XmlElement* parent) : Parameters()
{
    importFromXml(parent);
}


void Parameters::exportToXml(juce::XmlElement* parent)
{
    auto element = parent->createNewChildElement("parameters");

    length.exportToXml(element);

    for (auto parameter : params) {
        element->setAttribute(parameter->getParameterID(), parameter->getValue());
    }
}

bool Parameters::importFromXml (juce::XmlElement* parent)
{
    auto paramsXml = parent->getChildByName("parameters");

    if (paramsXml) {
        length.importFromXml(paramsXml);

        for (auto parameter : params) {
            parameter->setValueNotifyingHost(parent->getDoubleAttribute(parameter->getParameterID(), parameter->getValue()));
        }
        return true;
    }
    return false;
}
