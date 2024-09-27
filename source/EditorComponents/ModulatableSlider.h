//
// Created by arden on 9/26/24.
//

#ifndef DELAYLINES_MODULATABLESLIDER_H
#define DELAYLINES_MODULATABLESLIDER_H

#include "juce_gui_basics/juce_gui_basics.h"


class ModulationMappingEngine;
class ModulatableKey;
class ModOverlayButton;

class ModulatableSlider : public juce::Slider
{
public:
    ModulatableSlider();
    ~ModulatableSlider() override;
    void setMappingEngine(ModulationMappingEngine* me);
    void setModKey(ModulatableKey key);
    void resized() override;
private:
    void makeNewOverlay();
    ModulationMappingEngine* mappingEngine;
    std::unique_ptr<ModulatableKey> modKey;
    std::unique_ptr<ModOverlayButton> overlayButton;
};

#endif //DELAYLINES_MODULATABLESLIDER_H
