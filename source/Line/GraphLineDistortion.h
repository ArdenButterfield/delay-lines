//
// Created by arden on 12/15/24.
//

#ifndef ECHOLOCATION_GRAPHLINEDISTORTION_H
#define ECHOLOCATION_GRAPHLINEDISTORTION_H

#include "juce_dsp/juce_dsp.h"

class GraphLineDistortion
{
public:
    GraphLineDistortion()
        : distortionAmount(0), distortionThreshold(0), distortionThresholdGain(1.0), distortionType(0) {}
    ~GraphLineDistortion()
    = default;
    void prepareToplay(juce::dsp::ProcessSpec& spec);
    void setDistortionAmount(float amount);
    void setDistortionThreshold(float db);
    void setDistortionType(int type);
    void distortSample(std::vector<float>& sample);
private:
    float distortionAmount;
    float distortionThreshold;
    float distortionThresholdGain;
    int distortionType;
    std::vector<float> previousSample;
    juce::Random random;
};

#endif //ECHOLOCATION_GRAPHLINEDISTORTION_H
