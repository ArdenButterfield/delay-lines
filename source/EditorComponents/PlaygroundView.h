//
// Created by arden on 3/14/24.
//

#ifndef DELAYLINES_PLAYGROUNDVIEW_H
#define DELAYLINES_PLAYGROUNDVIEW_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_graphics/juce_graphics.h"
#include "../DelayGraph.h"
#include "LineEditor.h"

class PlaygroundView : public juce::Component, public juce::Timer
{
public:
    explicit PlaygroundView (DelayGraph& dg) : delayGraph(dg) { startTimerHz(60);}
    ~PlaygroundView() override = default;
    void resized() override;
    void paint(juce::Graphics &g) override;
private:
    void makeLineEditorIfNeeded();
    void timerCallback() override;
    void drawLine(juce::Graphics& g, GraphLine* line);
    void drawPoint(juce::Graphics& g, GraphPoint* point);
    void drawLineBeingCreated(juce::Graphics& g);
    DelayGraph& delayGraph;
    std::unique_ptr<LineEditor> lineEditor;
};

#endif //DELAYLINES_PLAYGROUNDVIEW_H
