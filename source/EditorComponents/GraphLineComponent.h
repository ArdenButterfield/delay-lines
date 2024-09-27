//
// Created by arden on 7/28/24.
//

#ifndef DELAYLINES_GRAPHLINECOMPONENT_H
#define DELAYLINES_GRAPHLINECOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"

class LineEditor;
class DelayGraph;
class PlaygroundComponent;
class ModulationMappingEngine;

class GraphLineComponent : public juce::Component
{
public:
    GraphLineComponent(ModulationMappingEngine& me, DelayGraph& _delayGraph, PlaygroundComponent* _playgroundComponent, int _id);
    ~GraphLineComponent() override;
    void paint(juce::Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
    int getIdentifier() const { return id; }

    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
private:
    std::unique_ptr<LineEditor> lineEditor;
    const int id;
    DelayGraph& delayGraph;
    PlaygroundComponent* playgroundComponent;

    const int lineHoverDistance = 10;
    ModulationMappingEngine& mappingEngine;
};

#endif //DELAYLINES_GRAPHLINECOMPONENT_H
