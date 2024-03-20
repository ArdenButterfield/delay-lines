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
    lengths.resize(spec->numChannels, juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>(defaultLength * spec->sampleRate));
    gains.resize(spec->numChannels, juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>(defaultGain));
    for (auto& l : lengths) {
        l.reset(spec->sampleRate, 0.2);
    }
    for (auto& g : gains) {
        g.reset(spec->sampleRate, 0.2);
    }
    internalDelayLine.setMaximumDelayInSamples(spec->sampleRate * 5);
    internalDelayLine.prepare(*spec);
    numChannels = spec->numChannels;
}

void GraphLine::setLength (float length)
{
    for (auto& l : lengths) {
        l.setTargetValue(length);
    }
}

void GraphLine::setGain (float gain)
{
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
        sample[channel] += internalDelayLine.popSample(channel, lengths[channel].getNextValue()) * gains[channel].getNextValue();
    }
}
