//
// Created by arden on 8/13/24.
//

#include "PlaygroundComponent.h"
PlaygroundComponent::PlaygroundComponent()
{
}
PlaygroundComponent::~PlaygroundComponent()
{
}

void PlaygroundComponent::resized()
{
}

void PlaygroundComponent::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.fillAll();
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}
