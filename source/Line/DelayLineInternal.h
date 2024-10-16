//
// Created by arden on 4/18/24.
//

#ifndef DELAYLINES_DELAYLINE_H
#define DELAYLINES_DELAYLINE_H

#include "juce_dsp/juce_dsp.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"

#include "ModOscillator.h"

class DelayLineInternal
{
public:
    DelayLineInternal(juce::dsp::ProcessSpec spec, float initialLength, int maxLength, ModOscillator* modOscillator=nullptr);
    ~DelayLineInternal();

    void setTargetLength(float length);

    void setTick(float tickLength);

    void pushSample(std::vector<float>& sample);
    void popSample(std::vector<float>& sample);

    void getEnvelope(float proportion, float& left, float& right);

    void clearLines();
private:
    const juce::dsp::ProcessSpec spec;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> envelopeDelayLine;
    juce::dsp::BallisticsFilter<float> envelopeFilter;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> length;
    ModOscillator* modOscillator;
    float tickLength;

};

#endif //DELAYLINES_DELAYLINE_H
