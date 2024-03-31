//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHLINE_H
#define DELAYLINES_GRAPHLINE_H

#include "GraphPoint.h"
#include "juce_dsp/juce_dsp.h"
#include "juce_audio_basics/juce_audio_basics.h"


class GraphLine : public juce::Timer {
public:
    GraphLine(GraphPoint* start, GraphPoint* end);

    void timerCallback() override;

    void setBypass(bool bypass);
    void setMute(bool mute);

    void setLength(float length);
    void setLengthEnvelopeFollow(float amt);
    void setModDepth(float depth);
    void setModRate(float rate);
    void setDistortionAmount(float amt);
    void setLowCutFilter(float freq);
    void setHighCutFilter(float freq);

    void setGain(float gain);
    void setInvert(bool invert);
    void setGainEnvelopeFollow(float amt);




    GraphPoint* const start;
    GraphPoint* const end;
    void pushSample(std::vector<float>& sample);
    void popSample(std::vector<float>& sample);
    void prepareToPlay(juce::dsp::ProcessSpec* spec);
    float userLength;
    float userGain;
    void toggleEnabled();
    bool isEnabled;

    void getEnvelope(float proportion, float& left, float& right);
    struct Parameters {
        bool bypass;
        bool mute;
        float length;
        float lengthEnvelopeFollow;
        float modDepth;
        float modRate;
        float distortion;
        float hiCut;
        float loCut;
        float gain;
        bool invert;
        float gainEnvelopeFollow;
    };

    Parameters parameters;

private:

    unsigned numChannels;
    float sampleRate;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> internalDelayLine;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> envelopeDelayLine;
    juce::dsp::BallisticsFilter<float> envelopeFilter;
    std::vector<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>> lengths;

    std::vector<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>> gains;
    const float defaultGain = 1.0;
    const float defaultLength = 1.0;
};
#endif //DELAYLINES_GRAPHLINE_H
