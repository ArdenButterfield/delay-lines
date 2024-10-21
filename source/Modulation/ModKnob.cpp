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
    unmapButton = std::make_unique<juce::TextButton>("unmap");
    mapButton->addListener(this);
    unmapButton->addListener(this);

    addChildComponent(unmapButton.get());

    addAndMakeVisible(mapButton.get());
    addAndMakeVisible(slider.get());

    slider->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    startTimerHz(30);
}
ModKnob::~ModKnob()
{
    mappingEngine->removeListener(this);
}
void ModKnob::paint (juce::Graphics& g)
{
    if (mappingEngine->modulationEngine->isMapped(paramIndex)) {
        g.fillAll(juce::Colours::darkgoldenrod);
    }
}
void ModKnob::resized()
{
    slider->setBounds(getLocalBounds().withHeight(getHeight() - 30));
    mapButton->setBounds(getLocalBounds().withTrimmedTop(getHeight() - 30));
    unmapButton->setBounds(mapButton->getBounds());
}
void ModKnob::buttonStateChanged (juce::Button*)
{

}
void ModKnob::buttonClicked (juce::Button* b)
{
    if (b == mapButton.get()) {
        if (mapButton->getToggleState()) {
            mappingEngine->enterModMappingMode(paramIndex);
        } else {
            mappingEngine->exitModMappingMode();
        }
    } else if (b == unmapButton.get()) {
        mappingEngine->modulationEngine->clearMapping(paramIndex);
    }
    repaint();
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
void ModKnob::timerCallback()
{
    auto mapped = mappingEngine->modulationEngine->isMapped(paramIndex);

    mapButton->setVisible(!mapped);
    unmapButton->setVisible(mapped);
    repaint();
}
