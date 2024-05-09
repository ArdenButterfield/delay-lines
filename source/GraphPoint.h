//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHPOINT_H
#define DELAYLINES_GRAPHPOINT_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_dsp/juce_dsp.h"

#include "Modulation/ModulatableKey.h"

class GraphPoint : public juce::Point<float>, juce::Timer {
public:
    enum PointType {
        inner,
        start,
        end
    };

    explicit GraphPoint(const juce::Point<float>& p, PointType pt, const int& id);
    GraphPoint(juce::XmlElement* element);

    void timerCallback() override;

    void bakeOffset();

    PointType pointType;

    void prepareToPlay(juce::dsp::ProcessSpec* spec);

    bool draggingOffset;

    std::vector<float> samples;
    juce::Point<float> offset;

    const int identifier;

    void exportToXml(juce::XmlElement* parent);
    bool importFromXml(juce::XmlElement* parent);

    bool modulateIfPossible(ModulatableKey& key, float newValue);
};

#endif //DELAYLINES_GRAPHPOINT_H
