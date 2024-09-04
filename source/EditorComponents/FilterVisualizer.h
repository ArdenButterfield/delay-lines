//
// Created by arden on 9/3/24.
//

#ifndef DELAYLINES_FILTERVISUALIZER_H
#define DELAYLINES_FILTERVISUALIZER_H

#include "FilteredSquares.h"
#include "juce_gui_basics/juce_gui_basics.h"

class FilterVisualizer : public juce::Component
{
public:
    FilterVisualizer() {
        low = 0.25; high = 0.75;
    }
    ~FilterVisualizer() override {

    }
    void paint(juce::Graphics &g) override {
        int convertedLow = std::floor(low * 100);
        int convertedHigh = std::floor(high * 100);

        auto match = filteredSquares.find(convertedLow * 1000 + convertedHigh);


        auto threshold = 127;

        for (unsigned x = 0; x * pixelSize < getWidth(); ++x) {
            for (unsigned y = 0; y * pixelSize < getHeight(); ++y) {
                if (match != filteredSquares.end() && match->second[y * 200 + x] > threshold) {
                    g.setColour(juce::Colours::white);
                } else {
                    g.setColour(juce::Colours::black);
                }
                g.fillRect(x + pixelSize, y * pixelSize, pixelSize, pixelSize);
            }
        }
    }
    void resized() override {
    }
    void setFilters(float _low, float _high) {
        low = _low, high = _high;
        repaint();
    }
private:
    float low;
    float high;
    int pixelSize = 5;
};

#endif //DELAYLINES_FILTERVISUALIZER_H
