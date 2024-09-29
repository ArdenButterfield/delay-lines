//
// Created by arden on 9/3/24.
//

#include "ModVisualizer.h"
#include "../../DelayGraph.h"

ModVisualizer::ModVisualizer (DelayGraph* dg, int l) : delayGraph(dg), lineIndex(l)
{
    startTimerHz(60);
}

void ModVisualizer::paint (juce::Graphics& g)
{
    auto line = delayGraph->getLine(lineIndex);
    if (line != nullptr) {
        auto oscillator = line->getModOscillator();
        if (oscillator) {
            oscillator->copyOutHistory (lows, highs, false);
        }
    }

    g.setColour(juce::Colours::black);
    auto path = juce::Path();
    path.startNewSubPath(0,(1 + lows[0] * 0.8f) * getHeight() / 2.f);
    for (unsigned i = 0; i < lows.size(); ++i) {
        auto v = lows[i];
        path.lineTo(i, (v * 0.8f + 1) * getHeight() / 2 + 4);
    }
    for (int i = highs.size() - 1; i >= 0; --i) {
        auto v = highs[i];
        path.lineTo(i, (v * 0.8f + 1) * getHeight() / 2 - 4);
    }
    path.closeSubPath();
    g.fillPath(path);
}

void ModVisualizer::resized()
{
    lows.resize(static_cast<unsigned long> (getWidth()));
    highs.resize(static_cast<unsigned long> (getWidth()));
}
void ModVisualizer::timerCallback()
{
    repaint();
}
