//
// Created by arden on 9/2/24.
//

#ifndef DELAYLINES_BPMTAPPER_H
#define DELAYLINES_BPMTAPPER_H

#include "juce_gui_basics/juce_gui_basics.h"

class BpmTapper : public juce::Component, public juce::Timer
{
public:
    class Listener {
    public:
        virtual void tempoSet(juce::int64 interval) = 0;
    };
    BpmTapper();
    ~BpmTapper() override;

    void addListener(Listener* listener);

    void removeListener(Listener* listener);
    void setTimeoutInterval(juce::int64 interval);
private:
    void paint(juce::Graphics &g) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown(const juce::MouseEvent &event) override;

    std::vector<juce::int64> millisecondIntervals;
    juce::int64 lastTap;
    juce::int64 timeoutInterval = 3 * 1000;
    std::set<Listener*> listeners;
};

#endif //DELAYLINES_BPMTAPPER_H
