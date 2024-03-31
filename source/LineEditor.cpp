//
// Created by arden on 3/18/24.
//

#include "LineEditor.h"

LineEditor::LineEditor(GraphLine& _graphLine) : graphLine(_graphLine)
{
    timeSlider.setRange(0, 5000);
    timeSlider.setValue(graphLine.userLength);

    timeEnvelopeFollowSlider.setRange(-1, 1);

    modDepthSlider.setRange(0,1);

    modRateSlider.setRange(0.1,30);

    distortionSlider.setRange(0, 100);

    loCutSlider.setRange(0, 2000);

    hiCutSlider.setRange(100,20000);

    gainSlider.setRange(0,2);
    gainSlider.setValue(graphLine.userGain);

    gainEnvelopeFollowSlider.setRange(-1,1);

    bypassButton.setButtonText("Bypass");

    muteButton.setButtonText("Mute");

    invertButton.setButtonText("Invert");


    for (auto slider : {
             &timeSlider,
             &timeEnvelopeFollowSlider,
             &modDepthSlider,
             &modRateSlider,
             &distortionSlider,
             &loCutSlider,
             &hiCutSlider,
             &gainSlider,
             &gainEnvelopeFollowSlider}) {
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(slider);
        slider->addListener(this);
    }

    for (auto button : {
             &bypassButton,
             &muteButton,
             &invertButton
         }) {
        addAndMakeVisible(button);
        button->addListener(this);
    }

    startTimerHz(60);
}

void LineEditor::resized()
{
    topBar = getLocalBounds().withHeight(30);
    mainSection = getLocalBounds().withTrimmedTop(topBar.getHeight());

    panels[0] = mainSection.withWidth(mainSection.getWidth() / 3);
    panels[1] = panels[0].withX(mainSection.getWidth() / 3);
    panels[2] = mainSection.withTrimmedLeft(panels[1].getRight());

    timeSlider.setBounds(panels[0].withHeight(panels[0].getWidth()));
    timeEnvelopeFollowSlider.setBounds(panels[0].withTop(timeSlider.getBottom()).withHeight(50));

    auto modZone = panels[0].withTop(timeEnvelopeFollowSlider.getBottom());

    modRateSlider.setBounds(modZone.withWidth(modZone.getWidth() / 2));
    modDepthSlider.setBounds(modZone.withLeft(modRateSlider.getRight()));

    distortionSlider.setBounds(panels[1].withHeight(panels[1].getWidth()));

    auto filterZone = panels[1].withTop(distortionSlider.getBottom());
    hiCutSlider.setBounds(filterZone.withHeight(filterZone.getHeight() / 2));
    loCutSlider.setBounds(filterZone.withTop(hiCutSlider.getBottom()));

    gainSlider.setBounds(panels[2].withHeight(panels[2].getWidth()));

    invertButton.setBounds(panels[2].withTop(gainSlider.getBottom()).withHeight(50));
    gainEnvelopeFollowSlider.setBounds(panels[2].withTop(invertButton.getBottom()));

    bypassButton.setBounds(topBar);
    bypassButton.changeWidthToFitText();
    muteButton.setBounds(topBar.withLeft(bypassButton.getRight()));
    muteButton.changeWidthToFitText();
}

void LineEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::red);
    g.fillAll();
    g.setColour(juce::Colours::brown);
    g.fillRect(topBar);
    g.setColour(juce::Colours::orangered);
    g.fillRect(panels[1]);
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 3);
}

void LineEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &timeSlider) {
        graphLine.setLength(timeSlider.getValue());
    } else if (slider == &gainSlider) {
        graphLine.setGain(gainSlider.getValue());
    } else if (slider == &timeEnvelopeFollowSlider) {
    }
}
void LineEditor::timerCallback()
{
    timeSlider.setValue(graphLine.parameters.length);
    timeEnvelopeFollowSlider.setValue(graphLine.parameters.lengthEnvelopeFollow);
    modDepthSlider.setValue(graphLine.parameters.modDepth);
    modRateSlider.setValue(graphLine.parameters.modRate);
    distortionSlider.setValue(graphLine.parameters.distortion);
    loCutSlider.setValue(graphLine.parameters.loCut);
    hiCutSlider.setValue(graphLine.parameters.hiCut);
    gainSlider.setValue(graphLine.parameters.gain);
    invertButton.setToggleState(graphLine.parameters.invert, juce::dontSendNotification);
    muteButton.setToggleState(graphLine.parameters.mute, juce::dontSendNotification);
    bypassButton.setToggleState(graphLine.parameters.bypass, juce::dontSendNotification);
}
void LineEditor::mouseDown (const juce::MouseEvent& event)
{
    if (topBar.contains(event.getPosition())) {
        dragger.startDraggingComponent(this, event);
        dragging = true;
    }
}

void LineEditor::mouseDrag (const juce::MouseEvent& event)
{
    if (dragging) {
        dragger.dragComponent (this, event, nullptr);
    }
}

void LineEditor::mouseUp (const juce::MouseEvent& event)
{
    dragging = false;
}
void LineEditor::buttonStateChanged (juce::Button*)
{
    // gets called on every mouse over
}
void LineEditor::buttonClicked (juce::Button* button)
{
    if (button == &muteButton) {
        graphLine.setMute(muteButton.getToggleState());
    } else if (button == &bypassButton) {
        graphLine.setBypass(bypassButton.getToggleState());
    } else if (button == &invertButton) {
        graphLine.setInvert(invertButton.getToggleState());
    }
}
