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
      sampleRate(0),
      bpm(120)
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

    lossmodel.clear();
    lossFilters.clear();
    currentLossState.clear();
    for (unsigned i = 0; i < spec.numChannels; ++i) {
        lossmodel.push_back(std::make_unique<LossModel>(spec.sampleRate));
        lossmodel.back()->setParameters(0.1, 0);
        lossFilters.emplace_back();
        lossFilters.back().prepare({spec.sampleRate, spec.maximumBlockSize, 1});
        lossFilters.back().setAttackTime(0.5);
        lossFilters.back().setReleaseTime(10);
        currentLossState.push_back(0);
    }

    hiCutFilters.resize(numChannels);
    loCutFilters.resize(numChannels);

    startTimerHz(60);

    lastSample.resize(numChannels, 0);
    prepared = true;
}

void GraphLine::calculateInternalLength()
{
    if (!delayLineInternal || start == nullptr || end == nullptr || start == end) {
        return;
    }
    auto lineVector = end->getDistanceFrom({start->x, start->y});
    if (juce::approximatelyEqual(lineVector, 0.f)) {
        return;
    }
    auto realLineVector = (*end + end->offset).getDistanceFrom(*start + start->offset);
    auto currentLength = parameters.length.getLengthInSamples(sampleRate, bpm) * realLineVector / lineVector;

    if (parameters.length.getMode() == parameters.length.beat) {
        delayLineInternal->setTick(parameters.length.getLengthOfDenominator(sampleRate, bpm));
    } else {
        delayLineInternal->setTick(0);
    }

    delayLineInternal->setTargetLength(currentLength);
}

void GraphLine::pushSample (std::vector<float>& sample)
{
    if ((!prepared) || (parameters.isMuted()) || (parameters.isStagnated())) {
        return;
    }

    auto val = std::vector<float>(numChannels);

    for (unsigned channel = 0; channel < numChannels; ++channel) {
        val[channel] = sample[channel] + parameters.feedback * lastSample[channel];
    }

    delayLineInternal->pushSample(val);
}

float GraphLine::distortSample (unsigned channel, float samp) const
{
    float wet, multiplier;
    float distortionAmount = parameters.distortion;
    if (juce::approximatelyEqual(distortionAmount, 0.f)) {
        return samp;
    }

    switch (parameters.distortionType.getIndex())
    {
        case 0:
            // analog clip
            wet = juce::dsp::FastMathApproximations::tanh(samp * distortionAmount * 5);
            return distortionAmount * wet + (1 - distortionAmount) * samp;
        case 1:
            // digital clip
            wet = std::max(std::min(samp * 1 + distortionAmount, 1.f), -1.f);
            return distortionAmount * wet + (1 - distortionAmount) * samp;
        case 2:
            // wavefold
            wet = sin(samp * (distortionAmount * 5 + 1));
            return distortionAmount * wet + (1 - distortionAmount) * samp;
        case 3:
            // bitcrush
            multiplier = std::pow(2.f, 16 - 16 * distortionAmount);
            return std::round(samp * multiplier) / multiplier;
        case 4:
            // packet loss
            return currentLossState[channel] * samp;
        case 5:
            // packet loss stereo
            return currentLossState[channel] * samp;
        default:
            return samp;
    }
}

void GraphLine::popSample ()
{
    if (!prepared || (parameters.isMuted())) {
        return;
    }

    auto val = std::vector<float>(numChannels);

    delayLineInternal->popSample(val, !parameters.isStagnated());

    auto gainVal = gain.getNextValue();
    if (parameters.distortionType.getIndex() == 4) {
        // mono
        auto v = lossmodel[0]->tick() ? 0.f : 1.f;
        for (unsigned channel = 0; channel < numChannels; ++channel) {
            currentLossState[channel] = lossFilters[channel].processSample(0,v);
        }
    } else if (parameters.distortionType.getIndex() == 5) {
        for (unsigned channel = 0; channel < numChannels; ++channel) {
            auto v = lossmodel[channel]->tick() ? 0.f : 1.f;
            currentLossState[channel] = lossFilters[channel].processSample(0,v);
        }
    }
    for (unsigned channel = 0; channel < numChannels; ++channel) {
        auto s = val[channel] * gainVal;
        if (!std::isfinite(s)) {
            s = 0;
        }
        s = parameters.distortion > 0 ? distortSample(channel, s) : s;
        s *= parameters.invert ? -1 : 1;

        s = parameters.loCut > 5 ? loCutFilters[channel].processSingleSampleRaw(s) : s;
        s = parameters.hiCut < 19999 ? hiCutFilters[channel].processSingleSampleRaw(s) : s;

        s = std::min(std::max(s, -5.f), 5.f);

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
    recalculateParameters();
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
    } else if (parameters.muteBypass == Parameters::mute) {
        parameters.muteBypass = Parameters::none;
    }
}

void GraphLine::setStagnate (bool stagnate) {
    if (stagnate) {
        parameters.muteBypass = Parameters::stagnate;
    } else if (parameters.muteBypass == Parameters::stagnate) {
        parameters.muteBypass = Parameters::none;
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

bool GraphLine::getModulationValue (ModulatableKey& key, float& val)
{
    return parameters.getModulationValue(key, val);
}


float GraphLine::getCurrentModPosition()
{
    return modOscillator->getCurrentValueWithoutTicking();
}

void GraphLine::clear()
{
    delayLineInternal->clearLines();
}

void GraphLine::parameterValueChanged (int parameterIndex, float newValue)
{
    recalculateParameters();
}

void GraphLine::recalculateParameters()
{
    gain.setTargetValue(parameters.gain);
    modOscillator->setDepth(parameters.modDepth);
    modOscillator->setFrequency(parameters.modRate);

    for (auto& f : loCutFilters) {
        f.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate,
            std::min(std::max((float)parameters.loCut, 5.f), sampleRate / 2)));
    }

    for (auto& f : hiCutFilters) {
        f.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate,
            std::min(std::max((float)parameters.hiCut, 5.f), sampleRate / 2)));
    }

    if (parameters.isMuted()) {
        delayLineInternal->clearLines();
    }

    for (auto& model : lossmodel) {
        model->setParameters(0.1, parameters.distortion);
    }
}
void GraphLine::setStretchTime(float newStretchTime)
{
    delayLineInternal->setStretchTime(newStretchTime);
}
