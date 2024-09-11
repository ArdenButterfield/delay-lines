//
// Created by arden on 9/10/24.
//

#ifndef DELAYLINES_FILTERSLIDER_H
#define DELAYLINES_FILTERSLIDER_H

#include "juce_gui_basics/juce_gui_basics.h"

class FilterSlider : public juce::Slider
{
public:
    FilterSlider();
    ~FilterSlider() override;
    void paint(juce::Graphics &) override;
    void resized() override;
};

#endif //DELAYLINES_FILTERSLIDER_H
