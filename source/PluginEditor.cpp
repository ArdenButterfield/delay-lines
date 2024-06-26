#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
      printXmlButton("Print XML"),
      presetBrowser(p.delayGraph),
      playgroundController(p.delayGraph),
      playgroundView(p.delayGraph),
      processorRef (p),
      modulatorOverlayButton("Modulator Overlay"),
      modKnobs(p.modulationEngine)
{
    playgroundView.addMouseListener(&playgroundController, false);

    modulatorOverlay = std::make_unique<ModulatorOverlay>(playgroundView);
    printXmlButton.addListener(this);
    modulatorOverlayButton.addListener(this);

    addAndMakeVisible(playgroundView);
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(presetBrowser);
    addAndMakeVisible(printXmlButton);
    addAndMakeVisible(modulatorOverlayButton);
//    addAndMakeVisible(modulatorOverlay.get()); TODO: bring back if neededp
    addAndMakeVisible(modKnobs);

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
    playgroundView.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() - 20, getHeight() - 150));
    modKnobs.setBounds(getLocalBounds().withTop(playgroundView.getBottom() + 10));
    printXmlButton.setBounds(10,10, 100, 30);
    presetBrowser.setBounds(150, 10, 200, 60);
    modulatorOverlayButton.setBounds(360, 10, 100, 30);
    mixSlider.setBounds(470, 10, 100, 30);
    modulatorOverlay->setBounds(playgroundView.getBounds());
}

void PluginEditor::buttonClicked (juce::Button* button)
{
    if (button == &printXmlButton) {
        processorRef.printXml();
    } else if (button == &modulatorOverlayButton) {
        modulatorOverlay->setVisible(!modulatorOverlay->isVisible());
    }
}
void PluginEditor::buttonStateChanged (juce::Button*)
{
}
