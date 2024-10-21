//
// Created by arden on 5/16/24.
//

#ifndef DELAYLINES_MODKNOBS_H
#define DELAYLINES_MODKNOBS_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "ModulationMappingEngine.h"

#include "ModKnob.h"

class ModKnobs : public juce::Component
{
public:
    ModKnobs();
    ~ModKnobs() override;
    void setMappingEngine(ModulationMappingEngine* me);
private:
    std::vector<std::unique_ptr<ModKnob>> modKnobs;
    void paint(juce::Graphics &g) override;
    void resized() override;
    ModulationMappingEngine* mappingEngine;
};

#endif //DELAYLINES_MODKNOBS_H
