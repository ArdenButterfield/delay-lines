//
// Created by arden on 10/9/24.
//

#include "NewLineButton.h"
#include "GraphLineModule.h"

NewLineButton::NewLineButton (DelayGraph& dg) : delayGraph(dg)
{
}
NewLineButton::~NewLineButton()
{
}
void NewLineButton::paint (juce::Graphics& g)
{
    auto realBounds = getLocalBounds().withTrimmedLeft(GraphLineModule::padding * 2).withTrimmedTop(GraphLineModule::padding * 2);
    if (isMouseOverOrDragging()) {
        g.setColour(juce::Colours::darkgrey);
    } else if (isMouseButtonDown()) {
        g.setColour(juce::Colours::black);
    } else {
        g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    }
    g.drawRect(realBounds, 10.f);
    // plus symbol in center
    g.fillRect(juce::Rectangle<int>(0,0,75,10)
        .withCentre(realBounds.getCentre()));
    g.fillRect(juce::Rectangle<int>(0,0,10,75)
        .withCentre(realBounds.getCentre()));
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
