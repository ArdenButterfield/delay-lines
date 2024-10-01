//
// Created by arden on 9/29/24.
//

#include "GraphLineModule.h"

GraphLineModule::GraphLineModule(ModulationMappingEngine& me, DelayGraph& dg, const int& _line) : delayGraph(dg), lineID(_line) {
    lineEditor = std::make_unique<LineEditor>(me, delayGraph, lineID);
    addAndMakeVisible(lineEditor.get());

    addAndMakeVisible(inputSelector);
    addAndMakeVisible(outputSelector);

    inputSelector.addListener(this);
    outputSelector.addListener(this);

    rebuildPointSelectors();
    startTimerHz(15);
}
GraphLineModule::~GraphLineModule()
{
}

void GraphLineModule::resized()
{
    lineEditor->setBounds(LineEditor::getDesiredBounds().withPosition(padding, padding));
    inputSelector.setBounds({padding, lineEditor->getBottom() + 2, getWidth() - 2 * padding, 20});
    outputSelector.setBounds({padding, inputSelector.getBottom() + 2, getWidth() - 2 * padding, 20});
}

void GraphLineModule::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::tan);
    g.setColour(juce::Colours::brown);
    g.drawRect(getBounds(), 2);
}

const juce::Rectangle<int> GraphLineModule::getDesiredBounds()
{
    auto leBounds = LineEditor::getDesiredBounds();
    return { 0, 0, leBounds.getWidth() + padding * 2, leBounds.getHeight() + padding * 2 + 20 * 2 + 4};
}

juce::String GraphLineModule::makeComboboxNameForId (int id)
{
    if (id == delayGraph.startPoint->identifier) {
        return "input";
    }
    if (id == delayGraph.endPoint->identifier) {
        return "output";
    }
    return juce::String(id);
}

void GraphLineModule::rebuildPointSelectors()
{
    inputSelector.clear();
    outputSelector.clear();
    for (auto& id : delayGraph.getAllPointIds()) {
        inputSelector.addItem(makeComboboxNameForId(id), id);
        outputSelector.addItem(makeComboboxNameForId(id), id);
    }

    auto lineComponent = delayGraph.getLine(lineID);
    if (lineComponent) {
        for (auto& id : delayGraph.getAllPointIds()) {
            if (id == lineComponent->start->identifier) {
                inputSelector.setSelectedId(id);
            } else if (id == lineComponent->end->identifier) {
                outputSelector.setSelectedId(id);
            }
        }
    }
}

void GraphLineModule::timerCallback()
{
    rebuildPointSelectors();
}

void GraphLineModule::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    if (delayGraph.getLine(lineID)) {
        if (comboBoxThatHasChanged == &inputSelector) {
            delayGraph.getLine(lineID)->start = delayGraph.getPoint(inputSelector.getSelectedId());
        } else if (comboBoxThatHasChanged == &outputSelector) {
            delayGraph.getLine(lineID)->end = delayGraph.getPoint(outputSelector.getSelectedId());
        }
    }
}
