//
// Created by arden on 12/15/24.
//

#include "GraphLineDistortion.h"
#include "GraphLine.h"

void GraphLineDistortion::prepareToplay (juce::dsp::ProcessSpec& spec)
{
    previousSample.resize(spec.numChannels);
    previousSample.clear();

    envelope.prepare(spec);
    gateEnvelope.prepare(spec);

    minGainReduction = 1.f;
    maxGainReduction = 0.f;
    startTimerHz(60);

    gateEnvelope.setAttackTime(0);
    gateEnvelope.setReleaseTime(100);

    envelope.setAttackTime(0);
    envelope.setReleaseTime(250);
}

void GraphLineDistortion::setDistortionAmount (float amount)
{
    if (!juce::approximatelyEqual(amount, distortionAmount)) {
        distortionAmount = amount;
        envelope.setReleaseTime(500 * (1 - distortionAmount));
        gateEnvelope.setReleaseTime(500 * (1 - distortionAmount));
    }
}

void GraphLineDistortion::setDistortionThreshold (float db)
{
    if (!juce::approximatelyEqual(db, distortionThreshold)) {
        distortionThreshold = db;
        distortionThresholdGain = juce::Decibels::decibelsToGain(db);
    }
}

void GraphLineDistortion::setDistortionType (int type)
{
    distortionType = type;
}

void GraphLineDistortion::distortSample (std::vector<float>& sample)
{

    for (auto& s : sample) {
        if (!std::isfinite(s)) {
            s = 0;
        }
    }

    switch (distortionType)
    {
        case 0:
            limiterDistort(sample);
            break;
        case 1:
            analogDistort(sample);
            break;
        case 2:
            sineDistort(sample);
            break;
        case 3:
            gateDistort(sample, 0);
            break;
        case 4:
            gateDistort(sample, 1.f);
            break;
        default:
            break;
    }
    for (int i = 0; i < sample.size(); ++i) {
        previousSample = sample;
    }
}

void GraphLineDistortion::paintComponent (juce::Graphics& g, juce::Component& c) const
{
    switch (distortionType) {
/*
        case 0: {
            auto p = juce::Path();
            p.startNewSubPath (0, 0);
            for (float x = 0; x < c.getWidth(); x += 0.5)
            {
                auto t = (x - (c.getWidth() * 0.5f)) / static_cast<float> (c.getHeight());
                p.lineTo (x, c.getHeight() * analogDistort (t) * 0.5f + c.getHeight() * 0.5f);
            }
            g.strokePath (p, { 4, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });
            break;
        }
        case 1: {
            auto p = juce::Path();
            p.startNewSubPath (0, 0);
            for (float x = 0; x < c.getWidth(); x += 0.5)
            {
                auto t = (x - (c.getWidth() * 0.5f)) / static_cast<float> (c.getHeight());
                p.lineTo (x, c.getHeight() * digitalDistort(t) * 0.5f + c.getHeight() * 0.5f);
            }
            g.strokePath (p, { 4, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });
            break;
        }
*/
        case 0: case 3: case 4: {
            g.fillRect(c.getLocalBounds().withWidth(c.getWidth() * std::min(std::max(prevMinGainReduction, 0.f), 1.f)).withRightX(c.getWidth()));
            g.drawRect(c.getLocalBounds().withWidth(c.getWidth() * std::min(std::max(prevMaxGainReduction, 0.f), 1.f)).withRightX(c.getWidth()));
        }
    }
}
void GraphLineDistortion::analogDistort (std::vector<float>& sample)
{
    for (auto& s : sample) {
        s = tanh (s / distortionThresholdGain) * distortionThresholdGain;
    }
}

void GraphLineDistortion::limiterDistort (std::vector<float>& sample) {
    for (int channel = 0; channel < sample.size(); ++channel) {
        auto env = envelope.processSample(channel, sample[channel]);
        auto gainScale = (env > distortionThresholdGain) ? (distortionThresholdGain / env) : 1;
        sample[channel] *= gainScale;
        auto gainReduction = 1 - gainScale;
        maxGainReduction = std::max(gainReduction, maxGainReduction);
        minGainReduction = std::min(gainReduction, minGainReduction);
    }
}

void GraphLineDistortion::sineDistort (std::vector<float>& sample) {
    for (auto& s : sample) {
        s = sin (s / distortionThresholdGain) * distortionThresholdGain;
    }
}

void GraphLineDistortion::gateDistort (std::vector<float>& sample, float chaos) {
    for (int channel = 0; channel < sample.size(); ++channel) {
        auto env = envelope.processSample (channel, sample[channel]);
        auto gainScale = (env > distortionThresholdGain || juce::approximatelyEqual (distortionThresholdGain, 0.f)) ? 1 : (env / distortionThresholdGain);
        sample[channel] *= gainScale;
        sample[channel] = tanh(sample[channel]);
        auto gainReduction = 1 - gainScale;
        maxGainReduction = std::max (gainReduction, maxGainReduction);
        minGainReduction = std::min (gainReduction, minGainReduction);
    }
}

/*
float GraphLineDistortion::digitalDistort (float s) const
{
    if (s < -distortionThresholdGain) {
        float scaledAmount = 10.f * std::pow(distortionAmount, 4.f);
        s = scaledAmount * (-s / distortionThresholdGain - 1);
        s = std::abs(std::fmod(s, 4.f) - 2.f);
        s *= distortionThresholdGain;
        s -= distortionThresholdGain;
        s *= -1;
    } else if (s > distortionThresholdGain) {
        float scaledAmount = 10.f * std::pow(distortionAmount, 4.f);
        s = scaledAmount * (s / distortionThresholdGain - 1);
        s = std::abs(std::fmod(s, 4.f) - 2.f);
        s *= distortionThresholdGain;
        s -= distortionThresholdGain;
    }
    return s;
}
*/

void GraphLineDistortion::timerCallback()
{
    prevMinGainReduction = minGainReduction;
    prevMaxGainReduction = maxGainReduction;
    minGainReduction = 1.f;
    maxGainReduction = 0.f;
}
