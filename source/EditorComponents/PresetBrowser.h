//
// Created by arden on 4/16/24.
//

#ifndef DELAYLINES_PRESETBROWSER_H
#define DELAYLINES_PRESETBROWSER_H

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_core/juce_core.h"

#include "../DelayGraph.h"

class PresetBrowser : public juce::Component, public juce::ComboBox::Listener
{
public:
    PresetBrowser(DelayGraph& dg);
    ~PresetBrowser();
    void paint(juce::Graphics &g) override;
    void resized() override;
    juce::ComboBox presetMenu;
private:
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    DelayGraph& delayGraph;
    std::unique_ptr<juce::XmlElement> presets;
};

#endif //DELAYLINES_PRESETBROWSER_H
