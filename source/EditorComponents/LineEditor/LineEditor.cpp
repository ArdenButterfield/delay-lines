//
// Created by arden on 3/18/24.
//

#include "LineEditor.h"
#include "../../parameters.h"

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
    addAndMakeVisible(distortionVisualizer);

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

    addAndMakeVisible(distortionTypeSelector);
    distortionTypeSelector.addItemList(DISTORTION_TYPE_OPTIONS, 1);
    distortionTypeSelector.addListener(this);
    startTimerHz(60);

    gainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    feedbackLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    gainSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::black);
    feedbackSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colours::black);
}

LineEditor::~LineEditor() {
    setLookAndFeel(nullptr);
}


void LineEditor::resized()
{
    topBar = getLocalBounds().withHeight(30);
    topBarUnderline = getLocalBounds().withTrimmedTop(topBar.getBottom()).withHeight(4);

    firstColumn = getLocalBounds().withTrimmedTop(topBarUnderline.getBottom()).withWidth(getWidth() / 2);
    secondColumn = firstColumn.withRight(getRight()).withTrimmedLeft(firstColumn.getRight());

    firstColumn = firstColumn.withTrimmedRight(1);
    secondColumn = secondColumn.withTrimmedLeft(1);
    columnGutter = firstColumn.withRight(secondColumn.getX()).withLeft(firstColumn.getRight());

    copyButton.setBounds(topBar.withTrimmedTop(2).withTrimmedLeft(2).withTrimmedBottom(2));
    copyButton.changeWidthToFitText();
    pasteButton.setBounds(copyButton.getBounds().withX(copyButton.getRight() + 2));
    pasteButton.changeWidthToFitText();

    auto firstColumnInner = firstColumn.withSize(firstColumn.getWidth() - 4, firstColumn.getHeight() - 4);

    bypassButton.setBounds(firstColumnInner.withHeight(25));
    muteButton.setBounds(firstColumnInner.withTrimmedTop(bypassButton.getHeight() + 2).withHeight(25));

    lengthEditor.setBounds(firstColumnInner.withTop(muteButton.getBottom() + 2).withHeight(80));

    auto gainAndFeedbackArea = firstColumnInner.withTop(lengthEditor.getBottom()).withHeight(50);
    auto gainArea = gainAndFeedbackArea.withWidth(gainAndFeedbackArea.getWidth() / 2);
    auto feedbackArea = gainAndFeedbackArea.withTrimmedLeft(gainArea.getRight());
    gainLabel.setBounds(gainArea.withHeight(12));
    feedbackLabel.setBounds(feedbackArea.withHeight(12));
    gainSlider.setBounds(gainArea.withTop(gainLabel.getBottom()));
    feedbackSlider.setBounds(feedbackArea.withTop(feedbackLabel.getBottom()));

    // Modding length
    modArea = secondColumn.withHeight(secondColumn.getHeight() / 3);
    modVisualizer.setBounds(modArea);

    auto thirdWidth = modArea.getWidth() / 3;
    modDepthLabel.setBounds(modArea.withWidth(thirdWidth).withHeight(10));
    modDepthSlider.setBounds(modArea.withWidth(thirdWidth).withTrimmedTop(modDepthLabel.getHeight()));

    modRateLabel.setBounds(modArea.withWidth(thirdWidth).withRightX(modArea.getRight()).withHeight(10));
    modRateSlider.setBounds(modArea.withWidth(thirdWidth).withRightX(modArea.getRight()).withTrimmedTop(modRateLabel.getHeight()));

    modLabel.setBounds(modArea.withX(modDepthSlider.getRight()).withRight(modRateSlider.getX()));

    // Filter
    filterArea = secondColumn.withTop(modArea.getBottom()).withHeight(secondColumn.getHeight() / 3);

    filterVisualizer.setBounds(filterArea);
    thirdWidth = modArea.getWidth() / 3;
    filterLoLabel.setBounds(filterArea.withWidth(thirdWidth).withHeight(10));
    loCutSlider.setBounds(filterArea.withWidth(thirdWidth).withTrimmedTop(filterLoLabel.getHeight()));

    filterHiLabel.setBounds(filterArea.withWidth(thirdWidth).withRightX(filterArea.getRight()).withHeight(10));
    hiCutSlider.setBounds(filterArea.withWidth(thirdWidth).withRightX(filterArea.getRight()).withTrimmedTop(filterHiLabel.getHeight()));

    filterLabel.setBounds(filterArea.withX(loCutSlider.getRight()).withRight(hiCutSlider.getX()));

    // Distortion
    distortionArea = secondColumn.withTop(filterArea.getBottom());
    distortionTypeSelector.setBounds(distortionArea.withHeight(40).withBottomY(distortionArea.getBottom()));
    distortionSlider.setBounds(distortionArea.withBottom(distortionTypeSelector.getY()));
    distortionVisualizer.setBounds(distortionSlider.getBounds());
}

void LineEditor::textureArea (juce::Graphics& g, juce::Rectangle<int> area)
{
    const float step = 8;
    bool odd = false;
    g.setColour(juce::Colour(0xfffff7ce));
    int ctr = 0;
    for (float y = area.getY(); y < area.getBottom(); y += step * std::sqrt(3.f/4.f)) {
        for (float x = area.getX() + (odd ? step / 2 : 0); x < area.getRight(); x += step) {
            g.fillEllipse(x, y, 3.f, 3.f);
            if (++ctr > 10000) {
                DBG("safety hit in drawing texture area");
                return;
                // safety for if we get passed a huge area
            }
        }
        odd = !odd;
    }
}

void LineEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colour(0xffffdb93));
    g.fillAll();
    textureArea (g, topBar);
    auto line = delayGraph.getLine(graphLine);
    if (line) {
        g.setColour (line->getColor());
        bypassButton.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, line->getColor());
        muteButton.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, line->getColor());
    }

    g.fillRect(topBarUnderline);
    g.fillRect(columnGutter);
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
    } else if (slider == &hiCutSlider) {
        line->parameters.hiCut = hiCutSlider.getValue();
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
    distortionTypeSelector.setSelectedId(line->parameters.distortionType + 1);
    distortionVisualizer.setDistortion(line->parameters.distortionType, line->parameters.distortion);
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

void LineEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }
    if (comboBoxThatHasChanged == &distortionTypeSelector) {
        line->parameters.distortionType = distortionTypeSelector.getSelectedItemIndex();
    }
}
