//
// Created by arden on 6/4/24.
//

#ifndef DELAYLINES_MODULATIONMAPPINGENGINE_H
#define DELAYLINES_MODULATIONMAPPINGENGINE_H

#include "ModulatableKey.h"

class ModulationEngine;

class ModulationMappingEngine
{
public:
    class Listener {
    public:
        Listener() = default;
        ~Listener() = default;
        virtual void mappingModeEntered() = 0;
        virtual void mappingModeExited() = 0;
    };
    ModulationMappingEngine();

    void setModulationEngine(ModulationEngine* me);

    void enterModMappingMode(int _parameterIndex);

    void exitModMappingMode();

    void setModMap(const ModulatableKey& key);

    [[nodiscard]] bool getInModMappingMode() const;

    void addListener(Listener* listener);

    void removeListener(Listener* listener);

    ModulationEngine* modulationEngine;
private:
    std::set<Listener*> listeners;
    bool inModMappingMode;
    unsigned int parameterIndex{};
};

#endif //DELAYLINES_MODULATIONMAPPINGENGINE_H
