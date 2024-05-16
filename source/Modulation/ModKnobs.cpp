//
// Created by arden on 5/16/24.
//

#include "ModKnobs.h"

ModKnobs::ModKnobs(ModulationEngine& me) : modulationEngine(me)
{
    for (const auto& id : modulationEngine.paramIds) {
        sliders.push_back(std::make_unique<juce::Slider>());
        attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            modulationEngine.treeState,
            id,
            *sliders.back()
            ));
        addAndMakeVisible(*sliders.back());
        sliders.back()->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        sliders.back()->setTextBoxStyle(juce::Slider::TextBoxRight, false, 30, 15);
    }
}

void ModKnobs::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::aqua);
}

void ModKnobs::resized()
{
    for (auto i = 0; i < sliders.size(); ++i) {
        sliders[i]->setBounds(getWidth() * i / sliders.size(), 0, getWidth() / sliders.size(), getHeight());
    }
}