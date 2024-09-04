//
// Created by arden on 3/19/24.
//

#include "GraphLine.h"
#include "juce_core/juce_core.h"
#include "../DelayGraph.h"

GraphLine::GraphLine(GraphPoint* _start, GraphPoint* _end, const int& id)
    : start(_start),
      end(_end),
      editorAttached(false),
      prepared(false),
      identifier(id),
      numChannels(0),
      sampleRate(0)
{
    color = getRandomColour();
}

GraphLine::GraphLine (GraphPoint* _start, GraphPoint* _end, juce::XmlElement* element)
    : start(_start),
      end(_end),
      parameters(element),
      editorAttached(false),
      prepared(false),
      identifier(element->getIntAttribute("id")),
      color(juce::Colour::fromString(element->getStringAttribute("color"))),
      numChannels(0),
      sampleRate(0)

{
}

juce::Colour GraphLine::getRandomColour()
{
    auto random = juce::Random();
    return {
        random.nextFloat(),
        1.0f,
        random.nextFloat() * 0.3f + 0.7f,
        1.0f};
}


void GraphLine::prepareToPlay (juce::dsp::ProcessSpec& spec)
{
    numChannels = spec.numChannels;
    sampleRate = static_cast<float>(spec.sampleRate);

    auto targetLength = parameters.length.getLengthInSamples(sampleRate, 120);
    modOscillator = std::make_unique<ModOscillator>(spec, parameters.modRate.get(), parameters.modDepth.get());
    delayLineInternal = std::make_unique<DelayLineInternal>(spec, targetLength, spec.sampleRate * 5, modOscillator.get());

    gain.setCurrentAndTargetValue(parameters.gain);



    hiCutFilters.resize(numChannels);
    loCutFilters.resize(numChannels);

    startTimerHz(60);

    lastSample.resize(numChannels, 0);
    prepared = true;
}

void GraphLine::calculateInternalLength()
{
    if (!delayLineInternal) {
            return;
}
    auto lineVector = end->getDistanceFrom({start->x, start->y});
    auto realLineVector = (*end + end->offset).getDistanceFrom(*start + start->offset);
    auto currentLength = parameters.length.getLengthInSamples(sampleRate, 120) * realLineVector / lineVector; // TODO: BPM

    delayLineInternal->setTargetLength(currentLength);
}

void GraphLine::setGain (float _gain)
{
    parameters.gain = _gain;
    gain.setTargetValue(_gain);
}

void GraphLine::pushSample (std::vector<float>& sample)
{
    if ((!prepared) || (parameters.isMuted())) {
        return;
    }

    auto val = std::vector<float>(numChannels);

    for (unsigned channel = 0; channel < numChannels; ++channel) {
        val[channel] = sample[channel] + parameters.feedback * lastSample[channel];
    }

    delayLineInternal->pushSample(val);
}

float GraphLine::distortSample (float samp) const
{
    auto distorted = juce::dsp::FastMathApproximations::tanh(samp * parameters.distortion * 5);
    return parameters.distortion * distorted + (1 - parameters.distortion) * samp;
}

void GraphLine::popSample ()
{
    if (!prepared || (parameters.isMuted())) {
        return;
    }

    auto val = std::vector<float>(numChannels);

    delayLineInternal->popSample(val);

    auto gainVal = gain.getNextValue();
    for (unsigned channel = 0; channel < numChannels; ++channel) {

        auto s = val[channel] * gainVal;
        s = parameters.distortion > 0 ? distortSample(s) : s;
        s *= parameters.invert ? -1 : 1;

        s = parameters.loCut > 5 ? loCutFilters[channel].processSingleSampleRaw(s) : s;
        s = parameters.hiCut < 19999 ? hiCutFilters[channel].processSingleSampleRaw(s) : s;


#if false
        float gain;

        if (parameters.gainEnvelopeFollow > 0) {
            gain = (1 - parameters.gainEnvelopeFollow) + parameters.gainEnvelopeFollow * inputEnvelope;
        } else {
            gain = 1 + inputEnvelope * parameters.gainEnvelopeFollow;
        }
        s *= gain;
#endif

        if (!parameters.isBypassed()) {
            for (auto point : realOutputs) {
                point->samples[channel] += s;
            }
            lastSample[channel] = s;
        }
    }
}

