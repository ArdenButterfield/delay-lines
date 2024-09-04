//
// Created by arden on 3/18/24.
//

#include "LineEditor.h"

LineEditor::LineEditor (DelayGraph& _delayGraph, const int& _line)
    : lengthEditor(_delayGraph, _line), delayGraph(_delayGraph), graphLine(_line), dragging(false), modVisualizer(&_delayGraph, _line)
{
    setLookAndFeel(&delayLinesLookAndFeel);
    timeEnvelopeFollowSlider.setRange(-1, 1);

    modDepthSlider.setRange(0,1);

    auto modrateRange = juce::NormalisableRange<double>(0.1, 30);
    modrateRange.setSkewForCentre(3);
    modRateSlider.setNormalisableRange(modrateRange);

    distortionSlider.setRange(0, 1);

    loCutSlider.setRange(0, 2000);

    hiCutSlider.setRange(100,20000);

    gainSlider.setRange(0,2);
    gainSlider.setSuffix(" dB");

    gainEnvelopeFollowSlider.setRange(-1,1);

    bypassButton.setButtonText("Bypass");

    muteButton.setButtonText("Mute");

    invertButton.setButtonText("Invert");

    feedbackSlider.setRange(0,1);
    feedbackSlider.setSuffix(" %");

    for (auto slider : {
             &timeEnvelopeFollowSlider,
             &distortionSlider,
             &gainEnvelopeFollowSlider}) {
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(slider);
        slider->addListener(this);
    }

    addAndMakeVisible(modVisualizer);
    addAndMakeVisible(filterVisualizer);

    for (auto slider : {
             &gainSlider,
             &feedbackSlider,
             &loCutSlider,
             &hiCutSlider,
             &modDepthSlider,
             &modRateSlider}) {
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

    gainLabel.setText("GAIN", juce::dontSendNotification);
    feedbackLabel.setText("FEEDBACK", juce::dontSendNotification);
    modLabel.setText("MOD", juce::dontSendNotification);
    modDepthLabel.setText("depth", juce::dontSendNotification);
    modRateLabel.setText("rate", juce::dontSendNotification);
    filterLabel.setText("FILTER", juce::dontSendNotification);
    filterLoLabel.setText("lo", juce::dontSendNotification);
    filterHiLabel.setText("hi", juce::dontSendNotification);

    for (auto label : {
             &gainLabel,
             &feedbackLabel,
             &modLabel,
             &modDepthLabel,
             &modRateLabel,
             &filterLabel,
             &filterLoLabel,
             &filterHiLabel
         }) {
        addAndMakeVisible(label);
    }

    startTimerHz(60);
}

LineEditor::~LineEditor()
= default;


void LineEditor::resized()
{
    topBar = getLocalBounds().withHeight(30);
    mainSection = getLocalBounds().withTrimmedTop(topBar.getHeight());

    bypassButton.setBounds(topBar);
    bypassButton.changeWidthToFitText();
    muteButton.setBounds(topBar.withLeft(bypassButton.getRight()));
    muteButton.changeWidthToFitText();

    lengthEditor.setBounds(mainSection.withHeight(40));

    auto gainAndFeedbackArea = mainSection.withTrimmedTop(lengthEditor.getHeight()).withHeight(50);
    auto gainArea = gainAndFeedbackArea.withWidth(gainAndFeedbackArea.getWidth() / 2);
    auto feedbackArea = gainAndFeedbackArea.withTrimmedLeft(gainArea.getRight());

    gainSlider.setBounds(gainArea.withHeight(40));
    feedbackSlider.setBounds(feedbackArea.withHeight(40));
    gainLabel.setBounds(gainArea.withTrimmedTop(gainSlider.getHeight()));
    feedbackLabel.setBounds(feedbackArea.withTrimmedTop(feedbackSlider.getHeight()));

    modArea = mainSection.withTrimmedTop(gainAndFeedbackArea.getBottom()).withHeight(80);

    modVisualizer.setBounds(modArea);

    auto thirdWidth = modArea.getWidth() / 3;
    modDepthLabel.setBounds(modArea.withWidth(thirdWidth).withHeight(10));
    modDepthSlider.setBounds(modArea.withWidth(thirdWidth).withTrimmedTop(modDepthLabel.getHeight()));

    modRateLabel.setBounds(modArea.withWidth(thirdWidth).withRightX(modArea.getRight()).withHeight(10));
    modRateSlider.setBounds(modArea.withWidth(thirdWidth).withRightX(modArea.getRight()).withTrimmedTop(modRateLabel.getHeight()));

    modLabel.setBounds(modArea.withX(modDepthSlider.getRight()).withRight(modRateSlider.getX()));

    filterArea = mainSection.withTrimmedTop(modArea.getBottom()).withHeight(80);

    filterVisualizer.setBounds(filterArea);

    thirdWidth = modArea.getWidth() / 3;
    filterLoLabel.setBounds(filterArea.withWidth(thirdWidth).withHeight(10));
    loCutSlider.setBounds(filterArea.withWidth(thirdWidth).withTrimmedTop(filterLoLabel.getHeight()));

    filterHiLabel.setBounds(filterArea.withWidth(thirdWidth).withRightX(filterArea.getRight()).withHeight(10));
    hiCutSlider.setBounds(filterArea.withWidth(thirdWidth).withRightX(filterArea.getRight()).withTrimmedTop(filterHiLabel.getHeight()));

    filterLabel.setBounds(filterArea.withX(loCutSlider.getRight()).withRight(hiCutSlider.getX()));


//    panels[0] = mainSection.withWidth(mainSection.getWidth() / 3);
//    panels[1] = panels[0].withX(mainSection.getWidth() / 3);
//    panels[2] = mainSection.withTrimmedLeft(panels[1].getRight());
//
//    lengthEditor.setBounds(panels[0].withHeight(panels[0].getWidth()));
//    timeEnvelopeFollowSlider.setBounds(panels[0].withTop(lengthEditor.getBottom()).withHeight(50));
//
//    auto modZone = panels[0].withTop(timeEnvelopeFollowSlider.getBottom());
//
//    modRateSlider.setBounds(modZone.withWidth(modZone.getWidth() / 2));
//    modDepthSlider.setBounds(modZone.withLeft(modRateSlider.getRight()));
//
//    distortionSlider.setBounds(panels[1].withHeight(panels[1].getWidth()));
//
//    auto filterZone = panels[1].withTop(distortionSlider.getBottom());
//    hiCutSlider.setBounds(filterZone.withHeight(filterZone.getHeight() / 2));
//    loCutSlider.setBounds(filterZone.withTop(hiCutSlider.getBottom()));
//
//    gainSlider.setBounds(panels[2].withHeight(panels[2].getWidth()));
//
//    invertButton.setBounds(panels[2].withTop(gainSlider.getBottom()).withHeight(50));
//
//    auto bottomOfPanelTwo = panels[2].withTop(invertButton.getBottom());
//
//    gainEnvelopeFollowSlider.setBounds(bottomOfPanelTwo.withWidth(bottomOfPanelTwo.getWidth() / 2));
//    feedbackSlider.setBounds(bottomOfPanelTwo.withLeft(gainEnvelopeFollowSlider.getRight()));

}

void LineEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::red);
    g.fillAll();
    g.setColour(juce::Colours::brown);
    auto line = delayGraph.getLine(graphLine);
    if (line) {
        g.setColour (line->getColor());
    }
    g.fillRect(topBar);
    g.setColour(juce::Colours::orangered);
    g.fillRect(panels[1]);
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 3);
//    g.drawRect(modArea, 1);
//    for (auto child : getChildren()) {
//        g.drawRect(child->getBounds());
//    }
}

