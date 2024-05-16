//
// Created by arden on 5/11/24.
//

#ifndef DELAYLINES_PARAMETERS_H
#define DELAYLINES_PARAMETERS_H

#endif //DELAYLINES_PARAMETERS_H

#include "juce_audio_processors/juce_audio_processors.h"
#include <array>

const int NUM_MOD_PARAMETERS = 10;

const juce::String MIX_PARAM_ID = "mix";

static juce::String makeModParamId(unsigned index) {
    return "mod" + juce::String(index);
}

static juce::String makeModParamName(unsigned index) {
    return "Modulator " + juce::String(index + 1);
}

const std::array<juce::String,  1> NON_MOD_PARAMETERS {
    MIX_PARAM_ID
};

