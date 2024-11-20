//
// Created by arden on 10/30/24.
//

#ifndef DELAYLINES_MODMAPBUTTON_H
#define DELAYLINES_MODMAPBUTTON_H

#include "juce_gui_basics/juce_gui_basics.h"

class ModMapButton : public juce::ToggleButton
{
public:
    ModMapButton(juce::Colour _splashColour, const juce::String& buttonText);
    ~ModMapButton() = default;
private:
    void paintButton(juce::Graphics &, bool, bool) override;
    juce::Colour splashColour;
    juce::Font font;
};

#endif //DELAYLINES_MODMAPBUTTON_H
