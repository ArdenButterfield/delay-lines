//
// Created by arden on 10/29/24.
//

#include "TitleGraphic.h"
#include "DelayLinesLookAndFeel.h"

TitleGraphic::TitleGraphic() : font(juce::Font(DelayLinesLookAndFeel::getLogoFont()).withHeight(25))
{
    startTimerHz(60);
    mouseInteractFilter.setAttackTime(3000);
    mouseInteractFilter.setReleaseTime(800);
    mouseInteractFilter.prepare({60, 512, 1});
    mouseInteractFilter.reset(0);
    stretch = 0;
    manualLink.setButtonText("Wildergarden - 2024");
    manualLink.setURL(juce::URL("https://github.com/ArdenButterfield/delay-lines"));
    manualLink.setFont(juce::Font(DelayLinesLookAndFeel::getMonoFont()).withHeight(12),
        false, juce::Justification::topLeft);
    manualLink.setColour(juce::HyperlinkButton::ColourIds::textColourId, juce::Colours::blue);
    addAndMakeVisible(manualLink);
}
TitleGraphic::~TitleGraphic()
{
}
void TitleGraphic::paint (juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(juce::Colours::white,
        {0,static_cast<float>(getHeight())},
        juce::Colours::black,
        {0,0},
        false));
    g.fillAll();

    const auto step = 20.f + stretch * 10;

    g.setFont(font);

    auto textBounds = getLocalBounds().withBottom(manualLink.getY() + 5).reduced(10);
    for (auto i = 1; i < 5; ++i) {
        g.setColour(juce::Colours::white.withAlpha(0.2f * (5 - i) * 0.2f));
        g.drawText("ECHO", textBounds.translated(step * i, step * i * 0.1f * (1 + 4 * stretch)), juce::Justification::topLeft);
    }
    g.setColour(juce::Colours::white);
    g.drawText("ECHO", textBounds, juce::Justification::topLeft);
    for (auto i = 1; i < 5; ++i) {
        g.setColour(juce::Colours::black.withAlpha(0.2f * (5 - i) * 0.2f));
        g.drawText("LOCATION", textBounds.translated(-step * i, -step * i * 0.1f * (1 + 4 * stretch)), juce::Justification::bottomRight);
    }
    g.setColour(juce::Colours::black);

    g.drawText("LOCATION", textBounds, juce::Justification::bottomRight);
}
void TitleGraphic::timerCallback()
{
    stretch = mouseInteractFilter.processSample(0, isMouseOver(true) ? 1 : 0);
    repaint();
}
void TitleGraphic::mouseEnter (const juce::MouseEvent& event)
{
    Component::mouseEnter (event);
}
void TitleGraphic::mouseExit (const juce::MouseEvent& event)
{
    Component::mouseExit (event);
}
void TitleGraphic::resized()
{
    manualLink.setBounds(getLocalBounds().withTop(getBottom() - 20));
}
