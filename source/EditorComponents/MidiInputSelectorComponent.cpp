//
// Created by arden on 11/1/24.
//

#include "MidiInputSelectorComponent.h"

MidiInputSelectorComponent::MidiInputSelectorComponent(MidiReceiver& mr) : receiver(mr)
{
    midiInputLabel.setText("Midi Input device", juce::dontSendNotification);
    addAndMakeVisible(midiInputLabel);
    addAndMakeVisible(midiInputList);

    auto midiInputs = juce::MidiInput::getAvailableDevices();
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    juce::StringArray midiInputNames;

    for (auto input : midiInputs) {
        midiInputNames.add (input.name);
    }

    midiInputList.addItemList (midiInputNames, 1);
    midiInputList.setSelectedId(receiver.getInputIndex() + 1);
    midiInputList.onChange = [this] { setMidiInput(midiInputList.getSelectedItemIndex()); };

    receiver.addListener(this);
}
MidiInputSelectorComponent::~MidiInputSelectorComponent()
{
    receiver.removeListener(this);
}
void MidiInputSelectorComponent::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::magenta);
    g.fillAll();
}
void MidiInputSelectorComponent::resized()
{
    midiInputLabel.setBounds(getLocalBounds().withWidth(20));
    midiInputList.setBounds(getLocalBounds().withTrimmedLeft(midiInputLabel.getRight()));
}

void MidiInputSelectorComponent::setMidiInput (int index)
{
    receiver.setMidiInput(index);
    midiInputList.setSelectedId (index + 1, juce::dontSendNotification);
}

void MidiInputSelectorComponent::noteCallback (const juce::MidiMessage& message)
{
}
