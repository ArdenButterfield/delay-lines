//
// Created by arden on 8/15/24.
//

#ifndef DELAYLINES_COMPACTSLIDER_H
#define DELAYLINES_COMPACTSLIDER_H

#include "juce_gui_basics/juce_gui_basics.h"

class CompactSlider : public juce::Slider, public juce::Label::Listener
{
public:
    CompactSlider() : suffix("") {
        setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(valueLabel);
        valueLabel.addListener(this);
        valueLabel.setInterceptsMouseClicks(false, true);
        valueLabel.setEditable(true);
    }

    void setSuffix(const juce::String& _suffix) {
        suffix = _suffix;
    }

    void paint(juce::Graphics& g) override {
        if (!valueLabel.isBeingEdited()) {
            valueLabel.setText(juce::String(getValue()) + suffix, juce::dontSendNotification);
        }
        auto val = valueToProportionOfLength(getValue());
        g.setColour(juce::Colours::black);
        g.fillRect(0, getHeight() - 4, getWidth() * val, 4);
    }

    void mouseUp(const juce::MouseEvent& event) override {
        if (event.mouseWasClicked()) {
            valueLabel.showEditor();
        } else {
            juce::Slider::mouseUp(event);
        }
    }

    void resized() override {
        juce::Slider::resized();
        valueLabel.setBounds(getLocalBounds().withTrimmedBottom(4));
    }

    void labelTextChanged(juce::Label *labelThatHasChanged) override {}
    void editorShown(juce::Label * label, juce::TextEditor & textEditor) override {
        textEditor.setText(juce::String(getValue()));
        textEditor.setInputRestrictions(10, "-.1234567890");
    }
    void editorHidden(juce::Label * label, juce::TextEditor & textEditor) override {
        setValue(textEditor.getText().getDoubleValue());
    }
private:
    juce::String suffix;
    juce::Label valueLabel;
};

#endif //DELAYLINES_COMPACTSLIDER_H
