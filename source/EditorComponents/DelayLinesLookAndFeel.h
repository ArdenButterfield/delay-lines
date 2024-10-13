//
// Created by arden on 8/14/24.
//

#ifndef DELAYLINES_DELAYLINESLOOKANDFEEL_H
#define DELAYLINES_DELAYLINESLOOKANDFEEL_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "BinaryData.h"
class DelayLinesLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DelayLinesLookAndFeel();
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle, juce::Slider & slider) override;
    static juce::Typeface::Ptr getMonoFont() {
        return juce::Typeface::createSystemTypefaceFor(BinaryData::SonoRegular_ttf, BinaryData::SonoRegular_ttfSize);
    }
    void drawToggleButton(juce::Graphics &, juce::ToggleButton &, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    juce::Font getPopupMenuFont() override;
    juce::Font getComboBoxFont(juce::ComboBox &) override;

    static void drawGraphPaperBackround(juce::Graphics& g, juce::Point<float> offset, juce::Component* component);
    static const float getGraphPaperGridSize() { return 20.f; }
};

#endif //DELAYLINES_DELAYLINESLOOKANDFEEL_H
