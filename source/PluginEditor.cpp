#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
      playgroundController(p.delayGraph),
      playgroundView(p.delayGraph),
      processorRef (p),
      printXmlButton("Print XML"),
      mixAttachment(p.getValueTreeState(), "mix", mixSlider)
{
    playgroundView.addMouseListener(&playgroundController, false);

    printXmlButton.addListener(this);

    addAndMakeVisible(playgroundView);
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(printXmlButton);

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
    playgroundView.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() - 20, getHeight() - 150));
    mixSlider.setBounds(getLocalBounds().withTop(playgroundView.getBottom() + 10));
    printXmlButton.setBounds(10,10, 100, 30);
}

void PluginEditor::buttonClicked (juce::Button* button)
{
    if (button == &printXmlButton) {
        processorRef.printXml();
    }
}
void PluginEditor::buttonStateChanged (juce::Button*)
{
}
