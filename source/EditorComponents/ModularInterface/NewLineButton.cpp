//
// Created by arden on 10/9/24.
//

#include "NewLineButton.h"
NewLineButton::NewLineButton (DelayGraph& dg) : delayGraph(dg)
{
}
NewLineButton::~NewLineButton()
{
}
void NewLineButton::paint (juce::Graphics& g)
{
    if (isMouseOverOrDragging()) {
        g.setColour(juce::Colours::brown);
    } else if (isMouseButtonDown()) {
        g.setColour(juce::Colours::black);
    } else {
        g.setColour(juce::Colours::brown.withAlpha(0.5f));
    }
    g.drawRect(getLocalBounds(), 10.f);
    // plus symbol in center
    g.fillRect(juce::Rectangle<int>(0,0,75,10).withCentre({getWidth() / 2, getHeight() / 2}));
    g.fillRect(juce::Rectangle<int>(0,0,10,75).withCentre({getWidth() / 2, getHeight() / 2}));
}
void NewLineButton::resized()
{
    Component::resized();
}
void NewLineButton::mouseEnter (const juce::MouseEvent& event)
{
    repaint();
}
void NewLineButton::mouseExit (const juce::MouseEvent& event)
{
    repaint();
}
void NewLineButton::mouseDown (const juce::MouseEvent& event)
{
    repaint();
}
void NewLineButton::mouseUp (const juce::MouseEvent& event)
{
    delayGraph.addLine(delayGraph.startPoint, delayGraph.endPoint);
}
