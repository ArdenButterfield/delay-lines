//
// Created by arden on 10/30/24.
//

#include "SwitchInterfaceButton.h"
void SwitchInterfaceButton::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setColour(juce::Colours::lightgrey);
    g.fillRoundedRectangle(getLocalBounds().reduced(2).toFloat(), shouldDrawButtonAsHighlighted ? 6 : 3);
    g.setColour(juce::Colours::darkgrey);
    g.drawRoundedRectangle(getLocalBounds().reduced(2).toFloat(), shouldDrawButtonAsHighlighted ? 6 : 3, 2);

    g.setColour(juce::Colours::black);
    auto innerRect = innerSpace.withHeight(innerSpace.getHeight() / 2).translated(0, position * innerSpace.getHeight() / 2);
    g.fillRoundedRectangle(innerRect, shouldDrawButtonAsHighlighted ? 6 : 3);

    g.setColour(juce::Colours::white.withMultipliedBrightness(1 - position));
    g.drawFittedText("WEB", innerSpace.withHeight(innerSpace.getHeight() / 2).toNearestInt(),
        juce::Justification::centred, 1);
    g.setColour(juce::Colours::white.withMultipliedBrightness(position));
    g.drawFittedText("GRID", innerSpace.withHeight(innerSpace.getHeight() / 2).withBottomY(innerSpace.getBottom()).toNearestInt(),
        juce::Justification::centred, 1);
}
