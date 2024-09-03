//
// Created by arden on 4/10/24.
//

#include "LengthEditor.h"


LengthEditor::LengthEditor (DelayGraph& _delayGraph, const int& _line) : delayGraph(_delayGraph), graphLine(_line)
{
    startTimerHz(60);
    unitSelector.addItemList({"ms", "Samples", "Hz", "Pitch", "Beat", "MIDI track"}, 1); // offset must start at 1: 0 is reserved for undefined
    unitSelector.addListener(this);

    samplesSlider.setNormalisableRange({0,200});
    samplesSlider.setSkewFactorFromMidPoint(30);

    millisecondsSlider.setNormalisableRange({0,5000});
    millisecondsSlider.setSkewFactorFromMidPoint(500);

    hertzSlider.setNormalisableRange({1, 20000});
    hertzSlider.setSkewFactorFromMidPoint(100);

    pitchSlider.setNormalisableRange({0, 127, 1});

    beatNumerator.setNormalisableRange({1, 32, 1});
    beatDenominator.setNormalisableRange({1, 32, 1});

    addAndMakeVisible(unitSelector);

    for (auto slider : {
             &samplesSlider,
             &millisecondsSlider,
             &hertzSlider,
             &beatNumerator,
             &beatDenominator}) {
        addAndMakeVisible(slider);
        slider->addListener(this);
    }

    addAndMakeVisible(pitchSlider);
    pitchSlider.addListener(this);

    addAndMakeVisible(bpmTapper);
    bpmTapper.addListener(this);
    bpmTapper.setTimeoutInterval(static_cast<juce::int64>(millisecondsSlider.getRange().getEnd()));

    updateSliders();
    updateUnitSelector();
}

void LengthEditor::resized()
{
    unitSelector.setBounds(getLocalBounds().withWidth(80).withRightX(getRight()));
    auto sliderZone = getLocalBounds().withRight(unitSelector.getX());

    samplesSlider.setBounds(sliderZone);
    bpmTapper.setBounds(sliderZone.withWidth(40));
    millisecondsSlider.setBounds(sliderZone.withTrimmedLeft(bpmTapper.getWidth()));

    hertzSlider.setBounds(sliderZone);
    pitchSlider.setBounds(sliderZone);

    beatNumerator.setBounds(sliderZone.withWidth(sliderZone.getWidth() / 2));
    beatDenominator.setBounds(beatNumerator.getBounds().withX(beatNumerator.getRight()));

}

void LengthEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::mediumaquamarine);
}

void LengthEditor::timerCallback()
{
    updateSliders();
}

void LengthEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &unitSelector) {
        updateUnitSelector();
    }
}
void LengthEditor::sliderValueChanged (juce::Slider* slider)
{
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }
    if (slider == &millisecondsSlider) {
        line->parameters.length.setMillisecondsLength(static_cast<float>(millisecondsSlider.getValue()));
    } else if (slider == &samplesSlider) {
        line->parameters.length.setSamplesLength(static_cast<float>(samplesSlider.getValue()));
    } else if (slider == &hertzSlider) {
        line->parameters.length.setHertz(static_cast<float>(hertzSlider.getValue()));
    } else if (slider == &pitchSlider) {
        line->parameters.length.setMidiNote(static_cast<int>(pitchSlider.getValue()));
    } else if (slider == &beatNumerator || slider == &beatDenominator) {
        auto n = static_cast<int>(std::round(beatNumerator.getValue()));
        auto d = static_cast<int>(std::round(beatDenominator.getValue()));
        line->parameters.length.setBeat(n, d);
    }
}

void LengthEditor::updateSliders()
{
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }


    switch (line->parameters.length.getMode()) {
        case DelayLength::ms:
            unitSelector.setSelectedId(1);
            break;
        case DelayLength::samps:
            unitSelector.setSelectedId(2);
            break;
        case DelayLength::hz:
            unitSelector.setSelectedId(3);
            break;
        case DelayLength::note:
            unitSelector.setSelectedId(4);
            break;
        case DelayLength::beat:
            unitSelector.setSelectedId(5);
            break;
        case DelayLength::midiTrack:
            unitSelector.setSelectedId(6);
        default: break;
    }

    samplesSlider.setValue(line->parameters.length.getSamplesLength(), juce::dontSendNotification);
    millisecondsSlider.setValue(line->parameters.length.getMillisecondsLength(), juce::dontSendNotification);
    hertzSlider.setValue(line->parameters.length.getHertz(), juce::dontSendNotification);
    pitchSlider.setValue(line->parameters.length.getMidiNote(), juce::dontSendNotification);
    beatNumerator.setValue(line->parameters.length.getNumerator(), juce::dontSendNotification);
    beatDenominator.setValue(line->parameters.length.getDenominator(), juce::dontSendNotification);
}
void LengthEditor::updateUnitSelector()
{
    auto line = delayGraph.getLine(graphLine);
    if (!line) {
        return;
    }

    millisecondsSlider.setVisible(false);
    samplesSlider.setVisible(false);
    hertzSlider.setVisible(false);
    pitchSlider.setVisible(false);
    beatNumerator.setVisible(false);
    beatDenominator.setVisible(false);

    bpmTapper.setVisible(false);

    switch (unitSelector.getSelectedId())
    {
        case 1:
            millisecondsSlider.setVisible(true);
            bpmTapper.setVisible(true);
            line->parameters.length.setMode(DelayLength::ms);
            break;
        case 2:
            samplesSlider.setVisible(true);
            line->parameters.length.setMode(DelayLength::samps);
            break;
        case 3:
            hertzSlider.setVisible(true);
            line->parameters.length.setMode(DelayLength::hz);
            break;
        case 4:
            pitchSlider.setVisible(true);
            line->parameters.length.setMode(DelayLength::note);
            break;
        case 5:
            beatNumerator.setVisible(true);
            beatDenominator.setVisible(true);
            line->parameters.length.setMode(DelayLength::beat);
            break;
        case 6:
            line->parameters.length.setMode(DelayLength::midiTrack);
        default: break;
    }
}
void LengthEditor::tempoSet (juce::int64 interval)
{
    std::cout << interval << "\n";
    millisecondsSlider.setValue(static_cast<double>(interval), juce::sendNotification);
}
