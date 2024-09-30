//
// Created by arden on 9/29/24.
//

#ifndef DELAYLINES_BOXOFLINEEDITORS_H
#define DELAYLINES_BOXOFLINEEDITORS_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "../../DelayGraph.h"
#include "GraphLineModule.h"

class ModulationMappingEngine;

class BoxOfLineEditors : public juce::Component, public DelayGraph::Listener
{
public:
    BoxOfLineEditors(ModulationMappingEngine& me, DelayGraph& dg);
    ~BoxOfLineEditors();

    void pointAdded(int identifier) override { addPoint(identifier); }
    void pointRemoved(int identifier) override { removePoint(identifier); }
    void lineAdded(int identifier) override { addLine(identifier); }
    void lineRemoved(int identifier) override { removeLine(identifier); }

    void addPoint(int identifier);
    void removePoint(int identifier);
    void addLine(int identifier);
    void removeLine(int identifier);

private:
    void paint(juce::Graphics &g) override;
    void resized() override;

    ModulationMappingEngine& mappingEngine;

    DelayGraph& delayGraph;

    std::vector<std::unique_ptr<GraphLineModule>> lineModules;

};

#endif //DELAYLINES_BOXOFLINEEDITORS_H
