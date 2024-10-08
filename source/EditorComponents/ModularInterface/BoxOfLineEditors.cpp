//
// Created by arden on 9/29/24.
//

#include "BoxOfLineEditors.h"
BoxOfLineEditors::BoxOfLineEditors (ModulationMappingEngine& me, DelayGraph& dg) : mappingEngine(me), delayGraph(dg), minimumHeight(0)
{
    for (auto& lineID : delayGraph.getAllLineIds()) {
        lineModules.push_back(std::make_unique<GraphLineModule>(mappingEngine, delayGraph, lineID));
    }
    for (auto& module : lineModules) {
        addAndMakeVisible(module.get());
        module->setBounds(LineEditor::getDesiredBounds());
    }
    delayGraph.addListener(this);
}

BoxOfLineEditors::~BoxOfLineEditors()
{
    delayGraph.removeListener(this);
}
void BoxOfLineEditors::addPoint (int identifier)
{
}
void BoxOfLineEditors::removePoint (int identifier)
{
}
void BoxOfLineEditors::addLine (int identifier)
{
    lineModules.push_back(std::make_unique<GraphLineModule>(mappingEngine, delayGraph, identifier));
    addAndMakeVisible(lineModules.back().get());
    resized();

}
void BoxOfLineEditors::removeLine (int identifier)
{
    std::cout << "len before " << lineModules.size() << "\n";
    std::erase_if(lineModules, [&identifier](auto& l) { return identifier == l->getIdentifier();});
    std::cout << "len after " << lineModules.size() << "\n";
    setSize(getWidth(), minimumHeight);
}

void BoxOfLineEditors::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::red);
}

void BoxOfLineEditors::resized()
{
    auto fb = juce::FlexBox();
    fb.flexWrap = juce::FlexBox::Wrap::wrap;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    fb.alignContent = juce::FlexBox::AlignContent::flexStart;

    for (auto& module : lineModules) {
        fb.items.add(juce::FlexItem(*module).withMinWidth(GraphLineModule::getDesiredBounds().getWidth()).withMinHeight(GraphLineModule::getDesiredBounds().getHeight()));
    }
    fb.performLayout(getLocalBounds());
    if (!lineModules.empty()) {
        auto requiredHeight = lineModules.back()->getBounds().getBottom();
        if (requiredHeight > getHeight()) {
            setSize(getWidth(), requiredHeight);
        }
    }
}
