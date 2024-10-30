//
// Created by arden on 5/3/24.
//

#include "ModOverlayButton.h"
ModOverlayButton::ModOverlayButton (ModulationMappingEngine& me, ModulatableKey key) : mappingEngine(me), modKey(std::move(key))
{
    mappingEngine.addListener(this);
}
ModOverlayButton::~ModOverlayButton()
{
    mappingEngine.removeListener(this);
}
void ModOverlayButton::mappingModeEntered(int parameterId)
{
    setVisible(true);
}
void ModOverlayButton::mappingModeExited()
{
    setVisible(false);
}
void ModOverlayButton::paint (juce::Graphics& g)
{
    auto brackets = juce::Path();
    brackets.startNewSubPath(10, 0);
    brackets.lineTo(0,0);
    brackets.lineTo(0, getHeight());
    brackets.lineTo(10, getHeight());

    brackets.startNewSubPath(getWidth() - 10, 0);
    brackets.lineTo(getWidth(), 0);
    brackets.lineTo(getWidth(), getHeight());
    brackets.lineTo(getWidth() - 10, getHeight());
    if (isMouseOver()) {
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.fillRect(getLocalBounds());
        g.setColour(overColor);
        g.strokePath(brackets, juce::PathStrokeType(12));
    } else {
        g.setColour(bgColor);
        g.strokePath(brackets, juce::PathStrokeType(6));
    }
}
void ModOverlayButton::resized()
{

}
void ModOverlayButton::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasClicked()) {
        mappingEngine.setModMap(modKey);
    }
}
