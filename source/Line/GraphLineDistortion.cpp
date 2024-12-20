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
}

void GraphLineDistortion::setDistortionAmount (float amount)
{
    if (!juce::approximatelyEqual(amount, distortionAmount)) {
        distortionAmount = amount;
        envelope.setAttackTime(100 * (1 - distortionAmount));
        envelope.setReleaseTime(500 * (1 - distortionAmount));
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
    int channel;
    switch (distortionType)
    {
        case 0:
            // analog clip
            channel = 0;
            for (auto& s : sample) {
                s = analogDistort(s);
            }
            break;
        case 1:
            // digital clip
            for (auto& s : sample) {
                s = digitalDistort(s);
            }
        case 2:
            // wavefold
            /*

            wet = juce::dsp::FastMathApproximations::sin(samp * (distortionAmount * 5 + 1));
            return distortionAmount * wet + (1 - distortionAmount) * samp;
        case 3:
            // bitcrush
            multiplier = std::pow(2.f, 16 - 16 * distortionAmount);
            return std::round(samp * multiplier) / multiplier;
        case 4:
            // packet loss
            return currentLossState[channel] * samp;
        case 5:
            // packet loss stereo
            return currentLossState[channel] * samp;
*/
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
    }
}
float GraphLineDistortion::analogDistort (float sample) const
{
    // auto s = sample * (1 + distortionAmount * random.nextFloat() - distortionAmount * 0.5);
    auto s = sample;
    return tanhf(s / distortionThresholdGain) * distortionThresholdGain;
}

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
