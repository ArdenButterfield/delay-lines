//
// Created by arden on 8/14/24.
//


#include "DelayLinesLookAndFeel.h"
#include "BinaryData.h"
#include "juce_gui_basics/juce_gui_basics.h"

DelayLinesLookAndFeel::DelayLinesLookAndFeel()
{
    setDefaultSansSerifTypeface(juce::Typeface::createSystemTypefaceFor(BinaryData::SonoRegular_ttf, BinaryData::SonoRegular_ttfSize));
    setColour(juce::ToggleButton::ColourIds::textColourId, juce::Colours::black);
    setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::black);
    setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::grey);
    setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::grey.withAlpha(0.2f));
    setColour(juce::ComboBox::ColourIds::textColourId, juce::Colours::black);
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

void DelayLinesLookAndFeel::drawToggleButton (juce::Graphics& g , juce::ToggleButton& b, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setFont(getMonoFont());
    LookAndFeel_V4::drawToggleButton (g, b, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

juce::Font DelayLinesLookAndFeel::getPopupMenuFont()
{
    return juce::Font(getMonoFont()).withHeight(17.0f);
}
juce::Font DelayLinesLookAndFeel::getComboBoxFont (juce::ComboBox& box)
{
    return juce::Font(getMonoFont()).withHeight(juce::jmin (16.0f, (float) box.getHeight() * 0.85f));

}
void DelayLinesLookAndFeel::drawGraphPaperBackround (juce::Graphics& g, juce::Point<float> offset, juce::Component* component)
{
    const float gridSize = 10;
    g.setColour(juce::Colours::white);
    g.fillAll();
    g.setColour(juce::Colours::lightblue.withAlpha(0.2f));
    for (auto y = std::fmod(offset.y, gridSize) - gridSize; y < component->getHeight() + gridSize; y += gridSize) {
        g.drawHorizontalLine(y, 0, component->getWidth());
    }
    for (auto x = std::fmod(offset.x, gridSize) - gridSize; x < component->getWidth() + gridSize; x += gridSize) {
        g.drawVerticalLine(x, 0, component->getHeight());
    }
}