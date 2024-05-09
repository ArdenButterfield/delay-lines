#pragma once

#include "BinaryData.h"
#include "EditorComponents/PlaygroundView.h"
#include "EditorComponents/PresetBrowser.h"
#include "EditorComponents/ModulatorOverlay.h"

#include "PlaygroundController.h"
#include "PluginProcessor.h"

#include "melatonin_inspector/melatonin_inspector.h"
#include "juce_audio_processors/juce_audio_processors.h"

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
    std::unique_ptr<ModulatorOverlay> modulatorOverlay;
    PresetBrowser presetBrowser;
    PlaygroundController playgroundController;
    PlaygroundView playgroundView;
    void buttonClicked(juce::Button *) override;

    void buttonStateChanged(juce::Button*) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    juce::TextButton modulatorOverlayButton { "mod overlay" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)

    juce::Slider mixSlider;
};
