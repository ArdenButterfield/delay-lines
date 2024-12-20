//
// Created by arden on 10/8/24.
//

#ifndef DELAYLINES_DISTORTIONVISUALIZER_H
#define DELAYLINES_DISTORTIONVISUALIZER_H

#include "../../Line/GraphLineDistortion.h"
#include "../../DelayGraph.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_basics/juce_gui_basics.h"

class DistortionVisualizer : public juce::Component
{
public:
    DistortionVisualizer(DelayGraph& delayGraph, const int& line);
    ~DistortionVisualizer() override;
    void setDistortion(int type, float amount);
    float opacity;
private:
    void fillValues();
    void paint(juce::Graphics &g) override;
    void resized() override;
    int distortionType;
    float distortionAmount;
    std::array<std::vector<float>, 2> values;
    DelayGraph& delayGraph;
    const int line;
};

#endif //DELAYLINES_DISTORTIONVISUALIZER_H
