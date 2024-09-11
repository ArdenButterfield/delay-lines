//
// Created by arden on 9/3/24.
//

#include "FilterVisualizer.h"
FilterVisualizer::FilterVisualizer()
{
    low = 0.25; high = 0.75;
}
void FilterVisualizer::paint (juce::Graphics& g)
{
    int convertedLow = std::floor(low * 25);
    int convertedHigh = std::floor(high * 25);

    auto matches = {
        filteredSquares.find(convertedLow * 1000 + convertedHigh),
        filteredSquares.find((convertedLow + 1) * 1000 + convertedHigh),
        filteredSquares.find(convertedLow * 1000 + (convertedHigh + 1)),
        filteredSquares.find((convertedLow + 1) * 1000 + (convertedHigh + 1)),
    };

    auto lpScore = 1 - (low * 25 - convertedLow);
    auto hpScore = 1 - (high * 25 - convertedHigh);

    auto matchScores = {
        lpScore * hpScore,
        (1 - lpScore) * hpScore,
        lpScore * (1 - hpScore),
        (1 - lpScore) * (1 - hpScore)
    };

    for (unsigned x = 0; x * pixelSize < getWidth(); ++x) {
        for (unsigned y = 0; y * pixelSize < getHeight(); ++y) {
            auto pixelScore = 0.f;
            for (int m = 0; m < 4; ++m) {
                auto match = matches.begin()[m];
                if (match != filteredSquares.end()) {
                    pixelScore += match->second[y * 100 + x] * matchScores.begin()[m];
                }
            }
            if (pixelScore > 255.f * 1/2) {
                g.setColour(juce::Colours::white);
            } else {
                g.setColour(juce::Colours::black);
            }
            g.fillRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize);
        }
    }
}

void FilterVisualizer::setFilters (float _low, float _high)
{
    low = _low, high = _high;
    repaint();
}
