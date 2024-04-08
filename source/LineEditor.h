//
// Created by arden on 3/18/24.
//

#ifndef DELAYLINES_LINEEDITOR_H
#define DELAYLINES_LINEEDITOR_H

#include "juce_gui_basics/juce_gui_basics.h"

#include "DelayGraph.h"

class LineEditor : public juce::Component, public juce::Slider::Listener, public juce::ToggleButton::Listener, public juce::Timer
{
public:
    LineEditor(GraphLine& graphLine);
    ~LineEditor() override;
    void resized() override;
    void paint(juce::Graphics &g) override;

    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

private:
    juce::ComponentDragger dragger;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonStateChanged(juce::Button *) override;
    void buttonClicked(juce::Button *) override;


    void timerCallback() override;

    GraphLine& graphLine;

    juce::ToggleButton bypassButton;
    juce::ToggleButton muteButton;

    juce::Slider timeSlider;
    juce::Slider modDepthSlider;
    juce::Slider modRateSlider;
    juce::Slider timeEnvelopeFollowSlider;

    juce::Slider distortionSlider;
    juce::Slider loCutSlider;
    juce::Slider hiCutSlider;

    juce::Slider gainSlider;
    juce::ToggleButton invertButton;
    juce::Slider gainEnvelopeFollowSlider;

    juce::Rectangle<int> topBar;
    juce::Rectangle<int> mainSection;
    std::array<juce::Rectangle<int>, 3> panels;

    bool dragging;
};

#endif //DELAYLINES_LINEEDITOR_H
