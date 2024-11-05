//
// Created by arden on 4/18/24.
//

#ifndef DELAYLINES_MODOSCILLATOR_H
#define DELAYLINES_MODOSCILLATOR_H

#include "juce_dsp/juce_dsp.h"

class ModOscillator {
public:
    ModOscillator(juce::dsp::ProcessSpec& spec, float initialRate, float initialDepth)
    {
        history.resize(static_cast<unsigned>(spec.sampleRate * 1));
        historyPostion = 0;

        oscillator.initialise([] (float x) {return std::sin(x);}, 128);
        modDepth.reset(512);
        oscillator.prepare(spec);
        depthParam = initialDepth;
        setFrequency(initialRate);
        currentValue = 0;
    }

    void setFrequency(float rate)
    {
        oscillator.setFrequency(rate);
        recalculateDepth(rate, depthParam);
    }
    void setDepth(float depth)
    {
        depthParam = depth;
        recalculateDepth(oscillator.getFrequency(), depth);
    }

    float getDepth() const {
        return depthParam;
    }

    float tick()
    {
        currentValue = oscillator.processSample(0);
        history[historyPostion] = depthParam * currentValue;
        ++historyPostion;
        historyPostion %= history.size();
        return 1 + modDepth.getNextValue() * currentValue;
    }

    void copyOutHistory(std::vector<float>& lows, std::vector<float>& highs, bool scroll) const {
        jassert(lows.size() == highs.size());
        jassert(lows.size() <= history.size());

        for (unsigned i = 0; i < lows.size(); ++i) {
            auto minVal = 1.0f;
            auto maxVal = -1.0f;
            for (auto j = i * history.size() / lows.size(); j < (i + 1) * history.size() / lows.size(); ++j) {
                auto v = history[scroll ? (j + historyPostion) % history.size() : j];
                if (v < minVal) {
                    minVal = v;
                }
                if (v > maxVal) {
                    maxVal = v;
                }
            }
            lows[i] = minVal;
            highs[i] = maxVal;
        }
    }

    float getCurrentValueWithoutTicking() const { return currentValue * depthParam; }

private:
    void recalculateDepth(float freq, float depth) {
        modDepth.setTargetValue(depth / (freq * juce::MathConstants<float>::twoPi * 10));
    }

    float depthParam;
    juce::dsp::Oscillator<float> oscillator;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modDepth;
    float currentValue;
    std::vector<float> history;
    int historyPostion;
};

#endif //DELAYLINES_MODOSCILLATOR_H

