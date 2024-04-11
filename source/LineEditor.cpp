//
// Created by arden on 3/18/24.
//

#include "LineEditor.h"

LineEditor::LineEditor (DelayGraph& _delayGraph, const juce::Identifier& _line) : delayGraph(_delayGraph), graphLine(_line), lengthEditor(_delayGraph, _line)
{
    timeEnvelopeFollowSlider.setRange(-1, 1);

    modDepthSlider.setRange(0,1);

    modRateSlider.setRange(0.1,30);

    distortionSlider.setRange(0, 1);

    loCutSlider.setRange(0, 2000);

    hiCutSlider.setRange(100,20000);

    gainSlider.setRange(0,2);

    gainEnvelopeFollowSlider.setRange(-1,1);

    bypassButton.setButtonText("Bypass");

    muteButton.setButtonText("Mute");

    invertButton.setButtonText("Invert");

    feedbackSlider.setRange(0,1);

    for (auto slider : {
             &timeEnvelopeFollowSlider,
             &modDepthSlider,
             &modRateSlider,
             &distortionSlider,
             &loCutSlider,
             &hiCutSlider,
             &gainSlider,
             &gainEnvelopeFollowSlider,
             &feedbackSlider}) {
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(slider);
        slider->addListener(this);
    }

    addAndMakeVisible(lengthEditor);

    for (auto button : {
             &bypassButton,
             &muteButton,
             &invertButton
         }) {
        addAndMakeVisible(button);
        button->addListener(this);
    }

    startTimerHz(60);
    timerCallback();
}

LineEditor::~LineEditor()
{
}


void LineEditor::resized()
{
    topBar = getLocalBounds().withHeight(30);
    mainSection = getLocalBounds().withTrimmedTop(topBar.getHeight());

    panels[0] = mainSection.withWidth(mainSection.getWidth() / 3);
    panels[1] = panels[0].withX(mainSection.getWidth() / 3);
    panels[2] = mainSection.withTrimmedLeft(panels[1].getRight());

    lengthEditor.setBounds(panels[0].withHeight(panels[0].getWidth()));
    timeEnvelopeFollowSlider.setBounds(panels[0].withTop(lengthEditor.getBottom()).withHeight(50));

    auto modZone = panels[0].withTop(timeEnvelopeFollowSlider.getBottom());

    modRateSlider.setBounds(modZone.withWidth(modZone.getWidth() / 2));
    modDepthSlider.setBounds(modZone.withLeft(modRateSlider.getRight()));

    distortionSlider.setBounds(panels[1].withHeight(panels[1].getWidth()));

    auto filterZone = panels[1].withTop(distortionSlider.getBottom());
    hiCutSlider.setBounds(filterZone.withHeight(filterZone.getHeight() / 2));
    loCutSlider.setBounds(filterZone.withTop(hiCutSlider.getBottom()));

    gainSlider.setBounds(panels[2].withHeight(panels[2].getWidth()));

    invertButton.setBounds(panels[2].withTop(gainSlider.getBottom()).withHeight(50));

    auto bottomOfPanelTwo = panels[2].withTop(invertButton.getBottom());

    gainEnvelopeFollowSlider.setBounds(bottomOfPanelTwo.withWidth(bottomOfPanelTwo.getWidth() / 2));
    feedbackSlider.setBounds(bottomOfPanelTwo.withLeft(gainEnvelopeFollowSlider.getRight()));

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
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }
    if (slider == &gainSlider) {
        line->setGain(gainSlider.getValue());
    } else if (slider == &timeEnvelopeFollowSlider) {
        line->setLengthEnvelopeFollow(timeEnvelopeFollowSlider.getValue());
    } else if (slider == &modRateSlider) {
        line->setModRate(modRateSlider.getValue());
    } else if (slider == &modDepthSlider) {
        line->setModDepth (modDepthSlider.getValue());
    } else if (slider == &distortionSlider) {
        line->setDistortionAmount(distortionSlider.getValue());
    } else if (slider == &loCutSlider) {
        line->setLowCutFilter(loCutSlider.getValue());
    } else if (slider == &hiCutSlider) {
        line->setHighCutFilter(hiCutSlider.getValue());
    } else if (slider == &gainEnvelopeFollowSlider) {
        line->setGainEnvelopeFollow(gainEnvelopeFollowSlider.getValue());
    } else if (slider == &feedbackSlider) {
        line->setFeedback(feedbackSlider.getValue());
    }
}

void LineEditor::timerCallback()
{
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }
    timeEnvelopeFollowSlider.setValue(line->parameters.lengthEnvelopeFollow, juce::dontSendNotification);
    modDepthSlider.setValue(line->parameters.modDepth, juce::dontSendNotification);
    modRateSlider.setValue(line->parameters.modRate, juce::dontSendNotification);
    distortionSlider.setValue(line->parameters.distortion, juce::dontSendNotification);
    loCutSlider.setValue(line->parameters.loCut, juce::dontSendNotification);
    hiCutSlider.setValue(line->parameters.hiCut, juce::dontSendNotification);
    gainSlider.setValue(line->parameters.gain, juce::dontSendNotification);
    feedbackSlider.setValue(line->parameters.feedback, juce::dontSendNotification);
    invertButton.setToggleState(line->parameters.invert, juce::dontSendNotification);
    muteButton.setToggleState(line->parameters.isMuted(), juce::dontSendNotification);
    bypassButton.setToggleState(line->parameters.isBypassed(), juce::dontSendNotification);
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
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }
    if (button == &muteButton) {
        line->setMute(muteButton.getToggleState());
    } else if (button == &bypassButton) {
        line->setBypass(bypassButton.getToggleState());
        delayGraph.setRealOutputs();
    } else if (button == &invertButton) {
        line->setInvert(invertButton.getToggleState());
    }
}
