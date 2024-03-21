//
// Created by arden on 3/18/24.
//

#include "LineEditor.h"

LineEditor::LineEditor(GraphLine& _graphLine) : graphLine(_graphLine)
{
    timeSlider.setRange(0, 5);
    gainSlider.setRange(0,2);

    timeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

    timeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);

    timeSlider.setValue(graphLine.userLength);
    gainSlider.setValue(graphLine.userGain);

    addAndMakeVisible(timeSlider);
    addAndMakeVisible(gainSlider);
    
    timeSlider.addListener(this);
    gainSlider.addListener(this);

    startTimerHz(60);
}

void LineEditor::resized()
{
    timeSlider.setBounds(getLocalBounds().withWidth(getWidth() / 2));
    gainSlider.setBounds(getLocalBounds().withTrimmedLeft(getWidth() / 2));
}

void LineEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::red);
    g.fillAll();
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 3);
}

void LineEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &timeSlider) {
        graphLine.setLength(timeSlider.getValue());
    } else if (slider == &gainSlider) {
        graphLine.setGain(gainSlider.getValue());
    }
}
void LineEditor::timerCallback()
{
    timeSlider.setValue(graphLine.userLength);
    gainSlider.setValue(graphLine.userGain);
}
