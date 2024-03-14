//
// Created by arden on 3/14/24.
//

#ifndef DELAYLINES_DELAYGRAPH_H
#define DELAYLINES_DELAYGRAPH_H

#include "juce_graphics/juce_graphics.h"

class GraphPoint : public juce::Point<int> {
public:
    explicit GraphPoint(const juce::Point<int>& p) : juce::Point<int>(p) {
    }
};

class GraphLine {
public:
    GraphLine(const GraphPoint* _start, const GraphPoint* _end) : start(_start), end(_end) {

    }
    const GraphPoint* start;
    const GraphPoint* end;
};

class DelayGraph
{
public:
    DelayGraph() {
        activePoint = nullptr;
        activePointAction = none;
    }
    ~DelayGraph() = default;
    void addPoint(const GraphPoint& point, bool connectToSelected=false);
    void deletePoint(const GraphPoint* point);
    const std::vector<std::unique_ptr<GraphPoint>>& getPoints();
    void addLine(const GraphLine& line);

    const std::vector<GraphLine>& getLines();

    enum PointAction {
        none,
        innerSelected,
        outerSelected,
        movingPoint,
        creatingLine
    };

    juce::Point<int> lineInProgressEnd;
    GraphPoint* lineInProgressEndPoint;

    GraphPoint* activePoint;
    PointAction activePointAction;

private:
    std::vector<std::unique_ptr<GraphPoint>> points;
    std::vector<GraphLine> lines;
};

#endif //DELAYLINES_DELAYGRAPH_H
