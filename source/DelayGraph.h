//
// Created by arden on 3/14/24.
//

#ifndef DELAYLINES_DELAYGRAPH_H
#define DELAYLINES_DELAYGRAPH_H

#include "juce_graphics/juce_graphics.h"
#include "juce_dsp/juce_dsp.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"

#include "GraphPoint.h"
#include "Line/GraphLine.h"
#include "Modulation/ModulatableKey.h"

class DelayGraph : public juce::Timer
{
public:
    DelayGraph();
    DelayGraph(juce::XmlElement* element);
    void addPoint(const juce::Point<float>& point, bool connectToSelected=false);
    void deletePoint(const GraphPoint* point);
    void deleteLine(const GraphLine* line);
    const std::vector<std::unique_ptr<GraphPoint>>& getPoints();
    void addLine(GraphPoint* start, GraphPoint* end);
    std::vector<std::unique_ptr<GraphLine>>& getLines();

    bool modulateIfPossible(ModulatableKey& key, float newValue);

    enum InteractionState {
        none = 0,
        innerSelected = 1,
        outerSelected = 2,
        movingPoint = 3,
        creatingLine = 4,
        lineHover = 5,
        editingLine = 6,
        stretchingPoint = 7
    };

    juce::Point<float> lineInProgressEnd;
    GraphPoint* lineInProgressEndPoint;

    GraphPoint* activePoint;

    GraphPoint* startPoint;
    GraphPoint* endPoint;

    InteractionState interactionState;

    GraphLine* activeLine;
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void processSample(std::vector<float>& sample);

    void bakeOffsets();

    GraphPoint* getPoint(const int& id);
    GraphLine* getLine(const int& id);

    void setRealOutputs();

    void timerCallback() override;

    void exportToXml(juce::XmlElement* parent);
    bool importFromXml(juce::XmlElement* parent);
private:
    std::set<GraphPoint*> realGlobalInputs;
    juce::CriticalSection criticalSection;
    std::unique_ptr<juce::dsp::ProcessSpec> processSpec;

    std::vector<std::unique_ptr<GraphPoint>> points;
    std::vector<std::unique_ptr<GraphLine>> lines;

    int findUniqueLineId();
    int findUniquePointId();
};

#endif //DELAYLINES_DELAYGRAPH_H
