//
// Created by arden on 3/19/24.
//

#ifndef DELAYLINES_GRAPHLINE_H
#define DELAYLINES_GRAPHLINE_H

#include "GraphPoint.h"
#include "juce_dsp/juce_dsp.h"
#include "juce_audio_basics/juce_audio_basics.h"

struct DelayLength {
    DelayLength() : samplesLength(0), millisecondsLength(500), hertz(10), midiNote(100), beatLength({1,4}), mode(ms) {}
    float samplesLength;
    float millisecondsLength;
    float hertz;
    int midiNote;
    std::array<int, 2> beatLength;

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
                return millisecondsLength * 1000.f / samplerate;
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
        midiNote = n;
        mode = note;
    }

    void setBeat(int numerator, int denominator) {
        beatLength[0] = numerator;
        beatLength[1] = denominator;
        mode = beat;
    }
};


struct Parameters {
    Parameters() {
        bypass = false;
        mute = false;
        length = 1000;
        lengthEnvelopeFollow = 0;
        modDepth = 0;
        modRate = 1;
        distortion = 0;
        hiCut = 20000;
        loCut = 0;
        gain = 1;
        invert = false;
        gainEnvelopeFollow = 0;
        feedback = 0;
    }
    bool bypass;
    bool mute;
    float length;
    float lengthEnvelopeFollow;
    float modDepth;
    float modRate;
    float distortion;
    float hiCut;
    float loCut;
    float gain;
    bool invert;
    float gainEnvelopeFollow;
    float feedback;
};


class GraphLine : public juce::Timer {
public:
    GraphLine(GraphPoint* start, GraphPoint* end, const juce::Identifier& id);

    void timerCallback() override;

    void setBypass(bool bypass);
    void setMute(bool mute);

    void setLength(float length);
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
