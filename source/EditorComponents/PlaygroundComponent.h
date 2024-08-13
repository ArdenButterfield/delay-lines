//
// Created by arden on 8/13/24.
//

#ifndef DELAYLINES_PLAYGROUNDCOMPONENT_H
#define DELAYLINES_PLAYGROUNDCOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"

class PlaygroundComponent : public juce::Component
{
public:
    PlaygroundComponent();
    ~PlaygroundComponent() override;
    void resized() override;
    void paint(juce::Graphics &g) override;
private:
};

#endif //DELAYLINES_PLAYGROUNDCOMPONENT_H
