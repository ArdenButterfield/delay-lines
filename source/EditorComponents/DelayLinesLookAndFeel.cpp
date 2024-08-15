//
// Created by arden on 8/14/24.
//

#include "DelayLinesLookAndFeel.h"
DelayLinesLookAndFeel::DelayLinesLookAndFeel()
{
}
void DelayLinesLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos,
                                            float minSliderPos,
                                            float maxSliderPos,
                                            const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style == juce::Slider::LinearBarVertical) {
        g.setColour (slider.findColour (juce::Slider::trackColourId));
        g.fillRect (juce::Rectangle<float> (static_cast<float> (x), (float) y + 0.5f, width - sliderPos * static_cast<float>(width) / static_cast<float>(height), (float) height - 1.0f));

        drawLinearSliderOutline (g, x, y, width, height, style, slider);

    } else {
        LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}
