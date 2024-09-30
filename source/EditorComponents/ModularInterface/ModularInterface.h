//
// Created by arden on 9/28/24.
//

#ifndef DELAYLINES_MODULARINTERFACE_H
#define DELAYLINES_MODULARINTERFACE_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "../../DelayGraph.h"
#include "BoxOfLineEditors.h"

class ModulationMappingEngine;

class ModularInterface : public juce::Component, public juce::Timer
{
public:
    ModularInterface(ModulationMappingEngine& me, DelayGraph& dg);
    ~ModularInterface() override;
private:
    void resized() override;
    void paint(juce::Graphics &g) override;
    void timerCallback() override;

    std::unique_ptr<BoxOfLineEditors> boxOfLineEditors;
    juce::Viewport lineEditorsViewport;

    ModulationMappingEngine& mappingEngine;

    DelayGraph& delayGraph;
};

#endif //DELAYLINES_MODULARINTERFACE_H
