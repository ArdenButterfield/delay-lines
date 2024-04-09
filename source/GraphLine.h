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
    GraphLine(GraphPoint* start, GraphPoint* end, const juce::Identifier& id);

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
    void toggleEnabled();

    void getEnvelope(float proportion, float& left, float& right);

    void bakeOffset();

    struct Parameters {
        Parameters() {
            bypass = false;
            mute = false;
            length = 1000;
            lengthEnvelopeFollow = 0;
            modDepth = 0;
            modRate = 1;
            distortion = 0;
            hiCut = 20000;
            loCut = 0;
            gain = 1;
            invert = false;
            gainEnvelopeFollow = 0;
        }
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
    std::vector<GraphPoint*> popDestinations;

    bool editorAttached;

    const juce::Identifier identifier;
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

    float distortSample(float samp);

    std::vector<juce::IIRFilter> loCutFilters;
    std::vector<juce::IIRFilter> hiCutFilters;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modDepth;
    juce::dsp::Oscillator<float> modOscillator;
};
#endif //DELAYLINES_GRAPHLINE_H
