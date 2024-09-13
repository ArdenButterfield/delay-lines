//
// Created by arden on 9/10/24.
//

#include "FilterSlider.h"

FilterSlider::FilterSlider()
{
    setSliderStyle(juce::Slider::SliderStyle::LinearBar);
}

FilterSlider::~FilterSlider()
{
}

void FilterSlider::paint (juce::Graphics& g)
{
    Slider::paint (g);
}


void FilterSlider::resized()
{
    Slider::resized();
}
