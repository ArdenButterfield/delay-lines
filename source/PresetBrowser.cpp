//
// Created by arden on 4/16/24.
//

#include "PresetBrowser.h"


PresetBrowser::PresetBrowser(DelayGraph& dg) : delayGraph(dg), presets(juce::parseXML(
#include "presets/defaults.xml"
))
{
    addAndMakeVisible(presetMenu);
    presetMenu.addListener(this);

    for (int i = 0; i < presets->getNumChildElements(); ++i) {
        auto child = presets->getChildElement(i);
        auto& name = child->getStringAttribute("name");
        presetMenu.addItem(name.isNotEmpty() ? name : "[untitled]", i + 1);
    }
}

PresetBrowser::~PresetBrowser()
{

}

void PresetBrowser::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::magenta);
}

void PresetBrowser::resized()
{
    presetMenu.setBounds(5, 5, getWidth() - 10, 30);
}

void PresetBrowser::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &presetMenu) {
        auto selectedPreset = presetMenu.getSelectedId() - 1;
        delayGraph.importFromXml(presets->getChildElement(selectedPreset)->getChildByName("plugin-state"));
    }
}
