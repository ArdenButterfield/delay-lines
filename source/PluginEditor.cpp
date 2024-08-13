#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
      printXmlButton("Print XML"),
      presetBrowser(p.delayGraph),
      playgroundController(p.delayGraph),
      playgroundViewOld(p.delayGraph),
      processorRef (p),
      modulatorOverlayButton("Modulator Overlay"),
      modKnobs(p.modulationEngine),
      playgroundComponent(p.delayGraph)
{
    playgroundViewOld.addMouseListener(&playgroundController, false);

    printXmlButton.addListener(this);
    modulatorOverlayButton.addListener(this);

    addAndMakeVisible(playgroundViewOld);
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(presetBrowser);
    addAndMakeVisible(printXmlButton);
    addAndMakeVisible(modulatorOverlayButton);
//    addAndMakeVisible(modulatorOverlay.get()); TODO: bring back if neededp
    addAndMakeVisible(modKnobs);
    addAndMakeVisible(playgroundComponent);
//    setResizable(true, true);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
}

PluginEditor::~PluginEditor() = default;

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    playgroundViewOld.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() - 20, getHeight() - 150));

    playgroundViewOld.setBounds(playgroundViewOld.getBounds().withWidth(playgroundViewOld.getWidth() / 2));
    playgroundComponent.setBounds(playgroundViewOld.getBounds().withX(playgroundViewOld.getRight()));

    modKnobs.setBounds(getLocalBounds().withTop(playgroundViewOld.getBottom() + 10));
    printXmlButton.setBounds(10,10, 100, 30);
    presetBrowser.setBounds(150, 10, 200, 60);
    modulatorOverlayButton.setBounds(360, 10, 100, 30);
    mixSlider.setBounds(470, 10, 100, 30);
}

void PluginEditor::buttonClicked (juce::Button* button)
{
    if (button == &printXmlButton) {
        processorRef.printXml();
    } else if (button == &modulatorOverlayButton) {
//        modulatorOverlay->setVisible(!modulatorOverlay->isVisible());
    }
}
void PluginEditor::buttonStateChanged (juce::Button*)
{
}
