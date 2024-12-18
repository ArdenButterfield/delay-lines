//
// Created by arden on 4/18/24.
//

#ifndef DELAYLINES_DELAYLINE_H
#define DELAYLINES_DELAYLINE_H

#include "juce_dsp/juce_dsp.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"

#include "SmoothedValue.h"
#include "LengthFader.h"
#include "ModOscillator.h"

class DelayLineInternal
{
public:
    DelayLineInternal(juce::dsp::ProcessSpec spec, float initialLength, int maxLength, ModOscillator* modOscillator=nullptr);
    ~DelayLineInternal();

    void setTargetLength(float length);

    void setTick(float tickLength);

    void pushSample(std::vector<float>& sample);
    void popSample(std::vector<float>& sample, bool updateReadPointer=true);

    void getEnvelope(float proportion, float& left, float& right);

    void clearLines();

    void setStretchTime(float stretchTime);
private:
    const juce::dsp::ProcessSpec spec;
    const int envelopeDelayLineDownsampleRatio;
    int envelopeCounter;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran> delayLine;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> envelopeDelayLine;

    juce::dsp::BallisticsFilter<float> envelopeFilter;

    SmoothedValue length;
    LengthFader lengthFader;
    ModOscillator* modOscillator;
    float tickLength;
    float stretchTime;

    bool stretchTimeIsZero;
};

#endif //DELAYLINES_DELAYLINE_H
