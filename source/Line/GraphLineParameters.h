//
// Created by arden on 4/15/24.
//

#ifndef DELAYLINES_GRAPHLINEPARAMETERS_H
#define DELAYLINES_GRAPHLINEPARAMETERS_H

#endif //DELAYLINES_GRAPHLINEPARAMETERS_H

#include "../Modulation/ModulatableKey.h"
#include "juce_audio_processors/juce_audio_processors.h"


struct DelayLength {
    DelayLength() : mode(ms), samplesLength(0), millisecondsLength(500), hertz(10), midiNote(100), beatLength({1,4}) {}
    explicit DelayLength(juce::XmlElement* parent);

    enum Mode {
        ms = 0,
        samps = 1,
        hz = 2,
        note = 3,
        beat = 4,
        midiTrack = 5
    };


    float getLengthInSamples(float samplerate, float bpm) {
        switch (mode)
        {
            case ms:
                return millisecondsLength * samplerate / 1000.f;
            case samps:
                return samplesLength;
            case hz:
                return samplerate / hertz;
            case note:
                return samplerate / static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(static_cast<int>(std::round(midiNote))));
            case beat:
                return (std::round(beatLength[0]) / std::round(beatLength[1])) // fraction of bar
                       * 4 // beats / bar
                       * (1 / bpm) // minutes / beat
                       * 60 // seconds / minute
                       * samplerate; // samples / second
            case midiTrack:
                return samplerate / static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(static_cast<int>(std::round(midiTrackNote))));
            default: return 0;
        }
    }

    float getLengthOfDenominator(float samplerate, float bpm) {
        if (juce::approximatelyEqual(beatLength[1], 0.f) || juce::approximatelyEqual(bpm, 0.f)) {
            return 0;
        }
        return (1 / std::round(beatLength[1])) // fraction of bar
               * 4 // beats / bar
               * (1 / bpm) // minutes / beat
               * 60 // seconds / minute
               * samplerate; // samples / second
    }

    void rescale(float proportion) {
        switch (mode) {
            case ms:
                millisecondsLength *= proportion;
                break;
            case samps:
                samplesLength *= proportion;
                break;
            case hz:
                hertz /= proportion;
                break;
            case note: case midiTrack:
                // TODO
                break;
            case beat:
                beatLength[0] *= proportion;
        }
    }

    void setMode(Mode m) {
        mode = m;
    }

    void setSamplesLength(float l) {
        samplesLength = l;
        mode = samps;
    }

    void setMillisecondsLength(float l) {
        millisecondsLength = l;
        mode = ms;
    }

    void setHertz(float h) {
        hertz = h;
        mode = hz;
    }

    void setMidiNote(int n) {
        midiNote = static_cast<float>(n);
        mode = note;
    }

    void setMidiTrackNote(int n) {
        midiTrackNote = static_cast<float>(n);
    }

    void setBeat(int numerator, int denominator) {
        beatLength[0] = static_cast<float>(numerator);
        beatLength[1] = static_cast<float>(denominator);
        mode = beat;
    }

    [[nodiscard]] float getSamplesLength() const {
        return samplesLength;
    }

    [[nodiscard]] float getMillisecondsLength() const {
        return millisecondsLength;
    }

    [[nodiscard]] float getHertz() const {
        return hertz;
    }

    [[nodiscard]] int getMidiNote() const {
        return static_cast<int>(std::round(midiNote));
    }

    [[nodiscard]] int getNumerator() const {
        return static_cast<int>(std::round(beatLength[0]));
    }

    [[nodiscard]] int getDenominator() const {
        return static_cast<int>(std::round(beatLength[1]));
    }

    [[nodiscard]] Mode getMode() const {
        return mode;
    }

    void exportToXml(juce::XmlElement* parent);
    bool importFromXml(juce::XmlElement* parent);

    bool modulateIfPossible(ModulatableKey& key, float newValue);
    bool getModulationValue (ModulatableKey& key, float& val);
private:
    Mode mode;

    float samplesLength;
    float millisecondsLength;
    float hertz;
    float midiNote;
    float midiTrackNote{};
    std::array<float, 2> beatLength;

};


struct Parameters {
    Parameters();

    explicit Parameters(juce::XmlElement* parent);

    enum MuteBypassOptions {
        none = 0,
        mute = 1,
        bypass = 2,
        stagnate = 3
    };

    bool isMuted() const {
        return muteBypass.getIndex() == mute;
    }

    bool isBypassed() const {
        return muteBypass.getIndex() == bypass;
    }

    bool isStagnated() const {
        return muteBypass.getIndex() == stagnate;
    }

    juce::AudioParameterChoice muteBypass;
    DelayLength length;
    juce::AudioParameterFloat lengthEnvelopeFollow;
    juce::AudioParameterFloat modDepth;
    juce::AudioParameterFloat modRate;
    juce::AudioParameterFloat distortion;
    juce::AudioParameterChoice distortionType;
    juce::AudioParameterFloat hiCut;
    juce::AudioParameterFloat loCut;
    juce::AudioParameterFloat gain;
    juce::AudioParameterBool invert;
    juce::AudioParameterFloat gainEnvelopeFollow;
    juce::AudioParameterFloat feedback;

    std::vector<juce::RangedAudioParameter*> params {
        &muteBypass,
        &lengthEnvelopeFollow,
        &modDepth,
        &modRate,
        &distortion,
        &distortionType,
        &hiCut,
        &loCut,
        &gain,
        &invert,
        &gainEnvelopeFollow,
        &feedback
    };

    bool modulateIfPossible(ModulatableKey& key, float newValue);
    bool getModulationValue (ModulatableKey& key, float& val);

    void exportToXml(juce::XmlElement* parent);

    bool importFromXml(juce::XmlElement* parent);
};