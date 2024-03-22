//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHLINE_H
#define DELAYLINES_GRAPHLINE_H

#include "GraphPoint.h"
#include "juce_dsp/juce_dsp.h"
#include "juce_audio_basics/juce_audio_basics.h"


class GraphLine {
public:
    GraphLine(GraphPoint* const _start, GraphPoint* const _end);

    void setLength(float length);
    void setGain(float gain);
    GraphPoint* const start;
    GraphPoint* const end;
    void pushSample(std::vector<float>& sample);
    void popSample(std::vector<float>& sample);
    void prepareToPlay(juce::dsp::ProcessSpec* spec);
    float userLength;
    float userGain;
    void toggleEnabled();
    bool isEnabled;
private:
    unsigned numChannels;
    float sampleRate;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> internalDelayLine;
    std::vector<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>> lengths;

    std::vector<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>> gains;
    const float defaultGain = 1.0;
    const float defaultLength = 1.0;
};
#endif //DELAYLINES_GRAPHLINE_H
