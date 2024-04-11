//
// Created by arden on 4/10/24.
//

#ifndef DELAYLINES_LENGTHEDITOR_H
#define DELAYLINES_LENGTHEDITOR_H

#include "juce_gui_basics/juce_gui_basics.h"

#include "DelayGraph.h"
#include "GraphLine.h"

class LengthEditor : public juce::Component, public juce::Timer, public juce::ComboBox::Listener, public juce::Slider::Listener
{
public:
    LengthEditor(DelayGraph& delayGraph, const juce::Identifier& line);

    void resized() override;
    void paint(juce::Graphics &g) override;
    void timerCallback() override;
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void sliderValueChanged(juce::Slider *slider) override;
private:
    juce::ComboBox unitSelector;

    juce::Slider samplesSlider;
    juce::Slider millisecondsSlider;
    juce::Slider hertzSlider;
    juce::Slider noteSlider;

    juce::TextEditor beatNumerator;
    juce::TextEditor beatDenominator;

    DelayGraph& delayGraph;
    const juce::Identifier& graphLine;
};

#endif //DELAYLINES_LENGTHEDITOR_H
