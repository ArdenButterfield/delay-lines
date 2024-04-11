//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHLINE_H
#define DELAYLINES_GRAPHLINE_H

#include "GraphPoint.h"
#include "juce_dsp/juce_dsp.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"

struct DelayLength {
    DelayLength() : samplesLength(0), millisecondsLength(500), hertz(10), midiNote(100), beatLength({1,4}), mode(ms) {}

    enum Mode {
        ms,
        samps,
        hz,
        note,
        beat
    };

    Mode mode;

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
                return samplerate / static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNote));
            case beat:
                return (static_cast<float>(beatLength[0]) / static_cast<float>(beatLength[1])) * 60 * samplerate / bpm;
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
        }
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

    float getSamplesLength() const {
        return samplesLength;
    }

    float getMillisecondsLength() const {
        return millisecondsLength;
    }

    float getHertz() const {
        return hertz;
    }

    int getMidiNote() const {
        return static_cast<int>(std::round(midiNote));
    }

    int getNumerator() const {
        return static_cast<int>(std::round(beatLength[0]));
    }

    int getDenominator() const {
        return static_cast<int>(std::round(beatLength[1]));
    }

private:
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
};


class GraphLine : public juce::Timer {
public:
    GraphLine(GraphPoint* start, GraphPoint* end, const juce::Identifier& id);

    void timerCallback() override;

    void setBypass(bool bypass);
    void setMute(bool mute);

    void setLengthEnvelopeFollow(float amt);
    void setModDepth(float depth);
    void setModRate(float rate);
    void setDistortionAmount(float amt);
    void setLowCutFilter(float freq);
    void setHighCutFilter(float freq);
    void setGain(float gain);

    void setInvert(bool invert);
    void setGainEnvelopeFollow(float amt);
    void setFeedback(float amt);
    GraphPoint* const start;

    GraphPoint* const end;
    void pushSample(std::vector<float>& sample);
    void popSample();
    void prepareToPlay(juce::dsp::ProcessSpec* spec);
    void toggleEnabled();
    void getEnvelope(float proportion, float& left, float& right);

    void bakeOffset();

    Parameters parameters;

    std::vector<GraphPoint*> popDestinations;
    bool editorAttached;

    bool prepared;
    const juce::Identifier identifier;

    std::set<GraphPoint*> realOutputs;

private:
    void calculateInternalLength();

    unsigned numChannels;
    float sampleRate;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> internalDelayLine;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> envelopeDelayLine;
    juce::dsp::BallisticsFilter<float> envelopeFilter;
    std::vector<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>> lengths;

    std::vector<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>> gains;
    const float defaultGain = 1.0;
    const float defaultLength = 1.0;

    float distortSample(float samp);
    std::vector<float> lastSample;
    std::vector<juce::IIRFilter> loCutFilters;
    std::vector<juce::IIRFilter> hiCutFilters;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modDepth;
    juce::dsp::Oscillator<float> modOscillator;
};
#endif //DELAYLINES_GRAPHLINE_H
