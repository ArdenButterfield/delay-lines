//
// Created by arden on 5/3/24.
//

#ifndef DELAYLINES_MODOVERLAYBUTTON_H
#define DELAYLINES_MODOVERLAYBUTTON_H

#include <utility>

#include "juce_gui_basics/juce_gui_basics.h"
#include "ModulatableKey.h"
#include "ModulationMappingEngine.h"


class ModOverlayButton : public juce::Component, public ModulationMappingEngine::Listener
{
public:
    ModOverlayButton(ModulationMappingEngine& me, ModulatableKey  key);

    ~ModOverlayButton() override;

    void mappingModeEntered(int parameterId) override;

    void mappingModeExited() override;

    ModulationMappingEngine& mappingEngine;
    ModulatableKey modKey;

    void paint(juce::Graphics &g) override;

    void resized() override;

    void mouseUp(const juce::MouseEvent &event) override;

private:
    const juce::Colour bgColor = juce::Colours::black;
};

#endif //DELAYLINES_MODOVERLAYBUTTON_H
