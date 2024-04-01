//
// Created by arden on 3/19/24.
//

#include "GraphLine.h"

GraphLine::GraphLine(GraphPoint* const _start, GraphPoint* const _end)
    : start(_start), end(_end)
{

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
    internalDelayLine.setMaximumDelayInSamples(spec->sampleRate * 5);
    envelopeDelayLine.setMaximumDelayInSamples(spec->sampleRate * 5);

    internalDelayLine.prepare(*spec);
    envelopeDelayLine.prepare(*spec);
    envelopeFilter.prepare(*spec);

    hiCutFilters.resize(spec->numChannels);
    loCutFilters.resize(spec->numChannels);

    numChannels = spec->numChannels;
    sampleRate = spec->sampleRate;

    startTimerHz(60);
}

void GraphLine::setLength (float length)
{
    parameters.length = length;

    auto lineVector = end->getDistanceFrom(*start);
    auto realLineVector = (*end + end->offset).getDistanceFrom(*start + start->offset);
    auto currentLength = (length * sampleRate / 1000) * realLineVector / lineVector;
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
    for (unsigned channel = 0; channel < numChannels; ++channel) {
        internalDelayLine.pushSample(channel, sample[channel]);
        auto envelope = envelopeFilter.processSample(channel, sample[channel]);
        envelopeDelayLine.pushSample(channel, envelope);
    }
}

float GraphLine::distortSample (float samp)
{
    auto distorted = juce::dsp::FastMathApproximations::tanh(samp * parameters.distortion * 5);
    return parameters.distortion * distorted + (1 - parameters.distortion) * samp;
}

void GraphLine::popSample (std::vector<float>& sample)
{
    for (unsigned channel = 0; channel < numChannels; ++channel) {

        auto length = std::min(lengths[channel].getNextValue(), (float)internalDelayLine.getMaximumDelayInSamples() - 1);
        auto s = internalDelayLine.popSample(channel, length) * gains[channel].getNextValue();
        s = parameters.distortion ? distortSample(s) : s;
        s *= parameters.invert ? -1 : 1;

        s = parameters.loCut > 5 ? loCutFilters[channel].processSingleSampleRaw(s) : s;
        s = parameters.hiCut < 19999 ? hiCutFilters[channel].processSingleSampleRaw(s) : s;

        envelopeDelayLine.popSample(channel, length);
        if (!parameters.mute) {
            sample[channel] += s;
        }
    }
}

void GraphLine::toggleEnabled()
{
    setMute(!parameters.mute);
}

void GraphLine::timerCallback()
{
    setLength(parameters.length);
}

void GraphLine::getEnvelope (float proportion, float& left, float& right)
{
    left = envelopeDelayLine.popSample(0, internalDelayLine.getDelay() * proportion, false);
    if (numChannels > 1) {
        right = envelopeDelayLine.popSample(1, internalDelayLine.getDelay() * proportion, false);
    } else {
        right = left;
    }
}
void GraphLine::setBypass (bool bypass)
{
    parameters.bypass = bypass;
}

void GraphLine::setMute (bool mute)
{
    if (parameters.mute && !mute) {
        internalDelayLine.reset();
        envelopeDelayLine.reset();
    }
    parameters.mute = mute;
}

void GraphLine::setLengthEnvelopeFollow (float amt)
{
    parameters.lengthEnvelopeFollow = amt;
}

void GraphLine::setModDepth (float depth)
{
    parameters.modDepth = depth;
}

void GraphLine::setModRate (float rate)
{
    parameters.modRate = rate;
}

void GraphLine::setDistortionAmount (float amt)
{
    parameters.distortion = amt;
}

void GraphLine::setLowCutFilter (float freq)
{
    if (freq != parameters.loCut) {
        parameters.loCut = freq;
        for (auto& f : loCutFilters) {
            f.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, std::max(freq, 5.f)));
        }
    }
}

void GraphLine::setHighCutFilter (float freq)
{
    if (freq != parameters.hiCut) {
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
