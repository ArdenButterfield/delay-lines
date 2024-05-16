//
// Created by arden on 4/19/24.
//

#ifndef DELAYLINES_MODULATOROVERLAY_H
#define DELAYLINES_MODULATOROVERLAY_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "PlaygroundView.h"


class ModulatorOverlay : public juce::Component
{
public:
    ModulatorOverlay(PlaygroundView& playgroundView);
    ~ModulatorOverlay() override;
    void resized() override;
    void paint(juce::Graphics &g) override;
private:
    PlaygroundView& playgroundView;
};

#endif //DELAYLINES_MODULATOROVERLAY_H
