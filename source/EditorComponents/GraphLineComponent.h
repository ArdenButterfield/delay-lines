//
// Created by arden on 7/28/24.
//

#ifndef DELAYLINES_GRAPHLINECOMPONENT_H
#define DELAYLINES_GRAPHLINECOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"

class DelayGraph;
class PlaygroundComponent;

class GraphLineComponent : public juce::Component
{
public:
    GraphLineComponent(DelayGraph& _delayGraph, PlaygroundComponent* _playgroundComponent, int _id);
    ~GraphLineComponent() override;
    void paint(juce::Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
    int getIdentifier() const { return id; }
private:
    const int id;
    DelayGraph& delayGraph;
    PlaygroundComponent* playgroundComponent;

    const int lineHoverDistance = 10;
};

#endif //DELAYLINES_GRAPHLINECOMPONENT_H
