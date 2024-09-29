//
// Created by arden on 3/18/24.
//

#ifndef DELAYLINES_LINEEDITOR_H
#define DELAYLINES_LINEEDITOR_H

#include "juce_gui_basics/juce_gui_basics.h"

#include "../../DelayGraph.h"
#include "LengthEditor.h"
#include "../DelayLinesLookAndFeel.h"
#include "ModVisualizer.h"
#include "FilterVisualizer.h"

#include "../../Modulation/ModulationMappingEngine.h"
#include "../../Modulation/ModOverlayButton.h"

class LineEditor : public juce::Component, public juce::Slider::Listener, public juce::ToggleButton::Listener, public juce::Timer
{
public:
    LineEditor(ModulationMappingEngine& me, DelayGraph& delayGraph, const int& line);
    ~LineEditor() override;
    void resized() override;
    void paint(juce::Graphics &g) override;

    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

    juce::ToggleButton bypassButton;
    juce::ToggleButton muteButton;

    LengthEditor lengthEditor;

    CompactSlider modDepthSlider;
    CompactSlider modRateSlider;
    juce::Slider timeEnvelopeFollowSlider;

    CompactSlider distortionSlider;
    CompactSlider loCutSlider;
    CompactSlider hiCutSlider;

    CompactSlider gainSlider;
    juce::ToggleButton invertButton;
    juce::Slider gainEnvelopeFollowSlider;

    CompactSlider feedbackSlider;

private:
    juce::TextButton copyButton;
    juce::TextButton pasteButton;

    juce::Label gainLabel;
    juce::Label feedbackLabel;
    juce::Label modLabel;
    juce::Label modRateLabel;
    juce::Label modDepthLabel;
    juce::Label filterLabel;
    juce::Label filterLoLabel;
    juce::Label filterHiLabel;


    juce::Rectangle<int> topBar;
    juce::Rectangle<int> mainSection;
    juce::Rectangle<int> modArea;
    juce::Rectangle<int> filterArea;
    juce::Rectangle<int> distortionArea;

    juce::ComponentDragger dragger;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonStateChanged(juce::Button *) override;
    void buttonClicked(juce::Button *) override;

    void timerCallback() override;

    DelayGraph& delayGraph;
    const int graphLine;

    std::array<juce::Rectangle<int>, 3> panels;

    bool dragging;

    DelayLinesLookAndFeel delayLinesLookAndFeel;

    ModVisualizer modVisualizer;
    FilterVisualizer filterVisualizer;
    ModulationMappingEngine& mappingEngine;
};

#endif //DELAYLINES_LINEEDITOR_H
