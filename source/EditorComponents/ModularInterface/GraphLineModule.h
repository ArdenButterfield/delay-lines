//
// Created by arden on 9/29/24.
//

#ifndef DELAYLINES_GRAPHLINEMODULE_H
#define DELAYLINES_GRAPHLINEMODULE_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "../../DelayGraph.h"
#include "../../Modulation/ModulationMappingEngine.h"
#include "../LineEditor/LineEditor.h"
class GraphLineModule : public juce::Component
{
public:
    GraphLineModule(ModulationMappingEngine& me, DelayGraph& delayGraph, const int& line);
    ~GraphLineModule();

    void resized() override;
    void paint(juce::Graphics &g) override;
private:
    juce::ComboBox inputSelector;
    juce::ComboBox outputSelector;
    std::unique_ptr<LineEditor> lineEditor;
};

#endif //DELAYLINES_GRAPHLINEMODULE_H
