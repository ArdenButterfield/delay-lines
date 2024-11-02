//
// Created by arden on 11/1/24.
//

#include "MidiReceiver.h"
MidiReceiver::MidiReceiver()
{
    std::cout << "midi receiver\n";
    auto midiInputs = juce::MidiInput::getAvailableDevices();

    lastInputIndex = -1;
    // find the first enabled device and use that by default
    for (auto input : midiInputs)
    {
        if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
        {
            setMidiInput (midiInputs.indexOf (input));
            break;
        }
    }
    // if no enabled devices were found just use the first one in the list
    if (lastInputIndex < 0) {
        setMidiInput (0);
    }
}

MidiReceiver::~MidiReceiver()
{
    deviceManager.removeMidiInputDeviceCallback (
        juce::MidiInput::getAvailableDevices()[lastInputIndex].identifier, this);
}

void MidiReceiver::setMidiInput (int index)
{
    auto list = juce::MidiInput::getAvailableDevices();

    deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);

    auto newInput = list[index];

    if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

    deviceManager.addMidiInputDeviceCallback (newInput.identifier, this);

    std::cout << "midi input set to " << newInput.name << "\n";
    lastInputIndex = index;
}

void MidiReceiver::handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message)
{
    std::cout << message.getDescription() << "\n";
    for (auto& listener : listeners) {
        listener->noteCallback(message);
    }
}
int MidiReceiver::getInputIndex() {
    if (lastInputIndex >= juce::MidiInput::getAvailableDevices().size()) {
        lastInputIndex = 0;
    }
    return lastInputIndex;
}
