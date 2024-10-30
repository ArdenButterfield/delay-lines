//
// Created by arden on 10/29/24.
//

#include "TitleGraphic.h"
TitleGraphic::TitleGraphic()
{
}
TitleGraphic::~TitleGraphic()
{
}
void TitleGraphic::paint (juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(juce::Colours::white,
        {0,static_cast<float>(getHeight())},
        juce::Colours::black,
        {0,0},
        false));
    g.fillAll();

    g.setColour(juce::Colours::white);
    g.drawText("ECHO LOCATION", 0, 0, getWidth(), getHeight(), juce::Justification::topLeft);
}
