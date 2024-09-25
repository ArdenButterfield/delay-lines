//
// Created by arden on 5/16/24.
//

#ifndef DELAYLINES_MODKNOBS_H
#define DELAYLINES_MODKNOBS_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "ModulationMappingEngine.h"

class ModKnobs : public juce::Component, public juce::Button::Listener, ModulationMappingEngine::Listener
{
public:
    ModKnobs();
    ~ModKnobs() override;
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
    void mappingModeEntered() override;
    void mappingModeExited() override;
};

#endif //DELAYLINES_MODKNOBS_H
