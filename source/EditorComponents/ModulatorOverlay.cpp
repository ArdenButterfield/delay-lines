//
// Created by arden on 4/19/24.
//

#include "ModulatorOverlay.h"

ModulatorOverlay::ModulatorOverlay(PlaygroundView& _playgroundView) : playgroundView(_playgroundView)
{

}

ModulatorOverlay::~ModulatorOverlay()
{

}

void ModulatorOverlay::resized()
{

}

void ModulatorOverlay::paint(juce::Graphics &g)
{
    g.setColour(juce::Colours::magenta.withAlpha(0.5f));
    // auto editor = playgroundView.getLineEditor();

}
