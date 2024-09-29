#pragma once

#include "BinaryData.h"
#include "EditorComponents/PresetBrowser.h"

#include "EditorComponents/PlaygroundInterface.h"
#include "EditorComponents/ModularInterface.h"

#include "PlaygroundControllerOld.h"
#include "PluginProcessor.h"

#include "juce_audio_processors/juce_audio_processors.h"
#include "EditorComponents/DelayLinesLookAndFeel.h"
#include "Modulation/ModKnobs.h"
#include "Modulation/ModulationMappingEngine.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor, public juce::Button::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    DelayLinesLookAndFeel delayLinesLookAndFeel;

    juce::TextButton printXmlButton;
private:
    juce::TextButton clearLinesButton;

    PresetBrowser presetBrowser;
    ModulationMappingEngine modulationMappingEngine;

    PlaygroundInterface playgroundInterface;
    ModularInterface modularInterface;
    juce::TextButton switchInterface;

    void buttonClicked(juce::Button *) override;

    void buttonStateChanged(juce::Button*) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    juce::TextButton modulatorOverlayButton { "mod overlay" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)

    juce::Slider mixSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment mixAttachment;
    ModKnobs modKnobs;

};
