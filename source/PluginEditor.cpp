#include "PluginEditor.h"
#include "parameters.h"
PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
      copyXmlButton("Copy state"),
      pasteXmlButton("Paste state"),
      clearLinesButton("Clear lines"),
      presetBrowser(p.delayGraph),
      playgroundInterface(modulationMappingEngine, p.delayGraph),
      modularInterface(modulationMappingEngine, p.delayGraph),
      processorRef (p),
      modulatorOverlayButton("Modulator Overlay"),
      mixAttachment(p.getValueTreeState(), MIX_PARAM_ID, mixSlider),
      stretchTimeAttachment(p.getValueTreeState(), STRETCH_TIME_ID, stretchTimeSlider),
      midiInputSelector(processorRef.midiReceiver)
{
    mixSlider.setNumDecimalPlacesToDisplay(1);
    mixSlider.setSuffix("%");
    stretchTimeSlider.setSuffix("\"");
    modulationMappingEngine.setModulationEngine(&processorRef.modulationEngine);

    modKnobs.setMappingEngine(&modulationMappingEngine);

    setLookAndFeel(&delayLinesLookAndFeel);

    copyXmlButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    pasteXmlButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    clearLinesButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    copyXmlButton.addListener(this);
    pasteXmlButton.addListener(this);
    modulatorOverlayButton.addListener(this);
    clearLinesButton.addListener(this);
    switchInterface.addListener(this);

    addAndMakeVisible(mixSlider);
    addAndMakeVisible(stretchTimeSlider);
    addAndMakeVisible(presetBrowser);
    addAndMakeVisible(copyXmlButton);
    addAndMakeVisible(pasteXmlButton);
    addAndMakeVisible(switchInterface);
    addAndMakeVisible(clearLinesButton);
    addAndMakeVisible(modulatorOverlayButton);
    addAndMakeVisible(modKnobs);
    addAndMakeVisible(playgroundInterface);
    addChildComponent(modularInterface);
    addAndMakeVisible(midiInputSelector);

    switchInterface.setToggleState(false, juce::sendNotification);


    addAndMakeVisible(titleGraphic);

    mixLabel.setText("Mix", juce::dontSendNotification);
    stretchLabel.setText("Stretch speed", juce::dontSendNotification);
    mixLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    stretchLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(mixLabel);
    addAndMakeVisible(stretchLabel);

    setResizable(true, false);
    setResizeLimits(500,500,10000,10000);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
}

PluginEditor::~PluginEditor() {
    setLookAndFeel(nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::lightgrey.withMultipliedBrightness(0.8f));
    g.setColour(juce::Colours::lightgrey);
    g.fillRect(mixArea);
    g.fillRect(stretchArea);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    topStrip = getLocalBounds().withHeight(100).reduced(10);
    titleGraphic.setBounds(topStrip.withWidth(150));
    switchInterface.setBounds(topStrip.withX(titleGraphic.getRight() + 10).withWidth(topStrip.getHeight() / 2));
    mixArea = topStrip.withWidth(60).withRightX(topStrip.getRight());
    mixSlider.setBounds(mixArea.withHeight(stretchArea.getHeight() / 2));
    mixLabel.setBounds(mixArea.withTrimmedTop(mixSlider.getBottom()));
    stretchArea = mixArea.withRightX(mixSlider.getX() - 10);
    stretchTimeSlider.setBounds(stretchArea.withHeight(stretchArea.getHeight() / 2));
    stretchLabel.setBounds(stretchArea.withTrimmedTop(stretchTimeSlider.getBottom()));

    auto presetBrowserArea = topStrip.withX(switchInterface.getRight() + 10)
                                 .withHeight(topStrip.getHeight() / 2)
                                 .withRight(stretchTimeSlider.getX() - 10);
    midiInputSelector.setBounds(presetBrowserArea.withWidth(presetBrowserArea.getWidth() / 2 - 2));
    presetBrowser.setBounds(presetBrowserArea.withLeft(midiInputSelector.getRight() + 5));
    auto belowPresetBrowser = presetBrowserArea.withBottomY(topStrip.getBottom() + 4);
    copyXmlButton.setBounds(belowPresetBrowser.withWidth(belowPresetBrowser.getWidth() / 4));
    pasteXmlButton.setBounds(copyXmlButton.getBounds().withX(copyXmlButton.getRight()));
    clearLinesButton.setBounds(belowPresetBrowser.withLeft(pasteXmlButton.getRight()));

    playgroundInterface.setBounds(getLocalBounds()
                                       .withTrimmedTop(topStrip.getBottom() + 10)
                                       .withTrimmedBottom(80)
                                       .withTrimmedRight(10)
                                       .withTrimmedLeft(10));
    modularInterface.setBounds(playgroundInterface.getBounds());

    modKnobs.setBounds(getLocalBounds().withTop(playgroundInterface.getBottom()).reduced(10));
}

void PluginEditor::buttonClicked (juce::Button* button)
{
    if (button == &copyXmlButton) {
        auto data = juce::MemoryBlock();
        processorRef.getStateInformation(data);
        auto xml = processorRef.getXmlFromBinary(data.begin(), data.getSize());
        juce::SystemClipboard::copyTextToClipboard(xml->toString(juce::XmlElement::TextFormat().singleLine()));
    } else if (button == &pasteXmlButton) {
        auto xml = juce::parseXML(juce::SystemClipboard::getTextFromClipboard());
        if (xml) {
            auto data = juce::MemoryBlock();
            processorRef.copyXmlToBinary(*xml, data);
            processorRef.setStateInformation(data.begin(), data.getSize());
        }
    } else if (button == &switchInterface) {
        playgroundInterface.setVisible(!switchInterface.getToggleState());
        modularInterface.setVisible(switchInterface.getToggleState());
    }
}

void PluginEditor::buttonStateChanged (juce::Button* button)
{
    if (button == &clearLinesButton) {
        processorRef.parameters.getParameter(CLEAR_PARAM_ID)->setValueNotifyingHost(button->isDown() ? 1.f : 0.f);
    }
}
