//
// Created by arden on 11/1/24.
//

#ifndef DELAYLINES_MIDIRECEIVER_H
#define DELAYLINES_MIDIRECEIVER_H

#include "juce_audio_devices/juce_audio_devices.h"
#include <unordered_set>

class MidiReceiver : private juce::MidiInputCallback
{
public:
    class Listener {
    public:
        Listener() = default;
        ~Listener() = default;
        virtual void noteCallback(const juce::MidiMessage& message) {}
    };
    MidiReceiver();
    ~MidiReceiver() override;
    int getInputIndex();
    void setMidiInput(int index);
    void addListener(Listener* l) {
        listeners.insert(l);
    }
    void removeListener(Listener* l) {
        listeners.erase(l);
    }
private:
    std::unordered_set<Listener*> listeners;
    juce::AudioDeviceManager deviceManager;
    int lastInputIndex;
    void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override;
};

#endif //DELAYLINES_MIDIRECEIVER_H
