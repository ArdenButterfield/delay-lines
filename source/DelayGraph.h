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
    class Listener
    {
    public:
        Listener() {}
        ~Listener() {}
        virtual void pointAdded(int identifier) = 0;
        virtual void lineAdded(int identifier) = 0;
        virtual void pointRemoved(int identifier) = 0;
        virtual void lineRemoved(int identifier) = 0;
    };

    DelayGraph();
    DelayGraph(juce::XmlElement* element);
    void addListener(Listener* listener);
    void removeListener(Listener* listener);
    void addPoint(const juce::Point<float>& point, bool connectToSelected=false);
    void deletePoint(const GraphPoint* point);
    void deleteLine(const GraphLine* line);
    const std::vector<std::unique_ptr<GraphPoint>>& getPoints();
    void addLine(GraphPoint* start, GraphPoint* end);
    void addPoint(juce::XmlElement* xml);
    void addLine(juce::XmlElement* xml);
    std::vector<std::unique_ptr<GraphLine>>& getLines();

    bool modulateIfPossible(ModulatableKey& key, float newValue);
    bool getModulationValue(ModulatableKey& key, float& val);

    std::vector<int> getAllLinesBetweenPoints(GraphPoint* pointA, GraphPoint* pointB);
    std::vector<int> getAllLinesStartingAndEndingAtSamePoint(GraphPoint* point);

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
    GraphPoint* lineInProgressEndPoint{};

    GraphPoint* activePoint;

    GraphPoint* startPoint;
    GraphPoint* endPoint;

    InteractionState interactionState;

    GraphLine* activeLine{};
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void processSample(std::vector<float>& sample);

    void bakeOffsets();

    GraphPoint* getPoint(const int& id);
    GraphLine* getLine(const int& id);

    bool copyPoint(int id, std::unique_ptr<GraphPoint>& out);

    void setMidiTrackNote(int pitch);

    void setRealOutputs();

    void timerCallback() override;
    std::vector<int> getAllPointIds();
    std::vector<int> getAllLineIds();

    void exportToXml(juce::XmlElement* parent);
    bool importFromXml(juce::XmlElement* parent);

    void applyGlobalOffset(juce::Point<float> offset);
    void clear();
    void setBpm(float _bpm) {
        bpm = _bpm;
        for (auto& line : lines) {
            line->setBpm(bpm);
        }
    }
    void setStretchTime (float newStretchTime);

private:
    float bpm;
    std::set<Listener*> listeners;
    std::set<GraphPoint*> realGlobalInputs;
    juce::CriticalSection criticalSection;
    std::unique_ptr<juce::dsp::ProcessSpec> processSpec;

    std::vector<std::unique_ptr<GraphPoint>> points;
    std::vector<std::unique_ptr<GraphLine>> lines;

    int findUniqueLineId();
    int findUniquePointId();
    bool needsClearing;
    const float clearingFadeoutLength = 100;
    float clearingFadeoutCounter;
};

#endif //DELAYLINES_DELAYGRAPH_H
