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

class GraphLine : public juce::Timer, juce::AudioProcessorParameter::Listener {
public:
    GraphLine(GraphPoint* start, GraphPoint* end, const int& id);
    GraphLine(GraphPoint* start, GraphPoint* end, juce::XmlElement* element);

    void timerCallback() override;

    void setInvert(bool invert);
    void setGainEnvelopeFollow(float amt);
    void setFeedback(float amt);
    void setMidiTrackNote(int pitch);

    float getCurrentModPosition();

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
    bool getModulationValue(ModulatableKey& key, float& val);

    juce::Colour getColor() { return color; }

    void clear();
    const ModOscillator* getModOscillator() {
        return modOscillator.get();
    }

    void setBypass (bool bypass);
    void setMute (bool mute);
private:
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
    static juce::Colour getRandomColour();

    juce::Colour color;

    std::unique_ptr<DelayLineInternal> delayLineInternal;

    void calculateInternalLength();
    void recalculateParameters();
    unsigned numChannels;
    float sampleRate;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;

    float distortSample(float samp) const;
    std::vector<float> lastSample;
    std::vector<juce::IIRFilter> loCutFilters;
    std::vector<juce::IIRFilter> hiCutFilters;
    std::unique_ptr<ModOscillator> modOscillator;
};

#endif //DELAYLINES_GRAPHLINE_H
