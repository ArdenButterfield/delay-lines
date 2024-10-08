//
// Created by arden on 10/8/24.
//

#include "DistortionVisualizer.h"
#include "juce_dsp/juce_dsp.h"
DistortionVisualizer::DistortionVisualizer() : distortionAmount(0), distortionType(0)
{
}
DistortionVisualizer::~DistortionVisualizer()
{
}
void DistortionVisualizer::setDistortion (int type, float amount)
{
    distortionType = type;
    distortionAmount = amount;
    fillValues();
}

void DistortionVisualizer::paint (juce::Graphics& g)
{
    for (auto i = 0; i < values[0].size(); ++i) {
        g.setColour(juce::Colours::black.withAlpha(std::min(std::max(0.f, abs(values[0][i])), 1.f)));
        g.drawVerticalLine(i,0,getHeight() / 2);
        g.setColour(juce::Colours::black.withAlpha(std::min(std::max(0.f, abs(values[1][i])), 1.f)));
        g.drawVerticalLine(i, getHeight() / 2, getBottom());

    }
}

void fillValuesRandom(std::vector<float>& vals, int seed, float distortionAmount) {
    auto randomizer = juce::Random(seed);
    auto nextrandom = 0.f;
    auto ctr = 0;
    for (auto& v : vals) {
        if (ctr % 5 == 0) {
            nextrandom = randomizer.nextFloat();
        }
        if (nextrandom * 1.5 < distortionAmount) {
            v = 0;
        }
        ++ctr;
    }
    std::cout << "\n";
}

void DistortionVisualizer::fillValues()
{
    for (auto& channel : values) {
        for (float i = 0; i < channel.size(); ++i) {
            float halfway = (channel.size() / 2);
            channel[i] = (i - halfway) / halfway;
        }
    }

    switch (distortionType)
    {
        case 0:
            // analog clip
            for (auto& v : values[0]) {
                v = juce::dsp::FastMathApproximations::tanh(v * (1 + distortionAmount * 5));
            }
            values[1] = values[0];
            return;
        case 1:
            // digital clip
            for (auto& v : values[0]) {
                v = std::max (std::min (v * (1 + distortionAmount), 1.f), -1.f);
            }
            values[1] = values[0];
            return;
        case 2:
            // wavefold
            for (auto& v : values[0]) {
                v = sin (v * (distortionAmount * 5 + 1));
            }
            values[1] = values[0];
            return;
        case 3:
            // bitcrush
            for (auto& v : values[0]) {
                auto multiplier = std::pow(2.f, 16 - 16 * distortionAmount);
                v = std::round(v * multiplier) / multiplier;
            }
            values[1] = values[0];
            return;
        case 4:
            // packet loss
            fillValuesRandom(values[0], 28, distortionAmount);
            values[1] = values[0];
            return;
        case 5:
            // packet loss stereo
            fillValuesRandom(values[0], 97405, distortionAmount);
            fillValuesRandom(values[1], 665, distortionAmount);
            return;
        default:
            return;
    }
}
void DistortionVisualizer::resized()
{
    for (auto& value : values) {
        value.resize(getWidth());
    }
    fillValues();
}
