//
// Created by arden on 8/15/24.
//

#ifndef DELAYLINES_COMPACTSLIDER_H
#define DELAYLINES_COMPACTSLIDER_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "BinaryData.h"

class CompactSlider : public juce::Slider, public juce::Label::Listener
{
public:
    CompactSlider();

    void setSuffix(const juce::String& _suffix);

    void paint(juce::Graphics& g) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void resized() override;

    virtual juce::String getDisplayText(double rawValue);

    void labelTextChanged(juce::Label *labelThatHasChanged) override {}
    void editorShown(juce::Label * label, juce::TextEditor & textEditor) override;
    void editorHidden(juce::Label * label, juce::TextEditor & textEditor) override;
private:
    juce::String suffix;
    juce::Label valueLabel;
};

#endif //DELAYLINES_COMPACTSLIDER_H
