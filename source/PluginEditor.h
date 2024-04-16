#pragma once

#include "BinaryData.h"
#include "PlaygroundController.h"
#include "PlaygroundView.h"
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
    void buttonClicked(juce::Button *) override;
    void buttonStateChanged(juce::Button*) override;

    PlaygroundController playgroundController;
    PlaygroundView playgroundView;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)

    juce::Slider mixSlider;
    const juce::AudioProcessorValueTreeState::SliderAttachment mixAttachment;
};
