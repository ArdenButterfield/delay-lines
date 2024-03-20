//
// Created by arden on 3/19/24.
//

#include "GraphLine.h"

GraphLine::GraphLine(const GraphPoint* _start, const GraphPoint* _end)
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
float GraphLine::processSample (float sample)
{
    return 0;
}
