//
// Created by arden on 12/15/24.
//

#ifndef ECHOLOCATION_GRAPHLINEDISTORTION_H
#define ECHOLOCATION_GRAPHLINEDISTORTION_H

#include "juce_dsp/juce_dsp.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
class GraphLine;

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
    void setDistortionType(int type);
    void distortSample(std::vector<float>& sample);
    void paintComponent(juce::Graphics& g, juce::Component& c) const;
private:
    float distortionAmount;
    float distortionThreshold;
    float distortionThresholdGain;
    int distortionType;
    std::vector<float> previousSample;
    juce::Random random;
    juce::dsp::BallisticsFilter<float> envelope;
    float analogDistort(float sample) const;
    float digitalDistort(float sample) const;
    const GraphLine& graphLine;

    float minGainReduction; // used for graphing
    float maxGainReduction;
    float prevMinGainReduction;
    float prevMaxGainReduction;
    float sampleRate;

    void timerCallback() override;
};

#endif //ECHOLOCATION_GRAPHLINEDISTORTION_H
