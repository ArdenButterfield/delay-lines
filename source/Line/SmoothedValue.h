//
// Created by arden on 10/16/24.
//

#ifndef DELAYLINES_SMOOTHEDVALUE_H
#define DELAYLINES_SMOOTHEDVALUE_H

#include "juce_core/juce_core.h"

// Uses cubic Bézier curves to have continuous value and first derivative across slides
// note that, since we always want to end up with a derivative of 0, the last two points
// of our curve have the same value.
class SmoothedValue
{
public:
    SmoothedValue() {
        rampLength = 1;
    }
    ~SmoothedValue() = default;

    void resetToTarget(float target) {
        isInMotion = false;
        proportion = 0.f;
        targetValue = target;
    }

    void setTarget(float target) {
        if (juce::approximatelyEqual(target, targetValue)) {
            return;
        }
        if (isInMotion) {
            auto newInitialValue = getValueWithoutTicking();
            auto newB = newInitialValue + getDerivative() / 3;
            initialValue = newInitialValue;
            b = newB;
            proportion = 0.f;
            isInMotion = true;
        } else {
            initialValue = targetValue;
            b = initialValue;
            isInMotion = true;
            proportion = 0.f;
        }
        targetValue = target;
    }

    void setRampLength(float numSteps) {
        rampLength = numSteps;
    }

    [[nodiscard]] float getDerivative() const {
        if (isInMotion) {
            return 3 * (1 - proportion) * (1 - proportion) * (b - initialValue) + 6 * (1 - proportion) * proportion * (targetValue - b);
        } else {
            return 0.f;
        }
    }

    [[nodiscard]] float getValueWithoutTicking() const {
        if (isInMotion) {
            return (1 - proportion) * (1 - proportion) * (1 - proportion) * initialValue
                   + 3 * (1 - proportion) * (1 - proportion) * proportion * b
                   + 3 * (1 - proportion) * proportion * proportion * targetValue
                   + proportion * proportion * proportion * targetValue;
        } else {
            return targetValue;
        }

    }

    float getNextValue() {
        auto v = getValueWithoutTicking();
        if (isInMotion) {
            proportion += 1 / rampLength;
            if (proportion >= 1) {
                isInMotion = false;
            }
        }
        return v;
    }
private:
    bool isInMotion{};
    float proportion{};
    float targetValue{};
    float initialValue{}, b{};
    float rampLength;
};

#endif //DELAYLINES_SMOOTHEDVALUE_H
