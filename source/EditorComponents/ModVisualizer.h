//
// Created by arden on 9/3/24.
//

#ifndef DELAYLINES_MODVISUALIZER_H
#define DELAYLINES_MODVISUALIZER_H

#include "juce_gui_basics/juce_gui_basics.h"

class DelayGraph;

class ModVisualizer : public juce::Component, public juce::Timer
{
public:
    ModVisualizer(DelayGraph* dg, int l);
    ~ModVisualizer() override {
        stopTimer();
    }
    void paint(juce::Graphics &g) override;
    void resized() override;
    void timerCallback() override;
private:
    DelayGraph* delayGraph;
    int lineIndex;
    std::vector<float> lows, highs;
};

#endif //DELAYLINES_MODVISUALIZER_H
