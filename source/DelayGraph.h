//
// Created by arden on 3/14/24.
//

#ifndef DELAYLINES_DELAYGRAPH_H
#define DELAYLINES_DELAYGRAPH_H

#include "juce_graphics/juce_graphics.h"
#include "juce_dsp/juce_dsp.h"

#include "GraphPoint.h"
#include "GraphLine.h"


class DelayGraph
{
public:
    DelayGraph();
    ~DelayGraph() = default;
    void addPoint(const juce::Point<int>& point, bool connectToSelected=false);
    void deletePoint(const GraphPoint* point);
    void deleteLine(const GraphLine* line);
    const std::vector<std::unique_ptr<GraphPoint>>& getPoints();
    void addLine(GraphPoint* start, GraphPoint* end);
    std::vector<std::unique_ptr<GraphLine>>& getLines();

    enum InteractionState {
        none,
        innerSelected,
        outerSelected,
        movingPoint,
        creatingLine,
        lineHover,
        editingLine
    };

    juce::Point<int> lineInProgressEnd;
    GraphPoint* lineInProgressEndPoint;

    GraphPoint* activePoint;

    GraphPoint* startPoint;
    GraphPoint* endPoint;

    InteractionState interactionState;

    GraphLine* activeLine;
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void processSample(std::vector<float>& sample);
private:
    std::unique_ptr<juce::dsp::ProcessSpec> processSpec;

    std::vector<std::unique_ptr<GraphPoint>> points;
    std::vector<std::unique_ptr<GraphLine>> lines;
};

#endif //DELAYLINES_DELAYGRAPH_H
