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
    internalDelayLine.prepare(*spec);
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
    }
}

void GraphLine::popSample (std::vector<float>& sample)
{
    for (unsigned channel = 0; channel < numChannels; ++channel) {
        auto s = internalDelayLine.popSample(channel, lengths[channel].getNextValue()) * gains[channel].getNextValue();
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
    }
}
void GraphLine::timerCallback()
{
    setLength(userLength);
}
