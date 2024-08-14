//
// Created by arden on 3/14/24.
//

#ifndef DELAYLINES_PLAYGROUNDCONTROLLEROLD_H
#define DELAYLINES_PLAYGROUNDCONTROLLEROLD_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "DelayGraph.h"

class PlaygroundControllerOld : public juce::MouseListener
{
public:
    explicit PlaygroundControllerOld (DelayGraph& dg) : delayGraph(dg) {}
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
    const int lineHoverDistance = 10;

    DelayGraph& delayGraph;
    void setHoveredPoint(const juce::Point<float>& mousePoint);
};

#endif //DELAYLINES_PLAYGROUNDCONTROLLEROLD_H
