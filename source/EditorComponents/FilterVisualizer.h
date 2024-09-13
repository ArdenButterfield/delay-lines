//
// Created by arden on 9/3/24.
//

#ifndef DELAYLINES_FILTERVISUALIZER_H
#define DELAYLINES_FILTERVISUALIZER_H

#include "FilterData.h"
#include "BinaryData.h"
#include "juce_gui_basics/juce_gui_basics.h"

class FilterVisualizer : public juce::Component
{
public:
    FilterVisualizer();
    ~FilterVisualizer() override {

    }
    void paint(juce::Graphics &g) override;

    void resized() override {
    }

    void setFilters(float _low, float _high);
private:
    float low;
    float high;
    int pixelSize = 3;
};

#endif //DELAYLINES_FILTERVISUALIZER_H
