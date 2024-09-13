//
// Created by arden on 9/3/24.
//

#include "ModVisualizer.h"
#include "../DelayGraph.h"

ModVisualizer::ModVisualizer (DelayGraph* dg, int l) : delayGraph(dg), lineIndex(l)
{
    startTimer(10);
    modValues.resize(60 * 3);
    bufferPosition = 0;
}

void ModVisualizer::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    auto path = juce::Path();
    path.startNewSubPath(0,(1 + modValues[bufferPosition] * 0.8) * getHeight() / 2);
    for (unsigned i = 0; i < modValues.size(); ++i) {
        auto v = modValues[(bufferPosition + i) % modValues.size()];
        path.lineTo((i * getWidth()) / getHeight(), (v * 0.8 + 1) * getHeight() / 2);
    }
    g.strokePath(path, juce::PathStrokeType(7));
}

void ModVisualizer::resized()
{

}

void ModVisualizer::hiResTimerCallback()
{
    auto line = delayGraph->getLine(lineIndex);
    if (line != nullptr) {
        modValues[bufferPosition] = line->getCurrentModPosition();
        bufferPosition++;
        bufferPosition %= modValues.size();
    }
}
