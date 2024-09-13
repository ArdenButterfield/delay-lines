//
// Created by arden on 9/2/24.
//

#include "BpmTapper.h"
BpmTapper::BpmTapper()
{
    startTimerHz(60);
    lastTap = 0;
}

BpmTapper::~BpmTapper()
{
}

void BpmTapper::paint (juce::Graphics& g)
{
    auto rawElapsed = juce::Time::currentTimeMillis() - lastTap;
    if (rawElapsed < timeoutInterval) {
        auto amountElapsed = 1 - static_cast<float>(rawElapsed) / timeoutInterval;
        g.setColour(juce::Colours::blue.withAlpha(0.3f));
        g.fillRect(getLocalBounds().withSizeKeepingCentre(getWidth() * amountElapsed, getHeight() * amountElapsed));
    }
    auto backgroundAlpha = 1 - static_cast<float>(rawElapsed) / 300;
    auto backgroundColor = juce::Colours::white.withAlpha(std::max(0.f, backgroundAlpha));
    g.fillAll(backgroundColor);
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void BpmTapper::resized()
{
    Component::resized();
}

void BpmTapper::mouseDown (const juce::MouseEvent& event)
{
    auto t = event.eventTime.toMilliseconds();
    if (t - lastTap < timeoutInterval) {
        millisecondIntervals.push_back(t - lastTap);
        lastTap = t;
        if (!millisecondIntervals.empty()) {
            juce::int64 averageLength = 0;
            for (auto interval : millisecondIntervals) {
                averageLength += interval;
            }
            averageLength /= millisecondIntervals.size();
            for (auto listener : listeners) {
                listener->tempoSet(averageLength);
            }
        }
    } else {
        millisecondIntervals.resize(0);
        lastTap = t;
    }
}

void BpmTapper::timerCallback() {
    if (juce::Time::currentTimeMillis() - lastTap > timeoutInterval) {
        lastTap = 0;
    }
    repaint();
}

void BpmTapper::addListener (BpmTapper::Listener* listener)
{
    listeners.insert(listener);
}

void BpmTapper::removeListener (BpmTapper::Listener* listener)
{
    if (listeners.contains(listener)) {
        listeners.erase(listener);
    }
}

void BpmTapper::setTimeoutInterval (juce::int64 interval)
{
    timeoutInterval = interval;
}
