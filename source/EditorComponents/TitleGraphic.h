//
// Created by arden on 10/29/24.
//

#ifndef DELAYLINES_TITLEGRAPHIC_H
#define DELAYLINES_TITLEGRAPHIC_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_dsp/juce_dsp.h"

class TitleGraphic : public juce::Component, public juce::Timer
{
public:
    TitleGraphic();
    ~TitleGraphic();

    void paint(juce::Graphics &g) override;
    void timerCallback() override;
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
private:
    juce::dsp::BallisticsFilter<float> mouseInteractFilter;
    float stretch;
    juce::HyperlinkButton manualLink;
    void resized() override;

    juce::Font font;
};

#endif //DELAYLINES_TITLEGRAPHIC_H
