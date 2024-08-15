//
// Created by arden on 8/14/24.
//

#ifndef DELAYLINES_DELAYLINESLOOKANDFEEL_H
#define DELAYLINES_DELAYLINESLOOKANDFEEL_H

#include "juce_gui_basics/juce_gui_basics.h"

class DelayLinesLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DelayLinesLookAndFeel();
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle, juce::Slider & slider) override;
};

#endif //DELAYLINES_DELAYLINESLOOKANDFEEL_H
