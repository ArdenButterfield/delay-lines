//
// Created by arden on 5/16/24.
//

#include "ModKnobs.h"
#include "ModulationMappingEngine.h"
#include "ModulationEngine.h"

ModKnobs::ModKnobs()
{
}

ModKnobs::~ModKnobs()
{
    mappingEngine->removeListener(this);
}

void ModKnobs::setMappingEngine (ModulationMappingEngine* me)
{
    if (!sliders.empty()) {
        for (auto& slider : sliders) {
            removeChildComponent(slider.get());
        }
        for (auto& mapButton : mapButtons) {
            removeChildComponent(mapButton.get());
        }
        sliders.clear();
        attachments.clear();
    }

    mappingEngine = me;

    mappingEngine->addListener(this);

    if (mappingEngine->modulationEngine != nullptr) {
        for (const auto& id : mappingEngine->modulationEngine->paramIds) {
            sliders.push_back(std::make_unique<juce::Slider>());
            attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                mappingEngine->modulationEngine->treeState,
                id,
                *sliders.back()
                    ));
            mapButtons.push_back(std::make_unique<juce::ToggleButton>("map " + id));
            addAndMakeVisible(*mapButtons.back());
            mapButtons.back()->addListener(this);
            addAndMakeVisible(*sliders.back());

            sliders.back()->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
            sliders.back()->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        }
    }
}


void ModKnobs::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::aqua);
}

void ModKnobs::resized()
{
    for (unsigned i = 0; i < sliders.size(); ++i) {
        auto sliderArea = getLocalBounds().withWidth(getWidth() / sliders.size()).withX(getWidth() * i / sliders.size());
        sliders[i]->setBounds(sliderArea.withHeight(getHeight() - 30));
        mapButtons[i]->setBounds(sliderArea.withTrimmedTop(getHeight() - 30));
    }
}
void ModKnobs::buttonStateChanged (juce::Button*)
{

}

void ModKnobs::buttonClicked (juce::Button* b)
{
    for (unsigned i = 0; i < mapButtons.size(); ++i) {
        if (mapButtons[i].get() == b) {
            if (mapButtons[i]->getToggleState()) {
                mappingEngine->enterModMappingMode(i);
                for (auto& button : mapButtons) {
                    if (button.get() != b) {
                        button->setToggleState(false, juce::dontSendNotification);
                    }
                }
            } else {
                mappingEngine->exitModMappingMode();
            }
        }
    }
}
void ModKnobs::mappingModeEntered()
{

}
void ModKnobs::mappingModeExited()
{
    for (auto& button : mapButtons) {
        button->setToggleState(false, juce::dontSendNotification);
    }
}