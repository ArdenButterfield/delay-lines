#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),  playgroundController(p.delayGraph), playgroundView(p.delayGraph), processorRef (p)
{
    playgroundView.addMouseListener(&playgroundController, false);

    juce::ignoreUnused (processorRef);

    addAndMakeVisible(playgroundView);

    setResizable(true, true);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
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
    playgroundView.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() - 20, getHeight() - 20));
}