void GraphLine::toggleEnabled()
{
    setMute(!parameters.isMuted());
}

void GraphLine::timerCallback()
{
    calculateInternalLength();
}

void GraphLine::getEnvelope (float proportion, float& left, float& right)
{
    if (!prepared) {
        left = 0;
        right = 0;
        return;
    }
    delayLineInternal->getEnvelope(proportion, left, right);
}
void GraphLine::setBypass (bool bypass)
{
    if (bypass) {
        parameters.muteBypass = Parameters::bypass;
    } else if (parameters.muteBypass == Parameters::bypass) {
        parameters.muteBypass = Parameters::none;
    }
}

void GraphLine::setMute (bool mute)
{
    if (mute) {
        parameters.muteBypass = Parameters::mute;
        delayLineInternal->clearLines();
    } else if (parameters.muteBypass == Parameters::mute) {
        parameters.muteBypass = Parameters::none;
    }
}

void GraphLine::setLengthEnvelopeFollow (float amt)
{
    parameters.lengthEnvelopeFollow = amt;
}

void GraphLine::setModDepth (float depth)
{
    parameters.modDepth = depth;
    modOscillator->setDepth(depth);
}

void GraphLine::setModRate (float rate)
{
    parameters.modRate = rate;
    modOscillator->setFrequency(rate);
}

void GraphLine::setDistortionAmount (float amt)
{
    parameters.distortion = amt;
}

void GraphLine::setLowCutFilter (float freq)
{
    if (!juce::approximatelyEqual(freq, (float)parameters.loCut)) {
        parameters.loCut = freq;
        for (auto& f : loCutFilters) {
            f.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, std::max(freq, 5.f)));
        }
    }
}

void GraphLine::setHighCutFilter (float freq)
{
    if (!juce::approximatelyEqual(freq, (float)parameters.loCut)) {
        parameters.hiCut = freq;
        for (auto& f : hiCutFilters) {
            f.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, std::min(freq, sampleRate / 2)));
        }
    }
}

void GraphLine::setInvert (bool invert)
{
    parameters.invert = invert;
}

void GraphLine::setGainEnvelopeFollow (float amt)
{
    parameters.gainEnvelopeFollow = amt;
}

void GraphLine::setFeedback (float amt)
{
    parameters.feedback = amt;
}

void GraphLine::setMidiTrackNote (int pitch)
{
    parameters.length.setMidiTrackNote(pitch);
}


void GraphLine::bakeOffset()
{
    auto lineVector = end->getDistanceFrom({start->x, start->y});
    auto realLineVector = (*end + end->offset).getDistanceFrom(*start + start->offset);

    parameters.length.rescale(realLineVector / lineVector);
    calculateInternalLength();
}

void GraphLine::exportToXml(juce::XmlElement* parent)
{
    auto element = parent->createNewChildElement("line");
    element->setAttribute("id", std::to_string(identifier));
    element->setAttribute("color", color.toString());
    parameters.exportToXml(element);
    element->setAttribute("start", start->identifier);
    element->setAttribute("end", end->identifier);
}

bool GraphLine::importFromXml (DelayGraph* dg, juce::XmlElement* parent)
{
    auto element = parent->getChildByAttribute("id", std::to_string(identifier));
    if (element) {
        auto colorString = element->getStringAttribute("color");
        if (colorString.isNotEmpty()) {
            color = juce::Colour::fromString(colorString);
        }
        auto _start = dg->getPoint(element->getIntAttribute("start"));
        auto _end = dg->getPoint(element->getIntAttribute("end"));
        if (_start && _end) {
            start = _start;
            end = _end;
            parameters.importFromXml(element);
            calculateInternalLength();
            return true;
        }
    }
    return false;
}

bool GraphLine::modulateIfPossible (ModulatableKey& key, float newValue)
{
    return parameters.modulateIfPossible(key, newValue);
}
float GraphLine::getCurrentModPosition()
{
    return modOscillator->getCurrentValueWithoutTicking();
}
