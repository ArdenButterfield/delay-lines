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
void ModOverlayButton::mappingModeEntered()
{
    setVisible(true);
}
void ModOverlayButton::mappingModeExited()
{
    setVisible(false);
}
void ModOverlayButton::paint (juce::Graphics& g)
{
    g.setColour(isMouseOver() ? overColor : bgColor);
    g.fillAll();
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
