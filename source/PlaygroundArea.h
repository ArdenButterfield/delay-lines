//
// Created by arden on 3/14/24.
//

#ifndef DELAYLINES_PLAYGROUNDAREA_H
#define DELAYLINES_PLAYGROUNDAREA_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "DelayGraph.h"

class PlaygroundArea : public juce::Component, public juce::Timer
{
public:
    explicit PlaygroundArea(DelayGraph& dg) : delayGraph(dg) { startTimerHz(60);}
    ~PlaygroundArea() override = default;
    void resized() override;
    void paint(juce::Graphics &g) override;
private:
    void timerCallback() override;
    DelayGraph& delayGraph;
};

#endif //DELAYLINES_PLAYGROUNDAREA_H
