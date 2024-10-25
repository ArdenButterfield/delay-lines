//
// Created by arden on 4/15/24.
//
#include "GraphLineParameters.h"
#include "../parameters.h"

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
        mode = static_cast<Mode>(delayLength->getIntAttribute("mode", 2));
        samplesLength = static_cast<float>(delayLength->getDoubleAttribute("samples", samplesLength));
        millisecondsLength = static_cast<float>(delayLength->getDoubleAttribute("milliseconds", millisecondsLength));
        hertz = static_cast<float>(delayLength->getDoubleAttribute("hertz", hertz));
        midiNote = static_cast<float>(delayLength->getDoubleAttribute("pitch", midiNote));
        beatLength[0] = static_cast<float>(delayLength->getDoubleAttribute("numerator", beatLength[0]));
        beatLength[1] = static_cast<float>(delayLength->getDoubleAttribute("denominator", beatLength[1]));
        return true;
    }
    return false;
}

Parameters::Parameters() : muteBypass(MUTE_BYPASS_ID, "mute bypass", {"none", "mute", "bypass", "stagnate"}, 0),
                           lengthEnvelopeFollow(LENGTH_ENVELOPE_FOLLOW_ID, "length envelope follow", -1.f, 1.f, 0.f),
                           modDepth(MOD_DEPTH_ID, "mod depth", 0.f, 1.f, 0.f),
                           modRate(MOD_RATE_ID, "mod rate", 0.1f, 30.f, 1.f),
                           distortion(DISTORTION_ID, "distortion", 0.f, 1.f, 0.f),
                           distortionType(DISTORTION_TYPE_ID, "distortion type", DISTORTION_TYPE_OPTIONS, 0),
                           hiCut(HICUT_ID, "hi cut", 0.f, 20000.f, 20000.f),
                           loCut(LOCUT_ID, "lo cut", 0.f, 20000.f, 0.f),
                           gain(GAIN_ID, "gain", 0.f, 2.f, 1.f),
                           invert(INVERT_ID, "invert", false),
                           gainEnvelopeFollow(GAIN_ENVELOPE_FOLLOW_ID, "gain envelope follow", -1.f, 1.f, 0.f),
                           feedback(FEEDBACK_ID, "feedback", 0.f, 120.f, 0.f)
{}


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
            parameter->setValueNotifyingHost(static_cast<float>(paramsXml->getDoubleAttribute(parameter->getParameterID(), parameter->getValue())));
        }
        return true;
    }
    return false;
}

bool Parameters::modulateIfPossible (ModulatableKey& key, float newValue)
{
    for (auto param : params) {
        if (key.parameterId == juce::Identifier(param->getParameterID())) {
            param->setValueNotifyingHost(newValue);
            return true;
        }
    }
    return false;
}

bool Parameters::getModulationValue (ModulatableKey& key, float& val)
{
    for (auto param : params) {
        if (key.parameterId == juce::Identifier(param->getParameterID())) {
            val = param->getValue();
            return true;
        }
    }
    return false;
}

