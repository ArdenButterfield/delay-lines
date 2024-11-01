//
// Created by arden on 11/1/24.
//

#ifndef DELAYLINES_MIDIINPUTSELECTORCOMPONENT_H
#define DELAYLINES_MIDIINPUTSELECTORCOMPONENT_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_devices/juce_audio_devices.h"

#include "../MidiReceiver.h"

class MidiInputSelectorComponent : public juce::Component, private MidiReceiver::Listener
{
public:
    MidiInputSelectorComponent(MidiReceiver& mr);
    ~MidiInputSelectorComponent() override;
private:
    MidiReceiver& receiver;
    juce::ComboBox midiInputList;
    juce::Label midiInputLabel;

    void paint(juce::Graphics &g) override;
    void resized() override;
    void setMidiInput(int index);
    void noteCallback(const juce::MidiMessage &message) override;
};

#endif //DELAYLINES_MIDIINPUTSELECTORCOMPONENT_H
