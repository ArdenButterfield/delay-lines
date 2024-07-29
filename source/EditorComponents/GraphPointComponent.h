//
// Created by arden on 7/28/24.
//

#ifndef DELAYLINES_GRAPHPOINTCOMPONENT_H
#define DELAYLINES_GRAPHPOINTCOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"

class PlaygroundView;


class GraphPointComponent : public juce::Component
{
public:
    GraphPointComponent(PlaygroundView* _playgroundView);
    ~GraphPointComponent();
    void paint(juce::Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
private:
    PlaygroundView* playgroundView;
};


#endif //DELAYLINES_GRAPHPOINTCOMPONENT_H
