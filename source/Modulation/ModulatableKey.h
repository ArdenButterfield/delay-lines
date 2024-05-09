//
// Created by arden on 5/3/24.
//

#ifndef DELAYLINES_MODULATABLEKEY_H
#define DELAYLINES_MODULATABLEKEY_H

#include "juce_core/juce_core.h"

struct ModulatableKey
{
    enum Type {
        point,
        line
    };

    Type type;

    int pointOrLineId;
    juce::Identifier parameterId;
};

#endif //DELAYLINES_MODULATABLEKEY_H
