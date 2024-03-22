//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHPOINT_H
#define DELAYLINES_GRAPHPOINT_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_dsp/juce_dsp.h"

class GraphPoint : public juce::Point<float>, juce::Timer {
public:
    enum PointType {
        inner,
        start,
        end
    };

    explicit GraphPoint(const juce::Point<float>& p, PointType pt) : juce::Point<float>(p), pointType(pt) {
        offset.setXY(0,0);
        startTimerHz(30);
    }

    void timerCallback() override {
        if (!draggingOffset) {
            offset *= 0.9;
            if (offset.getDistanceSquaredFromOrigin() < 0.2) {
                offset *= 0;
            }
        }
    }

    const PointType pointType;

    void prepareToPlay(juce::dsp::ProcessSpec* spec) {
        samples.resize(spec->numChannels);
    }

    bool draggingOffset;

    std::vector<float> samples;
    juce::Point<float> offset;
};

class InnerPoint : public GraphPoint {
public:
    explicit InnerPoint(const juce::Point<float>& p) : GraphPoint(p, inner) {
    }
};

class StartPoint : public GraphPoint {
public:
    explicit StartPoint(const juce::Point<float>& p) : GraphPoint(p, start) {
    }
};

class EndPoint : public GraphPoint {
public:
    explicit EndPoint(const juce::Point<float>& p) : GraphPoint(p, end) {
    }
};

#endif //DELAYLINES_GRAPHPOINT_H
