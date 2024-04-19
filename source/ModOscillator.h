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
        return 1 + modDepth.getNextValue() * oscillator.processSample(0);
    }

private:
    void recalculateDepth(float freq, float depth) {
        modDepth.setTargetValue(depth / (freq * juce::MathConstants<float>::twoPi * 10));
    }

    float depthParam;
    juce::dsp::Oscillator<float> oscillator;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modDepth;

};

#endif //DELAYLINES_MODOSCILLATOR_H

