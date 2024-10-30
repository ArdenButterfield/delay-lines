//
// Created by arden on 10/29/24.
//

#ifndef DELAYLINES_TITLEGRAPHIC_H
#define DELAYLINES_TITLEGRAPHIC_H

#include "juce_gui_basics/juce_gui_basics.h"

class TitleGraphic : public juce::Component
{
public:
    TitleGraphic();
    ~TitleGraphic();

    void paint(juce::Graphics &g) override;
};

#endif //DELAYLINES_TITLEGRAPHIC_H
