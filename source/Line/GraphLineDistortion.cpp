//
// Created by arden on 12/15/24.
//

#include "GraphLineDistortion.h"
void GraphLineDistortion::prepareToplay (juce::dsp::ProcessSpec& spec)
{
    previousSample.resize(spec.numChannels);
    previousSample.clear();
}
void GraphLineDistortion::setDistortionAmount (float amount)
{
    if (!juce::approximatelyEqual(amount, distortionAmount)) {
        distortionAmount = amount;
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
                s *= (1 + distortionAmount * random.nextFloat() - distortionAmount * 0.5);
                s = tanhf(s / distortionThresholdGain) * distortionThresholdGain;
            }
            break;
        case 1:
            // digital clip
            for (auto& s : sample) {
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
            }
            /*
        case 2:
            // wavefold
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
