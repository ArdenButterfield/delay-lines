//
// Created by arden on 3/18/24.
//

#include "LineEditor.h"
#include "../parameters.h"
LineEditor::LineEditor (ModulationMappingEngine& me, DelayGraph& _delayGraph, const int& _line)
    : lengthEditor(_delayGraph, _line), delayGraph(_delayGraph), graphLine(_line), dragging(false), modVisualizer(&_delayGraph, _line), mappingEngine(me)
{
    setLookAndFeel(&delayLinesLookAndFeel);
    timeEnvelopeFollowSlider.setRange(-1, 1);

    modDepthSlider.setRange(0,1);

    auto modrateRange = juce::NormalisableRange<double>(0.1, 30);
    modrateRange.setSkewForCentre(3);
    modRateSlider.setNormalisableRange(modrateRange);

    distortionSlider.setRange(0, 1);

    auto filterRange = juce::NormalisableRange<double>(0,20000);
    filterRange.setSkewForCentre(500);
    loCutSlider.setNormalisableRange(filterRange);
    hiCutSlider.setNormalisableRange(filterRange);

    gainSlider.setRange(0,2);
    gainSlider.setSuffix(" dB");

    gainEnvelopeFollowSlider.setRange(-1,1);

    bypassButton.setButtonText("Bypass");

    muteButton.setButtonText("Mute");

    invertButton.setButtonText("Invert");

    copyButton.setButtonText("Copy");
    pasteButton.setButtonText("Paste");

    feedbackSlider.setRange(0,1);
    feedbackSlider.setSuffix(" %");

    for (auto slider : {
             &timeEnvelopeFollowSlider,
             &gainEnvelopeFollowSlider}) {
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(slider);
        slider->addListener(this);
    }

    addAndMakeVisible(modVisualizer);
    addAndMakeVisible(filterVisualizer);

    for (auto slider : {
             &loCutSlider,
             &hiCutSlider,
             &gainSlider,
             &feedbackSlider,
             &modDepthSlider,
             &modRateSlider,
             &distortionSlider
         }) {
        addAndMakeVisible(slider);
        slider->addListener(this);
        slider->setMappingEngine(&mappingEngine);
    }
    loCutSlider.setModKey({ModulatableKey::line, graphLine, LOCUT_ID, loCutSlider.getNormalisableRange()});
    hiCutSlider.setModKey({ModulatableKey::line, graphLine, HICUT_ID, hiCutSlider.getNormalisableRange()});
    gainSlider.setModKey({ModulatableKey::line, graphLine, GAIN_ID, gainSlider.getNormalisableRange()});
    feedbackSlider.setModKey({ModulatableKey::line, graphLine, FEEDBACK_ID, feedbackSlider.getNormalisableRange()});
    modDepthSlider.setModKey({ModulatableKey::line, graphLine, MOD_DEPTH_ID, modDepthSlider.getNormalisableRange()});
    modRateSlider.setModKey({ModulatableKey::line, graphLine, MOD_RATE_ID, modRateSlider.getNormalisableRange()});
    distortionSlider.setModKey({ModulatableKey::line, graphLine, DISTORTION_ID, distortionSlider.getNormalisableRange()});

    addAndMakeVisible(lengthEditor);

    for (auto button : {
             &bypassButton,
             &muteButton,
             &invertButton
         }) {
        addAndMakeVisible(button);
        button->addListener(this);
    }

    for (auto button : {
             &copyButton,
             &pasteButton
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

LineEditor::~LineEditor() {
    setLookAndFeel(nullptr);
}


void LineEditor::resized()
{
    topBar = getLocalBounds().withHeight(30);
    mainSection = getLocalBounds().withTrimmedTop(topBar.getHeight());

    bypassButton.setBounds(topBar);
    bypassButton.changeWidthToFitText();
    muteButton.setBounds(topBar.withLeft(bypassButton.getRight()));
    muteButton.changeWidthToFitText();
    copyButton.setBounds(topBar.withLeft(muteButton.getRight()));
    copyButton.changeWidthToFitText();
    pasteButton.setBounds(topBar.withLeft(copyButton.getRight()));
    pasteButton.changeWidthToFitText();

    lengthEditor.setBounds(mainSection.withHeight(40));

    auto gainAndFeedbackArea = mainSection.withTrimmedTop(lengthEditor.getHeight()).withHeight(50);
    auto gainArea = gainAndFeedbackArea.withWidth(gainAndFeedbackArea.getWidth() / 2);
    auto feedbackArea = gainAndFeedbackArea.withTrimmedLeft(gainArea.getRight());

    gainSlider.setBounds(gainArea.withHeight(40));
    feedbackSlider.setBounds(feedbackArea.withHeight(40));
    gainLabel.setBounds(gainArea.withTrimmedTop(gainSlider.getHeight()));
    feedbackLabel.setBounds(feedbackArea.withTrimmedTop(feedbackSlider.getHeight()));

    // Modding length
    modArea = mainSection.withTrimmedTop(gainAndFeedbackArea.getBottom()).withHeight(80);

    modVisualizer.setBounds(modArea);

    auto thirdWidth = modArea.getWidth() / 3;
    modDepthLabel.setBounds(modArea.withWidth(thirdWidth).withHeight(10));
    modDepthSlider.setBounds(modArea.withWidth(thirdWidth).withTrimmedTop(modDepthLabel.getHeight()));

    modRateLabel.setBounds(modArea.withWidth(thirdWidth).withRightX(modArea.getRight()).withHeight(10));
    modRateSlider.setBounds(modArea.withWidth(thirdWidth).withRightX(modArea.getRight()).withTrimmedTop(modRateLabel.getHeight()));

    modLabel.setBounds(modArea.withX(modDepthSlider.getRight()).withRight(modRateSlider.getX()));

    // Filter
    filterArea = mainSection.withTrimmedTop(modArea.getBottom()).withHeight(80);

    filterVisualizer.setBounds(filterArea);

    thirdWidth = modArea.getWidth() / 3;
    filterLoLabel.setBounds(filterArea.withWidth(thirdWidth).withHeight(10));
    loCutSlider.setBounds(filterArea.withWidth(thirdWidth).withTrimmedTop(filterLoLabel.getHeight()));

    filterHiLabel.setBounds(filterArea.withWidth(thirdWidth).withRightX(filterArea.getRight()).withHeight(10));
    hiCutSlider.setBounds(filterArea.withWidth(thirdWidth).withRightX(filterArea.getRight()).withTrimmedTop(filterHiLabel.getHeight()));

    filterLabel.setBounds(filterArea.withX(loCutSlider.getRight()).withRight(hiCutSlider.getX()));

    // Distortion
    distortionArea = mainSection.withTrimmedTop(filterArea.getBottom()).withHeight(80);
    distortionSlider.setBounds(distortionArea.withSizeKeepingCentre(distortionArea.getHeight(), distortionArea.getHeight()));

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
}

void LineEditor::sliderValueChanged (juce::Slider* slider)
{
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }
    if (slider == &gainSlider) {
        line->parameters.gain = gainSlider.getValue();
    } else if (slider == &timeEnvelopeFollowSlider) {
        line->parameters.lengthEnvelopeFollow = timeEnvelopeFollowSlider.getValue();
    } else if (slider == &modRateSlider) {
        line->parameters.modRate = modRateSlider.getValue();
    } else if (slider == &modDepthSlider) {
        line->parameters.modDepth = modDepthSlider.getValue();
    } else if (slider == &distortionSlider) {
        line->parameters.distortion = distortionSlider.getValue();
    } else if (slider == &loCutSlider) {
        line->parameters.loCut = loCutSlider.getValue();
        filterVisualizer.setFilters(
            loCutSlider.valueToProportionOfLength(loCutSlider.getValue()),
            hiCutSlider.valueToProportionOfLength(hiCutSlider.getValue()));
    } else if (slider == &hiCutSlider) {
        line->parameters.hiCut = hiCutSlider.getValue();
        filterVisualizer.setFilters(
            loCutSlider.valueToProportionOfLength(loCutSlider.getValue()),
            hiCutSlider.valueToProportionOfLength(hiCutSlider.getValue()));
    } else if (slider == &gainEnvelopeFollowSlider) {
        line->parameters.gainEnvelopeFollow = gainEnvelopeFollowSlider.getValue();
    } else if (slider == &feedbackSlider) {
        line->parameters.feedback = feedbackSlider.getValue();
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

    filterVisualizer.setFilters(
        line->parameters.loCut.convertTo0to1(line->parameters.loCut),
        line->parameters.hiCut.convertTo0to1(line->parameters.hiCut));
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
    } else if (button == &copyButton) {
        auto xml = juce::XmlElement("line");
        auto line = delayGraph.getLine(graphLine);
        if (line) {
            line->parameters.exportToXml(&xml);
            juce::SystemClipboard::copyTextToClipboard(xml.toString());
        }
    } else if (button == &pasteButton) {
        auto xml = juce::parseXML(juce::SystemClipboard::getTextFromClipboard());
        if (xml) {
            auto line = delayGraph.getLine(graphLine);
            if (line) {
                line->parameters.importFromXml(xml.get());
            }
        }
    }
}
