//
// Created by arden on 7/28/24.
//

#ifndef DELAYLINES_GRAPHLINECOMPONENT_H
#define DELAYLINES_GRAPHLINECOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"

class PlaygroundViewOld;
class DelayGraph;

class GraphLineComponent : public juce::Component
{
public:
    GraphLineComponent(PlaygroundViewOld* _playgroundView, DelayGraph* _delayGraph, int _id);
    ~GraphLineComponent() override;
    void paint(juce::Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
private:
    const int id;
    DelayGraph* delayGraph;
    PlaygroundViewOld* playgroundViewOld;
};

#endif //DELAYLINES_GRAPHLINECOMPONENT_H
