//
// Created by arden on 7/28/24.
//

#ifndef DELAYLINES_GRAPHLINECOMPONENT_H
#define DELAYLINES_GRAPHLINECOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_dsp/juce_dsp.h"
class LineEditor;
class GraphLine;
class DelayGraph;
class PlaygroundInterface;
class ModulationMappingEngine;

class GraphLineComponent : public juce::Component
{
public:
    GraphLineComponent(ModulationMappingEngine& me, DelayGraph& _delayGraph, PlaygroundInterface* _playgroundInterface, int _id);
    ~GraphLineComponent() override;
    void paint(juce::Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
    int getIdentifier() const { return id; }

    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

    const float loopingBackRadius = 20;
    const float loopingBackStep = 10;
private:
    std::unique_ptr<LineEditor> lineEditor;
    const int id;
    DelayGraph& delayGraph;
    PlaygroundInterface* playgroundInterface;

    const int lineHoverDistance = 30;
    const float preTransformInnerLineWidth = 0.2;
    ModulationMappingEngine& mappingEngine;

    bool getLoopbackStatus(int& index, float& radius);

    void makeEnvelopePaths(bool bypassed, float scale=1);

    GraphLine* line;
    juce::Point<float> startPoint, endPoint;
    juce::Path leftLinePath;
    juce::Path rightLinePath;
    juce::Path innerLeftPath;
    juce::Path innerRightPath;
    juce::Path bypassArrow;

    juce::dsp::BallisticsFilter<double> lineWidth;
};

#endif //DELAYLINES_GRAPHLINECOMPONENT_H
