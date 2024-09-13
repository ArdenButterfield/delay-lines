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
    float tick()
    {
        currentValue = oscillator.processSample(0);
        return 1 + modDepth.getNextValue() * currentValue;
    }

    float getCurrentValueWithoutTicking() { return currentValue * depthParam; }

private:
    void recalculateDepth(float freq, float depth) {
        modDepth.setTargetValue(depth / (freq * juce::MathConstants<float>::twoPi * 10));
    }

    float depthParam;
    juce::dsp::Oscillator<float> oscillator;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modDepth;
    float currentValue;
};

#endif //DELAYLINES_MODOSCILLATOR_H