void LineEditor::sliderValueChanged (juce::Slider* slider)
{
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }
    if (slider == &gainSlider) {
        line->setGain(static_cast<float>(gainSlider.getValue()));
    } else if (slider == &timeEnvelopeFollowSlider) {
        line->setLengthEnvelopeFollow(static_cast<float>(timeEnvelopeFollowSlider.getValue()));
    } else if (slider == &modRateSlider) {
        line->setModRate(static_cast<float>(modRateSlider.getValue()));
    } else if (slider == &modDepthSlider) {
        line->setModDepth (static_cast<float>(modDepthSlider.getValue()));
    } else if (slider == &distortionSlider) {
        line->setDistortionAmount(static_cast<float>(distortionSlider.getValue()));
    } else if (slider == &loCutSlider) {
        line->setLowCutFilter(static_cast<float>(loCutSlider.getValue()));
    } else if (slider == &hiCutSlider) {
        line->setHighCutFilter(static_cast<float>(hiCutSlider.getValue()));
    } else if (slider == &gainEnvelopeFollowSlider) {
        line->setGainEnvelopeFollow(static_cast<float>(gainEnvelopeFollowSlider.getValue()));
    } else if (slider == &feedbackSlider) {
        line->setFeedback(static_cast<float>(feedbackSlider.getValue()));
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
    juce::ignoreUnused(event);
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
