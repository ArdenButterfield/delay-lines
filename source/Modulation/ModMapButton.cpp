//
// Created by arden on 10/30/24.
//

#include "ModMapButton.h"
#include "../EditorComponents/DelayLinesLookAndFeel.h"

void ModMapButton::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto backgroundColor = juce::Colours::lightgrey;
    if (shouldDrawButtonAsDown) {
        backgroundColor = backgroundColor.withMultipliedBrightness(0.4);
    } else if (getToggleState()) {
        backgroundColor = splashColour.withSaturation(std::sin(static_cast<float>(juce::Time::getCurrentTime().toMilliseconds() % 4000) / 4000.f * juce::MathConstants<float>::twoPi) * 0.5f + 0.5f);
    } else if (shouldDrawButtonAsHighlighted) {
        backgroundColor = backgroundColor.withMultipliedBrightness(1.2);
    }

    g.setColour(backgroundColor);
    g.fillRoundedRectangle(getLocalBounds().reduced(2).toFloat(), 3);
    g.setColour(juce::Colours::darkgrey);
    g.drawRoundedRectangle(getLocalBounds().reduced(2).toFloat(), 3, 2);
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(DelayLinesLookAndFeel::getMonoFont()).withHeight(shouldDrawButtonAsHighlighted ? 13 : 12));
    g.drawText(getButtonText(), getLocalBounds(), juce::Justification::centred);
}
