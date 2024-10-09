//
// Created by arden on 10/9/24.
//

#ifndef DELAYLINES_NEWLINEBUTTON_H
#define DELAYLINES_NEWLINEBUTTON_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "../../DelayGraph.h"

class NewLineButton : public juce::Component
{
public:
    explicit NewLineButton(DelayGraph& dg);
    ~NewLineButton() override;
private:
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

    DelayGraph& delayGraph;

    bool mouseIsOver;
};

#endif //DELAYLINES_NEWLINEBUTTON_H
