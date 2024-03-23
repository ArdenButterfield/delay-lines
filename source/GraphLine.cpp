//
// Created by arden on 3/19/24.
//

#include "GraphLine.h"

GraphLine::GraphLine(GraphPoint* const _start, GraphPoint* const _end)
    : start(_start), end(_end)
{
    userLength = defaultLength;
    userGain = defaultGain;
    isEnabled = true;

    startTimerHz(60);
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

    numChannels = spec->numChannels;
    sampleRate = spec->sampleRate;
}

void GraphLine::setLength (float length)
{
    userLength = length;

    auto lineVector = end->getDistanceFrom(*start);
    auto realLineVector = (*end + end->offset).getDistanceFrom(*start + start->offset);
    auto currentLength = (length * sampleRate) * realLineVector / lineVector;
    currentLength = std::min(currentLength, (float)internalDelayLine.getMaximumDelayInSamples());
    currentLength = std::max(currentLength, 0.f);

    for (auto& l : lengths) {
        l.setTargetValue(currentLength);
    }
}

void GraphLine::setGain (float gain)
{
    userGain = gain;
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

void GraphLine::popSample (std::vector<float>& sample)
{
    for (unsigned channel = 0; channel < numChannels; ++channel) {
        auto length = lengths[channel].getNextValue();
        auto s = internalDelayLine.popSample(channel, length) * gains[channel].getNextValue();
        envelopeDelayLine.popSample(channel, length);
        if (isEnabled) {
            sample[channel] += s;
        }
    }
}

void GraphLine::toggleEnabled()
{
    if (isEnabled) {
        isEnabled = false;
    } else {
        isEnabled = true;
        internalDelayLine.reset();
        envelopeDelayLine.reset();
    }
}

void GraphLine::timerCallback()
{
    setLength(userLength);
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
