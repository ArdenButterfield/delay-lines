//
// Created by arden on 9/26/24.
//

#include "ModulatableSlider.h"
#include "../../Modulation/ModOverlayButton.h"
#include "../../Modulation/ModulatableKey.h"
#include "../../Modulation/ModulationMappingEngine.h"

ModulatableSlider::ModulatableSlider() : mappingEngine(nullptr)
{
}

ModulatableSlider::~ModulatableSlider()
{
}

void ModulatableSlider::setMappingEngine (ModulationMappingEngine* me)
{
    mappingEngine = me;
    if (modKey) {
        makeNewOverlay();
    }
}

void ModulatableSlider::setModKey (ModulatableKey key)
{
    modKey = std::make_unique<ModulatableKey>(key);
    if (mappingEngine) {
        makeNewOverlay();
    }
}
void ModulatableSlider::makeNewOverlay()
{
    if (overlayButton) {
        removeChildComponent(overlayButton.get());
    }
    overlayButton = std::make_unique<ModOverlayButton>(*mappingEngine, *modKey);
    addChildComponent(overlayButton.get());
}
void ModulatableSlider::resized()
{
    if (overlayButton) {
        overlayButton->setBounds(getLocalBounds());
    }
}
