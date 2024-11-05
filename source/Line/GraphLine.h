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
#include "juce_dsp/juce_dsp.h"

class DelayGraph;

struct LossModel {
    /**
     * I used this in Empy back in the day...
     This class keeps track of the packet loss. This simple two state Markov Chain model is able to emulate the loss of packets being transmitted over the internet. [1] Packets are generally lost in bursts, which is represented here by two states, a state with packet loss and a state without.

     [1] G. Hasslinger and O. Hohlfeld, "The Gilbert-Elliott Model for Packet Loss in Real Time Services on the Internet," 14th GI/ITG Conference - Measurement, Modelling and Evalutation of Computer and Communication Systems, 2008, pp. 1-15.
     */
    LossModel(float fs ) : in_loss_state(false), sampleRate(fs) {}

    bool tick()
    {
        float random = ((float) rand()) / (float) RAND_MAX;
        if (in_loss_state && (random < q)) {
            in_loss_state = false;
        } else if ((!in_loss_state) && (random < p)) {
            in_loss_state = true;
        }

        return in_loss_state;
    }

    void setParameters(float averageLossLength, float lossProbability) {
        // If there is a probabilty q of us leaving the loss each sample, we will stay
        // in the loss state, on average, for (1 - q) / q (= sum(n:0->inf)n*q*(1-q)^n)
        // samples. Solving for q, we get q = 1 / (samle_length + 1)
        float sample_length = averageLossLength * sampleRate;
        q = 1.0 / (sample_length + 1.0);
        // The eigenvector of this markov chain is [q, p], which means that we spend p/(q+p) of the time
        // in the loss state and q/(q+p) of the time in the non-loss state. Since q is set by the length of
        // loss, we need to set p to achieve the correct balance. Let r be the probability that we are in a
        // loss state, if r = p/(q+p), then p = qr/(1-r).
        if (lossProbability == 1.0) {
            p = 1.0;
        } else {
            p = std::min(1.0f, (q * lossProbability) / (1 - lossProbability));
        }
    }

    float p;
    float q;
    bool in_loss_state;
    const float sampleRate;
};

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

    std::unordered_set<GraphPoint*> realOutputs;
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
    void setStagnate (bool stagnate);

    bool isGoingBackwards() const {
        return start != nullptr && end != nullptr && (start->identifier > end->identifier);
    }
    void setBpm(float _bpm) {
        bpm = _bpm;
    }

    void setStretchTime(float newStretchTime);
private:
    float bpm;
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
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pan;

    float distortSample(unsigned channel, float samp) const;
    std::vector<float> lastSample;
    std::vector<juce::IIRFilter> loCutFilters;
    std::vector<juce::IIRFilter> hiCutFilters;
    std::unique_ptr<ModOscillator> modOscillator;

    std::vector<std::unique_ptr<LossModel>> lossmodel;
    std::vector<juce::dsp::BallisticsFilter<float>> lossFilters;
    std::vector<float> currentLossState;
    std::vector<float> sampleVal;

    std::vector<float> panLevels;
    float previousPanAmount;
};

#endif //DELAYLINES_GRAPHLINE_H
