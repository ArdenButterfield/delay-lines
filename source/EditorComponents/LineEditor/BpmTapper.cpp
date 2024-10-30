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
        g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
        auto radius = getWidth() * std::pow(amountElapsed, 4.f) * 0.5f;
        g.fillEllipse(getWidth() / 2.f - radius, getHeight() / 2.f - radius, radius * 2, radius * 2);
    }
    auto backgroundAlpha = 1 - static_cast<float>(rawElapsed) / 300;
    auto backgroundColor = juce::Colours::white.withAlpha(std::min(std::max(0.f, backgroundAlpha), 1.f));
    g.setColour(backgroundColor);
    g.fillEllipse(0, 0, getWidth(), getHeight());
    g.setColour(juce::Colours::black);
    g.drawText("TAP", getLocalBounds(), juce::Justification::centred);
    auto tapBounds = getLocalBounds().reduced(2, 2);
    g.drawEllipse(tapBounds.toFloat(), rawElapsed < timeoutInterval ? 2.f : 1.f);
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
