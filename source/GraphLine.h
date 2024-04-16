//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHLINE_H
#define DELAYLINES_GRAPHLINE_H

#include "GraphLineParameters.h"
#include "GraphPoint.h"

#include "juce_dsp/juce_dsp.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"

class DelayGraph;

class GraphLine : public juce::Timer {
public:
    GraphLine(GraphPoint* start, GraphPoint* end, const int& id);
    GraphLine(GraphPoint* start, GraphPoint* end, juce::XmlElement* element);

    void timerCallback() override;

    void setBypass(bool bypass);
    void setMute(bool mute);

    void setLengthEnvelopeFollow(float amt);
    void setModDepth(float depth);
    void setModRate(float rate);
    void setDistortionAmount(float amt);
    void setLowCutFilter(float freq);
    void setHighCutFilter(float freq);
    void setGain(float gain);

    void setInvert(bool invert);
    void setGainEnvelopeFollow(float amt);
    void setFeedback(float amt);
    GraphPoint* start;

    GraphPoint* end;
    void pushSample(std::vector<float>& sample);
    void popSample();
    void prepareToPlay(juce::dsp::ProcessSpec* spec);
    void toggleEnabled();
    void getEnvelope(float proportion, float& left, float& right);

    void bakeOffset();

    Parameters parameters;

    std::vector<GraphPoint*> popDestinations;
    bool editorAttached;

    bool prepared;
    const int identifier;

    std::set<GraphPoint*> realOutputs;

    void exportToXml(juce::XmlElement* parent);
    bool importFromXml(DelayGraph* dg, juce::XmlElement* parent);

    std::string idToString();
    static int stringToId(std::string s);
private:
    void calculateInternalLength();

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
    std::vector<float> lastSample;
    std::vector<juce::IIRFilter> loCutFilters;
    std::vector<juce::IIRFilter> hiCutFilters;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modDepth;
    juce::dsp::Oscillator<float> modOscillator;
};
#endif //DELAYLINES_GRAPHLINE_H
