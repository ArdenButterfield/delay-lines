//
// Created by arden on 12/15/24.
//

#ifndef ECHOLOCATION_GRAPHLINEDISTORTION_H
#define ECHOLOCATION_GRAPHLINEDISTORTION_H

#include "juce_dsp/juce_dsp.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

class GraphLine;

struct GilbertElliottModel {
    /**
     This class keeps track of the packet loss. This simple two state Markov Chain model is able to emulate the loss of packets being transmitted over the internet. [1] Packets are generally lost in bursts, which is represented here by two states, a state with packet loss and a state without.

     [1] G. Hasslinger and O. Hohlfeld, "The Gilbert-Elliott Model for Packet Loss in Real Time Services on the Internet," 14th GI/ITG Conference - Measurement, Modelling and Evalutation of Computer and Communication Systems, 2008, pp. 1-15.
     */
    float p;
    float q;
    bool in_loss_state;

    GilbertElliottModel() : in_loss_state(false) {}

    void setParameters(const float probability, const float length) {
        // If there is a probabilty q of us leaving the loss each sample, we will stay
        // in the loss state, on average, for (1 - q) / q (= sum(n:0->inf)n*q*(1-q)^n)
        // samples. Solving for q, we get q = 1 / (samle_length + 1)
        q = 1.0 / (length + 1.0);

        // The eigenvector of this markov chain is [q, p], which means that we spend p/(q+p) of the time
        // in the loss state and q/(q+p) of the time in the non-loss state. Since q is set by the length of
        // loss, we need to set p to achieve the correct balance. Let r be the probability that we are in a
        // loss state, if r = p/(q+p), then p = qr/(1-r).
        if (probability == 1.0f) {
            p = 1.0;
        } else {
            p = std::min(1.0f, q * probability / (1 - probability));
        }
    }

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
};

class GraphLineDistortion : public juce::Timer
{
public:
    GraphLineDistortion(const GraphLine& l)
        : distortionAmount(0), distortionThreshold(0), distortionThresholdGain(1.0), distortionType(0), graphLine(l) {}
    ~GraphLineDistortion()
    = default;
    void prepareToplay(juce::dsp::ProcessSpec& spec);
    void setDistortionAmount(float amount);
    void setDistortionThreshold(float db);
    void setDistortionType(float type);
    void distortSample(std::vector<float>& sample);
    void paintComponent(juce::Graphics& g, juce::Component& c) const;
private:
    float distortionAmount;
    float distortionThreshold;
    float distortionThresholdGain;
    float distortionType;
    std::vector<float> previousSample;
    juce::Random random;
    juce::dsp::BallisticsFilter<float> envelope;
    juce::dsp::BallisticsFilter<float> gateEnvelope;
    juce::dsp::BallisticsFilter<float> gateTriggerEnvelope;

    void analogDistort(std::vector<float>& sample);
    void limiterDistort(std::vector<float>& sample);
    void sineDistort(std::vector<float>& sample);
    void gateDistort(std::vector<float>& sample, float chaos);

    const GraphLine& graphLine;

    float minGainReduction{}; // used for graphing
    float maxGainReduction{};
    float prevMinGainReduction{};
    float prevMaxGainReduction{};
    float sampleRate{};

    std::vector<GilbertElliottModel> lossModel;

    void timerCallback() override;
    float fs;
};

#endif //ECHOLOCATION_GRAPHLINEDISTORTION_H
