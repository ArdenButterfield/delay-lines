//
// Created by arden on 8/15/24.
//

#include "CompactSlider.h"
#include "../DelayLinesLookAndFeel.h"

CompactSlider::CompactSlider()
    : suffix("")
{
    setNumDecimalPlacesToDisplay(2);
    setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(valueLabel);
    valueLabel.addListener(this);
    valueLabel.setInterceptsMouseClicks(false, true);
    valueLabel.setEditable(true);
    valueLabel.setFont(
        juce::Font(DelayLinesLookAndFeel::getMonoFont()).withPointHeight(15));
    valueLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    valueLabel.setColour(juce::Label::backgroundWhenEditingColourId, juce::Colours::white.withAlpha(0.5f));
    valueLabel.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
    valueLabel.setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::black.withAlpha(0.2f));
    valueLabel.setJustificationType(juce::Justification::centred);
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
    valueLabel.setBounds(getLocalBounds().withTrimmedBottom(4));
    auto widthScalar = 1.f;
    float numCharacters = floor(std::log10(getMaximum())) + 1;
    numCharacters += getMinimum() < 0 ? 1 : 0;
    numCharacters += suffix.length();
    numCharacters += getNumDecimalPlacesToDisplay() + 1;
    auto heightConstrainer = valueLabel.getHeight() * 1.f;
    auto widthConstrainer = static_cast<float>(valueLabel.getWidth()) * 1.5f / (numCharacters);
    valueLabel.setFont(valueLabel.getFont().withHeight(std::min(heightConstrainer, widthConstrainer)));
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
    return juce::String(rawValue, getNumDecimalPlacesToDisplay()) + suffix;
}
void CompactSlider::setFontHeight (float height)
{
    valueLabel.setFont(valueLabel.getFont().withHeight(height));
}
