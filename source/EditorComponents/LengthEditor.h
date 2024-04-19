//
// Created by arden on 4/10/24.
//

#ifndef DELAYLINES_LENGTHEDITOR_H
#define DELAYLINES_LENGTHEDITOR_H

#include "juce_gui_basics/juce_gui_basics.h"

#include "../DelayGraph.h"
#include "../Line/GraphLine.h"

class LengthEditor : public juce::Component, public juce::Timer, public juce::ComboBox::Listener, public juce::Slider::Listener
{
public:
    LengthEditor(DelayGraph& delayGraph, const int& line);

    void resized() override;
    void paint(juce::Graphics &g) override;
    void timerCallback() override;
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void sliderValueChanged(juce::Slider *slider) override;
private:
    void updateSliders();
    void updateUnitSelector();

    juce::ComboBox unitSelector;

    juce::Slider samplesSlider;
    juce::Slider millisecondsSlider;
    juce::Slider hertzSlider;
    juce::Slider noteSlider;
    juce::Slider pitchSlider;

    juce::Slider beatNumerator;
    juce::Slider beatDenominator;

    DelayGraph& delayGraph;
    const int& graphLine;
};

#endif //DELAYLINES_LENGTHEDITOR_H
