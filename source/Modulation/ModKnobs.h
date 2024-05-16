//
// Created by arden on 5/16/24.
//

#ifndef DELAYLINES_MODKNOBS_H
#define DELAYLINES_MODKNOBS_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "ModulationEngine.h"

class ModKnobs : public juce::Component
{
public:
    ModKnobs(ModulationEngine& modulationEngine);
private:
    void paint(juce::Graphics &g) override;
    void resized() override;
    ModulationEngine& modulationEngine;
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
};

#endif //DELAYLINES_MODKNOBS_H
