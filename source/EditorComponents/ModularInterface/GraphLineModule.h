//
// Created by arden on 9/29/24.
//

#ifndef DELAYLINES_GRAPHLINEMODULE_H
#define DELAYLINES_GRAPHLINEMODULE_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "../../DelayGraph.h"
#include "../../Modulation/ModulationMappingEngine.h"
#include "../LineEditor/LineEditor.h"
class GraphLineModule : public juce::Component, public juce::Timer, public juce::ComboBox::Listener
{
public:
    GraphLineModule(ModulationMappingEngine& me, DelayGraph& delayGraph, const int& line);
    ~GraphLineModule() override;

    void resized() override;
    void paint(juce::Graphics &g) override;
    static const juce::Rectangle<int> getDesiredBounds();
private:
    static const int padding = 10;
    void timerCallback() override;
    void rebuildPointSelectors();
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;

    juce::ComboBox inputSelector;
    juce::ComboBox outputSelector;
    std::unique_ptr<LineEditor> lineEditor;
    juce::String makeComboboxNameForId(int id);
    DelayGraph& delayGraph;

    const int lineID;
};

#endif //DELAYLINES_GRAPHLINEMODULE_H
