//
// Created by arden on 5/3/24.
//

#ifndef DELAYLINES_MODOVERLAYBUTTON_H
#define DELAYLINES_MODOVERLAYBUTTON_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "ModulatableKey.h"

class ModOverlayButton : public juce::ShapeButton
{
public:
    explicit ModOverlayButton(const ModulatableKey& key, juce::Button::Listener* l)
        : juce::ShapeButton(juce::String(key.pointOrLineId) + "-" + key.parameterId.toString(),
            juce::Colours::magenta.withAlpha(0.3f),
            juce::Colours::magenta.withAlpha(0.5f),
            juce::Colours::magenta.withAlpha(0.8f)),
          modKey(key),
          listener(l)
    {
        addListener(listener);
    }
    const ModulatableKey modKey;
private:
    juce::Button::Listener* listener;
};

#endif //DELAYLINES_MODOVERLAYBUTTON_H
