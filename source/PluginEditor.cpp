#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),  playgroundController(p.delayGraph), playgroundArea(p.delayGraph), processorRef (p)
{
    playgroundArea.addMouseListener(&playgroundController, false);

    juce::ignoreUnused (processorRef);

    addAndMakeVisible(playgroundArea);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    playgroundArea.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() - 20, getHeight() - 20));
}
