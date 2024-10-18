//
// Created by arden on 10/17/24.
//

#ifndef DELAYLINES_LENGTHFADER_H
#define DELAYLINES_LENGTHFADER_H

#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"

class LengthFader
{
public:
    LengthFader() {
        queuedPosition = -1;
    }
    ~LengthFader() = default;

    void setFadeTime(float samples) {
        fadeTime = samples;
    }

    void resetToTarget(float pos) {
        currentPosition = pos;
        queuedPosition = -1;
        inFade = false;
    }

    void setTarget(float newPos) {
        if (juce::approximatelyEqual(newPos, currentPosition)) {
            return;
        }
        if (inFade) {
            queuedPosition = newPos;
        } else {
            inFade = true;
            targetPosition = newPos;
            fadeCounter = 0.f;
        }
    }

    void getNextValue(float& firstPostion, float& firstLevel, float& secondPosition, float& secondLevel) {
        if (inFade) {
            firstPostion = currentPosition;
            secondPosition = targetPosition;
            // Equal power fade
            firstLevel = juce::dsp::FastMathApproximations::cos(juce::MathConstants<float>::pi * fadeCounter * 0.5f);
            secondLevel = juce::dsp::FastMathApproximations::cos(juce::MathConstants<float>::pi * (1 - fadeCounter) * 0.5f);
        } else {
            firstPostion = currentPosition;
            firstLevel = 1;
            secondPosition = currentPosition;
            secondLevel = 0;
        }
        tick();
    }

    void tick() {
        if (inFade) {
            fadeCounter += 1 / fadeTime;

            if (fadeCounter >= 1) {
                fadeCounter = 0;
                currentPosition = targetPosition;
                if (queuedPosition >= 0) {
                    targetPosition = queuedPosition;
                    queuedPosition = -1;
                    inFade = true;
                }
                else {
                    inFade = false;
                }
            }
        }
    }

private:
    float currentPosition;
    float fadeTime;
    float fadeCounter;
    float targetPosition;
    float queuedPosition;
    bool inFade;
};

#endif //DELAYLINES_LENGTHFADER_H
