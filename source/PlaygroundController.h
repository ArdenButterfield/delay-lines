//
// Created by arden on 3/14/24.
//

#ifndef DELAYLINES_PLAYGROUNDCONTROLLER_H
#define DELAYLINES_PLAYGROUNDCONTROLLER_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "DelayGraph.h"

class PlaygroundController : public juce::MouseListener
{
public:
    explicit PlaygroundController(DelayGraph& dg) : delayGraph(dg) {}
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseDoubleClick(const juce::MouseEvent &event) override;

private:
    const int innerHoverDistance = 5;
    const int outerHoverDistance = 10;

    DelayGraph& delayGraph;
    void setHoveredPoint(const juce::Point<int>& mousePoint);
};

#endif //DELAYLINES_PLAYGROUNDCONTROLLER_H
