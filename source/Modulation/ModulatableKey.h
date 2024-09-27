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

    juce::NormalisableRange<double> range;

    float get(float modPosition) {
        return range.convertFrom0to1(modPosition);
    }
};

#endif //DELAYLINES_MODULATABLEKEY_H
