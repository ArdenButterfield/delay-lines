#pragma once

#include "BinaryData.h"
#include "EditorComponents/PlaygroundView.h"
#include "EditorComponents/PresetBrowser.h"

#include "PlaygroundController.h"
#include "PluginProcessor.h"

#include "juce_audio_processors/juce_audio_processors.h"

#include "Modulation/ModKnobs.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor, public juce::Button::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::TextButton printXmlButton;
private:
    PresetBrowser presetBrowser;
    PlaygroundController playgroundController;
    PlaygroundView playgroundView;
    void buttonClicked(juce::Button *) override;

    void buttonStateChanged(juce::Button*) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    juce::TextButton modulatorOverlayButton { "mod overlay" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)

    juce::Slider mixSlider;

    ModKnobs modKnobs;
};
