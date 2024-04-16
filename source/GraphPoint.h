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

    explicit GraphPoint(const juce::Point<float>& p, PointType pt, const int& id);
    GraphPoint(juce::XmlElement* element);

    void timerCallback() override {
        if (!draggingOffset) {
            offset *= 0.9;
            if (offset.getDistanceSquaredFromOrigin() < 0.2) {
                offset *= 0;
            }
        }
    }

    void bakeOffset() {
        this->x += offset.x;
        this->y += offset.y;
        offset.x = 0;
        offset.y = 0;
    }

    const PointType pointType;

    void prepareToPlay(juce::dsp::ProcessSpec* spec) {
        samples.resize(spec->numChannels);
    }

    bool draggingOffset;

    std::vector<float> samples;
    juce::Point<float> offset;

    const int identifier;

    void exportToXml(juce::XmlElement* parent);
    bool importFromXml(juce::XmlElement* parent);

    std::string idToString();
    static int stringToId(std::string s);
};

#endif //DELAYLINES_GRAPHPOINT_H
