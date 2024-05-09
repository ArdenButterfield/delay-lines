//
// Created by arden on 5/6/24.
//

#ifndef DELAYLINES_MODULATIONENGINE_H
#define DELAYLINES_MODULATIONENGINE_H

#include "juce_audio_processors/juce_audio_processors.h"
#include "../DelayGraph.h"
#include "ModulatableKey.h"

class ModulationEngine : juce::AudioProcessorParameter::Listener
{
public:
    ModulationEngine(unsigned int numParameters, DelayGraph& delayGraph);
    ~ModulationEngine() override;

    std::vector<std::unique_ptr<juce::AudioParameterFloat>>& getParameters();
    float getParameterValue(unsigned int index);
    void setParameterValue(unsigned int index, float value);
    void setMapping(unsigned int index, ModulatableKey key);
    void clearMapping(unsigned int index);
    bool isMapped(unsigned int index);
private:
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {
        juce::ignoreUnused(parameterIndex);
        juce::ignoreUnused(gestureIsStarting);
    }
    std::vector<std::unique_ptr<ModulatableKey>> mappings;
    std::vector<std::unique_ptr<juce::AudioParameterFloat>> parameters;
    DelayGraph& delayGraph;

};

#endif //DELAYLINES_MODULATIONENGINE_H
