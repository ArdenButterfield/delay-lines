//
// Created by arden on 8/15/24.
//

#ifndef DELAYLINES_PITCHCOMPACTSLIDER_H
#define DELAYLINES_PITCHCOMPACTSLIDER_H

#include "CompactSlider.h"
#include "juce_audio_basics/juce_audio_basics.h"
class PitchCompactSlider : public CompactSlider
{
public:
    juce::String getDisplayText(double rawValue) override {
        return juce::MidiMessage::getMidiNoteName(static_cast<int>(rawValue), true, true, 4);
    }
};

#endif //DELAYLINES_PITCHCOMPACTSLIDER_H
