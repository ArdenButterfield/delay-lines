//
// Created by arden on 5/16/24.
//

#ifndef DELAYLINES_MODKNOBS_H
#define DELAYLINES_MODKNOBS_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "ModulationMappingEngine.h"

class ModKnobs : public juce::Component, public juce::Button::Listener
{
public:
    ModKnobs();
    void setMappingEngine(ModulationMappingEngine* me);
private:
    void paint(juce::Graphics &g) override;
    void resized() override;
    ModulationMappingEngine* mappingEngine;
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::ToggleButton>> mapButtons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
    void buttonStateChanged(juce::Button *) override;
    void buttonClicked(juce::Button *) override;
};

#endif //DELAYLINES_MODKNOBS_H
