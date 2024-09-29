//
// Created by arden on 9/28/24.
//

#ifndef DELAYLINES_MODULARINTERFACE_H
#define DELAYLINES_MODULARINTERFACE_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "../DelayGraph.h"
#include "GraphLineModule.h"

class ModulationMappingEngine;

class ModularInterface : public juce::Component, public DelayGraph::Listener, public juce::Timer
{
public:
    ModularInterface(ModulationMappingEngine& me, DelayGraph& dg);
    ~ModularInterface() override;
private:
    void pointAdded(int identifier) override { addPoint(identifier); }
    void pointRemoved(int identifier) override { removePoint(identifier); }
    void lineAdded(int identifier) override { addLine(identifier); }
    void lineRemoved(int identifier) override { removeLine(identifier); }

    void addPoint(int identifier);
    void removePoint(int identifier);
    void addLine(int identifier);
    void removeLine(int identifier);

    void resized() override;
    void paint(juce::Graphics &g) override;
    void timerCallback() override;


    ModulationMappingEngine& mappingEngine;

    DelayGraph& delayGraph;

    std::vector<std::unique_ptr<GraphLineModule>> lineModules;
};

#endif //DELAYLINES_MODULARINTERFACE_H
