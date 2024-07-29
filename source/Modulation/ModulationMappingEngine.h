//
// Created by arden on 6/4/24.
//

#ifndef DELAYLINES_MODULATIONMAPPINGENGINE_H
#define DELAYLINES_MODULATIONMAPPINGENGINE_H

#include "ModulationEngine.h"
#include "ModulatableKey.h"

class ModulationMappingEngine
{
public:
    ModulationMappingEngine() : inModMappingMode(false), modulationEngine(nullptr) {}

    void setModulationEngine(ModulationEngine* me) {
        modulationEngine = me;
    }

    void enterModMappingMode(int _parameterIndex) {
        parameterIndex = _parameterIndex;
        inModMappingMode = true;
    }

    void setModMap(const ModulatableKey& key) {
        if (inModMappingMode) {
            modulationEngine->setMapping(parameterIndex, key);
            inModMappingMode = false;
        }
    }

    bool getInModMappingMode() {
        return inModMappingMode;
    }
private:
    bool inModMappingMode;
    unsigned int parameterIndex;
    ModulationEngine* modulationEngine;
};

#endif //DELAYLINES_MODULATIONMAPPINGENGINE_H
