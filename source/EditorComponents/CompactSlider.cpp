//
// Created by arden on 8/15/24.
//

#include "CompactSlider.h"
#include "DelayLinesLookAndFeel.h"

CompactSlider::CompactSlider()
    : suffix("")
{
    setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(valueLabel);
    valueLabel.addListener(this);
    valueLabel.setInterceptsMouseClicks(false, true);
    valueLabel.setEditable(true);
    valueLabel.setFont(
        juce::Font(DelayLinesLookAndFeel::getMonoFont()).withPointHeight(15));
}

void CompactSlider::setSuffix (const juce::String& _suffix)
{
    suffix = _suffix;
}

void CompactSlider::paint (juce::Graphics& g)
{
    if (!valueLabel.isBeingEdited()) {
        valueLabel.setText(getDisplayText(getValue()), juce::dontSendNotification);
    }
    auto val = valueToProportionOfLength(getValue());
    g.setColour(juce::Colours::black);
    g.fillRect(0, getHeight() - 4, getWidth() * val, 4);
}

void CompactSlider::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasClicked()) {
        valueLabel.showEditor();
    } else {
        juce::Slider::mouseUp(event);
    }
}

void CompactSlider::resized()
{
    juce::Slider::resized();
    valueLabel.setBounds(getLocalBounds().withTrimmedBottom(4));
    ModulatableSlider::resized();
}

void CompactSlider::editorShown (juce::Label* label, juce::TextEditor& textEditor)
{
    textEditor.setText(juce::String(getValue()));
    textEditor.setInputRestrictions(10, "-.1234567890");
    textEditor.moveCaretToEnd();
}

void CompactSlider::editorHidden (juce::Label* label, juce::TextEditor& textEditor)
{
    setValue(textEditor.getText().getDoubleValue());
}
juce::String CompactSlider::getDisplayText (double rawValue)
{
    return juce::String(rawValue) + suffix;
}