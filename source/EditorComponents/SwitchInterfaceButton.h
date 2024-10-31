//
// Created by arden on 10/30/24.
//

#ifndef DELAYLINES_SWITCHINTERFACEBUTTON_H
#define DELAYLINES_SWITCHINTERFACEBUTTON_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_dsp/juce_dsp.h"

class SwitchInterfaceButton : public juce::ToggleButton, public juce::Timer
{
public:
    SwitchInterfaceButton() : juce::ToggleButton("switch interface") {
        startTimerHz(60);
        filter.prepare({60, 16, 1});
        filter.setAttackTime(100);
        filter.setReleaseTime(100);
    }
    ~SwitchInterfaceButton() override = default;
private:
    void resized() override {
        filter.reset(getBallisticsInput());
        position = getBallisticsInput();
        innerSpace = getLocalBounds().toFloat().reduced(4);
    }
    float getBallisticsInput() {
        return getToggleState() ? 1.f : 0.f;
    }
    void timerCallback() override {
        position = filter.processSample(0, getBallisticsInput());
        repaint();
    }
    void paintButton(juce::Graphics & g, bool, bool) override;
    juce::dsp::BallisticsFilter<float> filter;
    float position;
    juce::Rectangle<float> innerSpace;
};

#endif //DELAYLINES_SWITCHINTERFACEBUTTON_H
