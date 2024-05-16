//
// Created by arden on 4/18/24.
//

#include "DelayLineInternal.h"

DelayLineInternal::DelayLineInternal (juce::dsp::ProcessSpec _spec, float initialLength, int maxLength, ModOscillator* _modOscillator)
    : spec(_spec), delayLine(maxLength), envelopeDelayLine(maxLength), modOscillator(_modOscillator)
{
    delayLine.prepare(spec);
    envelopeDelayLine.prepare(spec);
    delayLine.setDelay(initialLength);
    envelopeDelayLine.setDelay(initialLength);
    envelopeFilter.prepare(spec);
    envelopeFilter.setAttackTime(5); // TODO: tweak these
    envelopeFilter.setReleaseTime(50);

    length.setCurrentAndTargetValue(initialLength);
    length.reset(spec.sampleRate, 0.2);
}

DelayLineInternal::~DelayLineInternal()
= default;

void DelayLineInternal::setTargetLength (float l)
{
    l = std::min(l, (float)delayLine.getMaximumDelayInSamples() - 1);
    l = std::max(l, 0.f);

    length.setTargetValue(l);
}

void DelayLineInternal::pushSample (std::vector<float>& sample)
{
    for (unsigned i = 0; i < spec.numChannels; ++i) {
        delayLine.pushSample(static_cast<int>(i), sample[i]);
        auto envelope = envelopeFilter.processSample(static_cast<int>(i),sample[i]);
        envelopeDelayLine.pushSample(static_cast<int>(i), envelope);
    }
}

void DelayLineInternal::popSample (std::vector<float>& sample)
{
    auto l = length.getNextValue();


    if (modOscillator) {
        l *= modOscillator->tick();
    }
    for (unsigned i = 0; i < spec.numChannels; ++i) {
        sample[i] = delayLine.popSample(static_cast<int>(i), l, true);
        envelopeDelayLine.popSample(static_cast<int>(i), l, true);
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
