//
// Created by arden on 8/13/24.
//

#ifndef DELAYLINES_PLAYGROUNDINTERFACE_H
#define DELAYLINES_PLAYGROUNDINTERFACE_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "../../DelayGraph.h"
#include "GraphLineComponent.h"
#include "GraphPointComponent.h"
#include "../../Modulation/ModulationMappingEngine.h"
#include <set>

class PlaygroundInterface : public juce::Component, public DelayGraph::Listener, public juce::Timer
{
public:
    explicit PlaygroundInterface(ModulationMappingEngine& me, DelayGraph& _delayGraph);
    ~PlaygroundInterface() override;
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
    juce::Point<float> gridOffset;

    ModulationMappingEngine& mappingEngine;

    void paintBackground(juce::Graphics &g);

    void applyGlobalOffset();
    juce::Point<float> globalOffset;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    DelayGraph& delayGraph;
    std::set<std::unique_ptr<GraphLineComponent>> lineComponents;
    std::set<std::unique_ptr<GraphPointComponent>> pointComponents;

};

#endif //DELAYLINES_PLAYGROUNDINTERFACE_H
