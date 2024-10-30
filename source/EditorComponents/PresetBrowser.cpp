//
// Created by arden on 4/16/24.
//

#include "PresetBrowser.h"
#include "DelayLinesLookAndFeel.h"

PresetBrowser::PresetBrowser(DelayGraph& dg) : delayGraph(dg), presets(juce::parseXML(
#include "../presets/defaults.xml"
))
{
    addAndMakeVisible(presetMenu);
    presetMenu.addListener(this);

    for (int i = 0; i < presets->getNumChildElements(); ++i) {
        auto child = presets->getChildElement(i);
        auto& name = child->getStringAttribute("name");
        presetMenu.addItem(name.isNotEmpty() ? name : "[untitled]", i + 1);
    }

    titleLabel.setText("Presets", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(DelayLinesLookAndFeel::getMonoFont()).withHeight(16.0f));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);
}

PresetBrowser::~PresetBrowser()
= default;

void PresetBrowser::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::lightgrey);
}

void PresetBrowser::resized()
{
    titleLabel.setBounds(getLocalBounds().withWidth(90));
    presetMenu.setBounds(getLocalBounds().reduced(3).withLeft(titleLabel.getRight()));
}

void PresetBrowser::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &presetMenu) {
        auto selectedPreset = presetMenu.getSelectedId() - 1;
        delayGraph.importFromXml(presets->getChildElement(selectedPreset)->getChildByName("plugin-state"));
    }
}
