//
// Created by arden on 4/18/24.
//

#include "DelayLineInternal.h"

DelayLineInternal::DelayLineInternal (juce::dsp::ProcessSpec _spec, float initialLength, int maxLength, ModOscillator* _modOscillator)
    : spec(_spec), delayLine(maxLength), envelopeDelayLine(maxLength), modOscillator(_modOscillator), tickLength(0.f)
{
    delayLine.prepare(spec);
    envelopeDelayLine.prepare(spec);
    delayLine.setDelay(initialLength);
    envelopeDelayLine.setDelay(initialLength);
    envelopeFilter.prepare(spec);
    envelopeFilter.setAttackTime(5);
    envelopeFilter.setReleaseTime(100);

    length.setRampLength(spec.sampleRate * stretchTime);
    length.resetToTarget(initialLength);
    lengthFader.setFadeTime(200);
    lengthFader.resetToTarget(initialLength);
    stretchTime = 0;
}

DelayLineInternal::~DelayLineInternal()
= default;

void DelayLineInternal::setTargetLength (float l)
{
    if (!juce::approximatelyEqual(tickLength, 0.f)) {
        l = std::round(l / tickLength) * tickLength;
    }

    auto target = std::min(std::max(0.f, l), static_cast<float>(delayLine.getMaximumDelayInSamples() - 1));
    length.setTarget(target);
    lengthFader.setTarget(target);
}

void DelayLineInternal::pushSample (std::vector<float>& sample)
{
    for (unsigned i = 0; i < spec.numChannels; ++i) {
        delayLine.pushSample(static_cast<int>(i), sample[i]);
        auto envelope = envelopeFilter.processSample(static_cast<int>(i),sample[i]);
        envelopeDelayLine.pushSample(static_cast<int>(i), envelope);
    }
}

void DelayLineInternal::popSample (std::vector<float>& sample, bool updateReadPointer)
{
    if (juce::approximatelyEqual(stretchTime, 0.f)) {
        float firstLength, firstLevel, secondLength, secondLevel;
        lengthFader.getNextValue(firstLength, firstLevel, secondLength, secondLevel);
        if (modOscillator) {
            auto mod = modOscillator->tick();
            firstLength *= mod;
            secondLength *= mod;
        }
        for (unsigned i = 0; i < spec.numChannels; ++i) {
            sample[i] = delayLine.popSample(static_cast<int>(i), firstLength, false) * firstLevel
                        + delayLine.popSample(static_cast<int>(i), secondLength, updateReadPointer) * secondLevel;
            envelopeDelayLine.popSample(static_cast<int>(i), firstLength, updateReadPointer);
        }
        length.getNextValue();
    } else {
        auto l = length.getNextValue();

        if (modOscillator) {
            l *= modOscillator->tick();
        }
        for (unsigned i = 0; i < spec.numChannels; ++i) {
            sample[i] = delayLine.popSample(static_cast<int>(i), l, updateReadPointer);
            envelopeDelayLine.popSample(static_cast<int>(i), l, updateReadPointer);
        }
        lengthFader.tick();
    }
}

void DelayLineInternal::getEnvelope (float proportion, float& left, float& right)
{
    left = envelopeDelayLine.popSample(0, delayLine.getDelay() * proportion, false);
    if (spec.numChannels > 1) {
        right = envelopeDelayLine.popSample(1, delayLine.getDelay() * proportion, false);
    } else {
        right = left;
    }
}

void DelayLineInternal::clearLines()
{
    envelopeDelayLine.reset();
    delayLine.reset();
}

void DelayLineInternal::setTick (float _tickLength)
{
    tickLength = _tickLength;
}

void DelayLineInternal::setStretchTime(float _stretchTime)
{
    if (!juce::approximatelyEqual(stretchTime, _stretchTime)) {
        stretchTime = _stretchTime;
        length.setRampLength(stretchTime);
    }
}
