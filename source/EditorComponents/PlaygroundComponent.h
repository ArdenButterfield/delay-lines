//
// Created by arden on 8/13/24.
//

#ifndef DELAYLINES_PLAYGROUNDCOMPONENT_H
#define DELAYLINES_PLAYGROUNDCOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "../DelayGraph.h"
#include "GraphLineComponent.h"
#include "GraphPointComponent.h"

#include <set>

class PlaygroundComponent : public juce::Component, public DelayGraph::Listener, public juce::Timer
{
public:
    explicit PlaygroundComponent(DelayGraph& _delayGraph);
    ~PlaygroundComponent() override;
    void resized() override;
    void paint(juce::Graphics &g) override;

    void pointAdded(int identifier) override { addPoint(identifier); }
    void pointRemoved(int identifier) override { removePoint(identifier); }
    void lineAdded(int identifier) override { addLine(identifier); }
    void lineRemoved(int identifier) override { removeLine(identifier); }

    void addPoint(int identifier);
    void removePoint(int identifier);
    void addLine(int identifier);
    void removeLine(int identifier);

    void timerCallback() override { repaint(); }
    juce::Point<float> getGlobalOffset() { return globalOffset; }
private:
    void applyGlobalOffset();
    juce::Point<float> globalOffset;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    DelayGraph& delayGraph;
    std::set<std::unique_ptr<GraphLineComponent>> lineComponents;
    std::set<std::unique_ptr<GraphPointComponent>> pointComponents;
};

#endif //DELAYLINES_PLAYGROUNDCOMPONENT_H
