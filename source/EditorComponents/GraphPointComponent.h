//
// Created by arden on 7/28/24.
//

#ifndef DELAYLINES_GRAPHPOINTCOMPONENT_H
#define DELAYLINES_GRAPHPOINTCOMPONENT_H

#include "../DelayGraph.h"
#include "juce_gui_basics/juce_gui_basics.h"
class PlaygroundComponent;

class LineGhostComponent : public juce::Component
{
public:
    LineGhostComponent(juce::Point<float> _start, juce::Point<float> _end) : start(_start), end(_end) {}
    void paint(juce::Graphics &g) override {
        g.setColour(juce::Colours::green);
        g.drawLine({start, end}, 2.0);
    }
    juce::Point<float> start, end;
};

class GraphPointComponent : public juce::Component
{
public:
    GraphPointComponent(DelayGraph& delayGraph, PlaygroundComponent* _playgroundComponent, int id);
    ~GraphPointComponent() override {}
    void paint(juce::Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
    int getIdentifier() { return identifier; }
private:
    DelayGraph& delayGraph;
    PlaygroundComponent* playgroundComponent;
    int identifier;
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseDoubleClick(const juce::MouseEvent &event) override;
    void updateFocus(const juce::Point<float>& mousePoint);
    std::unique_ptr<LineGhostComponent> ghostComponent;
    const int innerHoverDistance = 5;
    const int outerHoverDistance = 10;
};


#endif //DELAYLINES_GRAPHPOINTCOMPONENT_H