//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHPOINT_H
#define DELAYLINES_GRAPHPOINT_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_dsp/juce_dsp.h"

class GraphPoint : public juce::Point<int> {
public:
    enum PointType {
        inner,
        start,
        end
    };

    explicit GraphPoint(const juce::Point<int>& p, PointType pt) : juce::Point<int>(p), pointType(pt) {
    }

    const PointType pointType;

    void prepareToPlay(juce::dsp::ProcessSpec* spec) {
        samples.resize(spec->numChannels);
    }

    std::vector<float> samples;
};

class InnerPoint : public GraphPoint {
public:
    explicit InnerPoint(const juce::Point<int>& p) : GraphPoint(p, inner) {
    }
};

class StartPoint : public GraphPoint {
public:
    explicit StartPoint(const juce::Point<int>& p) : GraphPoint(p, start) {
    }
};

class EndPoint : public GraphPoint {
public:
    explicit EndPoint(const juce::Point<int>& p) : GraphPoint(p, end) {
    }
};

#endif //DELAYLINES_GRAPHPOINT_H
