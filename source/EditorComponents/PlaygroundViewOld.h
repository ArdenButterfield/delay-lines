//
// Created by arden on 3/14/24.
//

#ifndef DELAYLINES_PLAYGROUNDVIEWOLD_H
#define DELAYLINES_PLAYGROUNDVIEWOLD_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_graphics/juce_graphics.h"
#include "../DelayGraph.h"
#include "LineEditor.h"
#include "../Modulation/ModOverlayButton.h"

#include "GraphLineComponent.h"
#include "GraphPointComponent.h"

class PlaygroundViewOld : public juce::Component, public juce::Timer
{
public:
    explicit PlaygroundViewOld (DelayGraph& dg);
    ~PlaygroundViewOld() override = default;
    void resized() override;
    void paint(juce::Graphics &g) override;
    const std::unique_ptr<LineEditor>* getLineEditor();
private:
    void makeLineEditorIfNeeded();
    void timerCallback() override;
    void drawLine(juce::Graphics& g, GraphLine* line) const;
    void drawPoint(juce::Graphics& g, GraphPoint* point) const;
    void drawLineBeingCreated(juce::Graphics& g) const;
    DelayGraph& delayGraph;
    std::unique_ptr<LineEditor> lineEditor;

    ModOverlayButton overlayButton;
};

#endif //DELAYLINES_PLAYGROUNDVIEWOLD_H
