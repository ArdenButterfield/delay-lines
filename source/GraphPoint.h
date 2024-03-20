//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHPOINT_H
#define DELAYLINES_GRAPHPOINT_H

#include "juce_gui_basics/juce_gui_basics.h"

class GraphPoint : public juce::Point<int> {
public:
    explicit GraphPoint(const juce::Point<int>& p) : juce::Point<int>(p) {
    }
};

#endif //DELAYLINES_GRAPHPOINT_H
