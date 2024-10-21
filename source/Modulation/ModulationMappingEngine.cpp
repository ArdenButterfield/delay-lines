//
// Created by arden on 6/4/24.
//

#include "ModulationMappingEngine.h"
#include "ModulationEngine.h"

ModulationMappingEngine::ModulationMappingEngine() : modulationEngine(nullptr), inModMappingMode(false) {}
void ModulationMappingEngine::setModulationEngine (ModulationEngine* me)
{
    modulationEngine = me;
}
void ModulationMappingEngine::enterModMappingMode (int _parameterIndex)
{
    parameterIndex = static_cast<unsigned int> (_parameterIndex);
    inModMappingMode = true;
    for (auto& listener : listeners) {
        listener->mappingModeEntered(parameterIndex);
    }
}
void ModulationMappingEngine::exitModMappingMode()
{
    inModMappingMode = false;
    for (auto& listener : listeners) {
        listener->mappingModeExited();
    }
}
void ModulationMappingEngine::setModMap (const ModulatableKey& key)
{
    if (inModMappingMode) {
        modulationEngine->setMapping(parameterIndex, key);
        exitModMappingMode();
    }
}
bool ModulationMappingEngine::getInModMappingMode() const
{
    return inModMappingMode;
}
void ModulationMappingEngine::addListener (ModulationMappingEngine::Listener* listener)
{
    listeners.insert(listener);
}
void ModulationMappingEngine::removeListener (ModulationMappingEngine::Listener* listener)
{
    if (listeners.contains(listener)) {
        listeners.erase(listener);
    }
}
