//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHLINE_H
#define DELAYLINES_GRAPHLINE_H

#include "GraphLineParameters.h"
#include "../GraphPoint.h"
#include "../Modulation/ModulatableKey.h"
#include "DelayLineInternal.h"

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
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
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
    bool modulateIfPossible(ModulatableKey& key, float newValue);
private:
    std::unique_ptr<DelayLineInternal> delayLineInternal;

    void calculateInternalLength();

    unsigned numChannels;
    float sampleRate;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;

    float distortSample(float samp);
    std::vector<float> lastSample;
    std::vector<juce::IIRFilter> loCutFilters;
    std::vector<juce::IIRFilter> hiCutFilters;
    std::unique_ptr<ModOscillator> modOscillator;
};
#endif //DELAYLINES_GRAPHLINE_H
