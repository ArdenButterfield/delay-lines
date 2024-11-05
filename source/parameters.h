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
const juce::String STRETCH_TIME_ID = "stretchtime";
const juce::String CLEAR_PARAM_ID = "clear";
static juce::String makeModParamId(unsigned index) {
    return "mod" + juce::String(index);
}

static juce::String makeModParamName(unsigned index) {
    return "Modulator " + juce::String(index + 1);
}

const std::array<juce::String, 3> NON_MOD_PARAMETERS {
    MIX_PARAM_ID,
    STRETCH_TIME_ID,
    CLEAR_PARAM_ID
};


const juce::String MUTE_BYPASS_ID = "mutebypass";
const juce::String LENGTH_ENVELOPE_FOLLOW_ID = "lengthenvelopefollow";
const juce::String MOD_DEPTH_ID = "moddepth";
const juce::String MOD_RATE_ID = "modrate";
const juce::String DISTORTION_ID = "distortion";
const juce::String DISTORTION_TYPE_ID = "distortiontype";
const juce::String HICUT_ID = "hicut";
const juce::String LOCUT_ID = "locut";
const juce::String GAIN_ID = "gain";
const juce::String INVERT_ID = "invert";
const juce::String GAIN_ENVELOPE_FOLLOW_ID = "gainenvelopefollow";
const juce::String FEEDBACK_ID = "feedback";
const juce::String PAN_PARAMETER_ID = "pan";

const juce::StringArray DISTORTION_TYPE_OPTIONS = {"analog clip", "digital clip", "wave fold", "bitcrush", "packetloss", "packetloss stereo"};