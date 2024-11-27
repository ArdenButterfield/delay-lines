//
// Created by arden on 9/29/24.
//

#include "GraphLineModule.h"

GraphLineModule::GraphLineModule(ModulationMappingEngine& me, DelayGraph& dg, const int& _line) : delayGraph(dg), lineID(_line) {
    lineEditor = std::make_unique<LineEditor>(me, delayGraph, lineID, false);
    addAndMakeVisible(lineEditor.get());

    addAndMakeVisible(inputSelector);
    addAndMakeVisible(outputSelector);
    addAndMakeVisible(deleteLineButton);

    inputSelector.addListener(this);
    outputSelector.addListener(this);
    deleteLineButton.addListener(this);
    deleteLineButton.setButtonText("Delete Line");

    deleteLineButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);

    rebuildPointSelectors();
    startTimerHz(15);
}
GraphLineModule::~GraphLineModule()
{
}

void GraphLineModule::resized()
{
    lineEditor->setBounds(LineEditor::getDesiredBounds().withPosition(padding * 2, padding * 2));
    auto bottomArea = getLocalBounds().reduced(2)
                          .withTop(lineEditor->getBottom() + 2)
                          .withTrimmedLeft(padding * 2);
    deleteLineButton.setBounds(bottomArea
                                    .withWidth(100)
                                    .withRightX(bottomArea.getRight()));
    inputSelector.setBounds(bottomArea.withRight(deleteLineButton.getX() - 2)
                                 .withHeight(bottomArea.getHeight() / 2));
    outputSelector.setBounds(bottomArea.withRight(deleteLineButton.getX() - 2)
                                  .withHeight(bottomArea.getHeight() / 2).withBottomY(bottomArea.getBottom()));

}

void GraphLineModule::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.fillRect(getLocalBounds().withTrimmedTop(padding * 2).withTrimmedLeft(padding * 2));
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
            }
            if (id == lineComponent->end->identifier) {
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
            auto id = outputSelector.getSelectedId();
            auto point = delayGraph.getPoint(id);
            if (point == nullptr) {
                return;
            }
            if (point != delayGraph.getLine(lineID)->end) {
                delayGraph.getLine(lineID)->end = point;
            }
        }
    }
}
void GraphLineModule::buttonClicked (juce::Button* b)
{
    if (b == &deleteLineButton) {
        delayGraph.deleteLine(delayGraph.getLine(lineID));
    }
}

void GraphLineModule::buttonStateChanged (juce::Button*)
{
}
