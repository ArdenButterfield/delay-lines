//
// Created by arden on 4/10/24.
//

#ifndef DELAYLINES_LENGTHEDITOR_H
#define DELAYLINES_LENGTHEDITOR_H

#include "juce_gui_basics/juce_gui_basics.h"

#include "../DelayGraph.h"
#include "../Line/GraphLine.h"

#include "CompactSlider.h"
#include "PitchCompactSlider.h"

#include "BpmTapper.h"

class LengthEditor : public juce::Component, public juce::Timer, public juce::ComboBox::Listener, public juce::Slider::Listener, public BpmTapper::Listener
{
public:
    LengthEditor(DelayGraph& delayGraph, const int& line);

    void resized() override;
    void paint(juce::Graphics &g) override;
    void timerCallback() override;
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void sliderValueChanged(juce::Slider *slider) override;
private:
    void tempoSet(juce::int64 interval) override;

    void updateSliders();
    void updateUnitSelector();

    juce::ComboBox unitSelector;

    BpmTapper bpmTapper;

    CompactSlider samplesSlider;
    CompactSlider millisecondsSlider;
    CompactSlider hertzSlider;
    CompactSlider noteSlider;
    PitchCompactSlider pitchSlider;

    CompactSlider beatNumerator;
    CompactSlider beatDenominator;

    DelayGraph& delayGraph;
    const int& graphLine;
};

#endif //DELAYLINES_LENGTHEDITOR_H
