//
// Created by arden on 11/4/24.
//

#include "PluginProcessor.h"
#include <juce_audio_basics/juce_audio_basics.h>

int main() {
    PluginProcessor pluginProcessor;
    const int blockSize = 512;
    pluginProcessor.prepareToPlay(44100, blockSize);
    auto samples = juce::AudioBuffer<float>(pluginProcessor.getTotalNumInputChannels(), blockSize);
    auto random = juce::Random();
    for (auto channel = 0; channel < samples.getNumChannels(); ++channel) {
        for (auto sample = 0; sample < samples.getNumSamples(); ++sample) {
            samples.setSample(channel, sample, random.nextFloat() - 0.5f);
        }
    }

    auto midiBuffer = juce::MidiBuffer();

    for (int i = 0; i < 100000; ++i) {
        pluginProcessor.processBlock(samples, midiBuffer);
    }
    float i = 0;
    for (auto channel = 0; channel < samples.getNumChannels(); ++channel) {
        for (auto sample = 0; sample < samples.getNumSamples(); ++sample) {
            i += samples.getSample(channel, sample);
        }
    }
    std::cout << i << "\n";

    return 0;
}