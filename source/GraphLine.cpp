//
// Created by arden on 3/19/24.
//

#include "GraphLine.h"
#include "juce_core/juce_core.h"

GraphLine::GraphLine(GraphPoint* const _start, GraphPoint* const _end, const juce::Identifier& id)
    : start(_start), end(_end), identifier(id)
{
    modOscillator.initialise([] (float x) {return std::sin(x);}, 128);
    prepared = false;

}

void GraphLine::prepareToPlay (juce::dsp::ProcessSpec* spec)
{
    lengths.resize(spec->numChannels, juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>(defaultLength * spec->sampleRate));
    gains.resize(spec->numChannels, juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>(defaultGain));
    for (auto& l : lengths) {
        l.reset(spec->sampleRate, 0.2);
    }
    for (auto& g : gains) {
        g.reset(spec->sampleRate, 0.2);
    }
    modDepth.reset(512);
    numChannels = spec->numChannels;
    sampleRate = spec->sampleRate;

    internalDelayLine.setMaximumDelayInSamples(spec->sampleRate * 5);
    envelopeDelayLine.setMaximumDelayInSamples(spec->sampleRate * 5);

    internalDelayLine.prepare(*spec);
    envelopeDelayLine.prepare(*spec);
    envelopeFilter.prepare(*spec);

    hiCutFilters.resize(spec->numChannels);
    loCutFilters.resize(spec->numChannels);

    modOscillator.prepare(*spec);

    startTimerHz(60);

    lastSample.resize(spec->numChannels, 0);
    prepared = true;
}

void GraphLine::calculateInternalLength()
{
    auto lineVector = end->getDistanceFrom(*start);
    auto realLineVector = (*end + end->offset).getDistanceFrom(*start + start->offset);
    auto currentLength = parameters.length.getLengthInSamples(sampleRate, 120) * realLineVector / lineVector; // TODO: BPM
    currentLength = std::min(currentLength, (float)internalDelayLine.getMaximumDelayInSamples() - 1);
    currentLength = std::max(currentLength, 0.f);
    for (auto& l : lengths) {
        l.setTargetValue(currentLength);
    }
}

void GraphLine::setGain (float gain)
{
    parameters.gain = gain;
    for (auto& g : gains) {
        g.setTargetValue(gain);
    }
}

void GraphLine::pushSample (std::vector<float>& sample)
{
    if ((!prepared) || (parameters.isMuted())) {
        return;
    }

    for (unsigned channel = 0; channel < numChannels; ++channel) {
        auto val = sample[channel] + parameters.feedback * lastSample[channel];
        internalDelayLine.pushSample(static_cast<int>(channel), val);
        auto envelope = envelopeFilter.processSample(static_cast<int>(channel), val);
        envelopeDelayLine.pushSample(static_cast<int>(channel), envelope);
    }
}

float GraphLine::distortSample (float samp)
{
    auto distorted = juce::dsp::FastMathApproximations::tanh(samp * parameters.distortion * 5);
    return parameters.distortion * distorted + (1 - parameters.distortion) * samp;
}

void GraphLine::popSample ()
{
    if (!prepared || (parameters.isMuted())) {
        return;
    }

    auto mod = 1 + modDepth.getNextValue() * modOscillator.processSample(0) / (parameters.modRate * juce::MathConstants<float>::twoPi * 10);

    for (unsigned channel = 0; channel < numChannels; ++channel) {

        auto length = std::min(lengths[channel].getNextValue() * mod, (float)internalDelayLine.getMaximumDelayInSamples() - 1);
        auto s = internalDelayLine.popSample(channel, length) * gains[channel].getNextValue();
        s = parameters.distortion ? distortSample(s) : s;
        s *= parameters.invert ? -1 : 1;

        s = parameters.loCut > 5 ? loCutFilters[channel].processSingleSampleRaw(s) : s;
        s = parameters.hiCut < 19999 ? hiCutFilters[channel].processSingleSampleRaw(s) : s;

        envelopeDelayLine.popSample(channel, length);
        auto inputEnvelope = envelopeDelayLine.popSample(static_cast<int>(channel), 0, false);

        float gain;
        if (parameters.gainEnvelopeFollow > 0) {
            gain = (1 - parameters.gainEnvelopeFollow) + parameters.gainEnvelopeFollow * inputEnvelope;
        } else {
            gain = 1 + inputEnvelope * parameters.gainEnvelopeFollow;
        }
        s *= gain;
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

    left = envelopeDelayLine.popSample(0, internalDelayLine.getDelay() * proportion, false);
    if (numChannels > 1) {
        right = envelopeDelayLine.popSample(1, internalDelayLine.getDelay() * proportion, false);
    } else {
        right = left;
    }
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
        envelopeDelayLine.reset();
        internalDelayLine.reset();
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
    modDepth.setTargetValue(depth);
}

void GraphLine::setModRate (float rate)
{
    parameters.modRate = rate;
    modOscillator.setFrequency(rate);
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

void GraphLine::bakeOffset()
{
    auto lineVector = end->getDistanceFrom(*start);
    auto realLineVector = (*end + end->offset).getDistanceFrom(*start + start->offset);

    parameters.length.rescale(realLineVector / lineVector);
    calculateInternalLength();
}
