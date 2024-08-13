//
// Created by arden on 7/28/24.
//

#ifndef DELAYLINES_GRAPHPOINTCOMPONENT_H
#define DELAYLINES_GRAPHPOINTCOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"

class PlaygroundViewOld;


class GraphPointComponent : public juce::Component
{
public:
    GraphPointComponent(PlaygroundViewOld* _playgroundView);
    ~GraphPointComponent();
    void paint(juce::Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
private:
    PlaygroundViewOld* playgroundViewOld;
};


#endif //DELAYLINES_GRAPHPOINTCOMPONENT_H
