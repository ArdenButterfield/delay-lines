//
// Created by arden on 9/3/24.
//

#ifndef DELAYLINES_MODVISUALIZER_H
#define DELAYLINES_MODVISUALIZER_H

#include "juce_gui_basics/juce_gui_basics.h"

class DelayGraph;

class ModVisualizer : public juce::Component, public juce::HighResolutionTimer
{
public:
    ModVisualizer(DelayGraph* dg, int l);
    ~ModVisualizer() override {
        stopTimer();
    }
    void paint(juce::Graphics &g) override;
    void resized() override;
    void hiResTimerCallback() override;
private:
    std::vector<float> modValues;
    unsigned bufferPosition;
    DelayGraph* delayGraph;
    int lineIndex;
};

#endif //DELAYLINES_MODVISUALIZER_H
