//
// Created by arden on 10/21/24.
//

#ifndef DELAYLINES_MODKNOB_H
#define DELAYLINES_MODKNOB_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "ModulationMappingEngine.h"

class ModKnob : public juce::Component, public juce::Button::Listener, ModulationMappingEngine::Listener, juce::Timer
{
public:
    ModKnob(ModulationMappingEngine* me, juce::String paramId, const int _paramIndex);
    ~ModKnob() override;
private:
    void paint(juce::Graphics &g) override;
    void resized() override;
    ModulationMappingEngine* mappingEngine;
    std::unique_ptr<juce::Slider> slider;
    std::unique_ptr<juce::ToggleButton> mapButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    void buttonStateChanged(juce::Button *) override;
    void buttonClicked(juce::Button *) override;
    void mappingModeEntered(int parameterIndex) override;
    void mappingModeExited() override;

    void timerCallback() override;

    std::unique_ptr<juce::TextButton> unmapButton;

    const int paramIndex;

    juce::Label mappingLabel;
};

#endif //DELAYLINES_MODKNOB_H
