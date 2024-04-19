//
// Created by arden on 4/15/24.
//

#ifndef DELAYLINES_GRAPHLINEPARAMETERS_H
#define DELAYLINES_GRAPHLINEPARAMETERS_H

#endif //DELAYLINES_GRAPHLINEPARAMETERS_H

#include "juce_audio_processors/juce_audio_processors.h"

struct DelayLength {
    DelayLength() : mode(ms), samplesLength(0), millisecondsLength(500), hertz(10), midiNote(100), beatLength({1,4}) {}
    DelayLength(juce::XmlElement* parent);

    enum Mode {
        ms = 0,
        samps = 1,
        hz = 2,
        note = 3,
        beat = 4
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
                return (std::round(beatLength[0]) / std::round(beatLength[1])) * 60 * samplerate / bpm;
        }
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
            case note:
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

private:
    Mode mode;

    float samplesLength;
    float millisecondsLength;
    float hertz;
    float midiNote;
    std::array<float, 2> beatLength;

};


struct Parameters {
    Parameters() : muteBypass("mutebypass", "mute bypass", {"none", "mute", "bypass"}, 0),
                   lengthEnvelopeFollow("lengthenvelopefollow", "length envelope follow", -1, 1, 0),
                   modDepth("moddepth", "mod depth", 0, 1, 0),
                   modRate("modrate", "mod rate", 0.1, 30, 1),
                   distortion("distortion", "distortion", 0, 1, 0),
                   hiCut("hicut", "hi cut", 100, 20000, 20000),
                   loCut("locut", "lo cut", 0, 2000, 0),
                   gain("gain", "gain", 0, 2, 1),
                   invert("invert", "invert", false),
                   gainEnvelopeFollow("gainenvelopefollow", "gain envelope follow", -1, 1, 0),
                   feedback("feedback", "feedback", 0, 1, 0)
    {}

    Parameters(juce::XmlElement* parent);

    enum MuteBypassOptions {
        none = 0,
        mute = 1,
        bypass = 2
    };

    bool isMuted() const {
        return muteBypass.getIndex() == mute;
    }

    bool isBypassed() const {
        return muteBypass.getIndex() == bypass;
    }

    juce::AudioParameterChoice muteBypass;
    DelayLength length;
    juce::AudioParameterFloat lengthEnvelopeFollow;
    juce::AudioParameterFloat modDepth;
    juce::AudioParameterFloat modRate;
    juce::AudioParameterFloat distortion;
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
        &hiCut,
        &loCut,
        &gain,
        &invert,
        &gainEnvelopeFollow,
        &feedback
    };

    void exportToXml(juce::XmlElement* parent);
    bool importFromXml(juce::XmlElement* parent);

};