//
// Created by arden on 3/18/24.
//

#ifndef DELAYLINES_LINEEDITOR_H
#define DELAYLINES_LINEEDITOR_H

#include "juce_gui_basics/juce_gui_basics.h"

#include "DelayGraph.h"

class LineEditor : public juce::Component
{
public:
    LineEditor(GraphLine& graphLine);
    void resized() override;
    void paint(juce::Graphics &g) override;
private:
    GraphLine& graphLine;
    juce::Slider timeSlider;
    juce::Slider gainSlider;
};

#endif //DELAYLINES_LINEEDITOR_H
