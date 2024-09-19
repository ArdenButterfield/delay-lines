#include "PluginEditor.h"
#include "parameters.h"
PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
      printXmlButton("Print XML"),
      clearLinesButton("Clear lines"),
      presetBrowser(p.delayGraph),
      playgroundComponent(p.delayGraph),
      processorRef (p),
      modulatorOverlayButton("Modulator Overlay"),
      mixAttachment(p.getValueTreeState(), MIX_PARAM_ID, mixSlider),
      modKnobs(p.modulationEngine)
{
    setLookAndFeel(&delayLinesLookAndFeel);

    printXmlButton.addListener(this);
    modulatorOverlayButton.addListener(this);
    clearLinesButton.addListener(this);


    addAndMakeVisible(mixSlider);
    addAndMakeVisible(presetBrowser);
    addAndMakeVisible(printXmlButton);
    addAndMakeVisible(clearLinesButton);
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
    playgroundComponent.setBounds(getLocalBounds().withSizeKeepingCentre(getWidth() - 20, getHeight() - 150));


    modKnobs.setBounds(getLocalBounds().withTop(playgroundComponent.getBottom() + 10));
    printXmlButton.setBounds(10,10, 100, 30);
    presetBrowser.setBounds(150, 10, 200, 60);
//    modulatorOverlayButton.setBounds(360, 10, 100, 30);
    clearLinesButton.setBounds(360, 10, 100, 30);
    mixSlider.setBounds(470, 10, 100, 30);
}

void PluginEditor::buttonClicked (juce::Button* button)
{
    if (button == &printXmlButton) {
        processorRef.printXml();
    } else if (button == &modulatorOverlayButton) {
    }
}

void PluginEditor::buttonStateChanged (juce::Button* button)
{
    if (button == &clearLinesButton) {
        processorRef.parameters.getParameter(CLEAR_PARAM_ID)->setValueNotifyingHost(button->isDown() ? 1.f : 0.f);
    }
}
