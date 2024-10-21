//
// Created by arden on 10/21/24.
//

#include "ModKnob.h"
#include "ModulationEngine.h"

ModKnob::ModKnob (ModulationMappingEngine* me, juce::String paramId, const int _paramIndex) : mappingEngine(me), paramIndex(_paramIndex)
{
    mappingEngine->addListener(this);
    slider = std::make_unique<juce::Slider>();
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        mappingEngine->modulationEngine->treeState,
        paramId,
        *slider);

    mapButton = std::make_unique<juce::ToggleButton>("map " + paramId);

    mapButton->addListener(this);

    addAndMakeVisible(mapButton.get());
    addAndMakeVisible(slider.get());

    slider->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

}
ModKnob::~ModKnob()
{
    mappingEngine->removeListener(this);
}
void ModKnob::paint (juce::Graphics& g)
{

}
void ModKnob::resized()
{
    slider->setBounds(getLocalBounds().withHeight(getHeight() - 30));
    mapButton->setBounds(getLocalBounds().withTrimmedTop(getHeight() - 30));
}
void ModKnob::buttonStateChanged (juce::Button*)
{

}
void ModKnob::buttonClicked (juce::Button*)
{
    if (mapButton->getToggleState()) {
        mappingEngine->enterModMappingMode(paramIndex);
    } else {
        mappingEngine->exitModMappingMode();
    }
}
void ModKnob::mappingModeEntered (int parameterIndex)
{
    if (parameterIndex != paramIndex) {
        mapButton->setToggleState(false, juce::dontSendNotification);
    }
}
void ModKnob::mappingModeExited()
{
    mapButton->setToggleState(false, juce::dontSendNotification);
}
