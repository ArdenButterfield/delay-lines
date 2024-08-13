//
// Created by arden on 7/28/24.
//

#ifndef DELAYLINES_GRAPHPOINTCOMPONENT_H
#define DELAYLINES_GRAPHPOINTCOMPONENT_H

#include "../DelayGraph.h"
#include "juce_gui_basics/juce_gui_basics.h"

class PlaygroundComponent;

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
};


#endif //DELAYLINES_GRAPHPOINTCOMPONENT_H
