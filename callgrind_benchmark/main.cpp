//
// Created by arden on 11/4/24.
//

#include "PluginProcessor.h"
#include <juce_audio_basics/juce_audio_basics.h>

int main() {
    PluginProcessor pluginProcessor;
    const int blockSize = 512;
    pluginProcessor.prepareToPlay(44100, blockSize);
    auto samples = juce::AudioBuffer<float>(pluginProcessor.getTotalNumInputChannels(), blockSize);
    auto random = juce::Random();
    for (auto channel = 0; channel < samples.getNumChannels(); ++channel) {
        for (auto sample = 0; sample < samples.getNumSamples(); ++sample) {
            samples.setSample(channel, sample, random.nextFloat() - 0.5f);
        }
    }
    auto xml = juce::parseXML(R"(<?xml version="1.0" encoding="UTF-8"?> <plugin-state apvts="&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;&#13;&#10;&#13;&#10;&lt;DELAY-LINE-PARAMETERS&gt;&#13;&#10;  &lt;PARAM id=&quot;clear&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mix&quot; value=&quot;50.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod0&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod1&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod2&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod3&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod4&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod5&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod6&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod7&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod8&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod9&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;stretchtime&quot; value=&quot;0.7299999594688416&quot;/&gt;&#13;&#10;&lt;/DELAY-LINE-PARAMETERS&gt;&#13;&#10;"><graph><points><point id="1" x="73.0" y="57.0" type="1"/><point id="2" x="409.0" y="234.0" type="2"/><point id="3" x="376.0" y="63.0" type="0"/><point id="4" x="195.0" y="281.0" type="0"/></points><lines><line id="1" color="ff64a758" start="1" end="2"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.7099999785423279" modrate="0.1685618758201599" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="4" samples="0.0" milliseconds="1342.6884765625" hertz="10.0" pitch="100.0" numerator="3.0" denominator="8.0" midiTrackPitchShift="0.0"/></parameters></line><line id="2" color="ff5758a8" start="1" end="3"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.7239999771118164" pan="0.5"><delayLength mode="3" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="59.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="3" color="ffef0cf3" start="3" end="2"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="4" samples="0.0" milliseconds="458.993896484375" hertz="10.0" pitch="100.0" numerator="0.8480451107025146" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="4" color="ffc94436" start="3" end="4"><parameters mutebypass="1.0" lengthenvelopefollow="0.5" moddepth="0.6800000071525574" modrate="0.03244147077202797" distortion="1.0" distortiontype="0.4000000059604645" hicut="1.0" locut="0.0" gain="1.0" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="5" color="ffd4ff00" start="4" end="2"><parameters mutebypass="0.6666666865348816" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line></lines></graph><mod-mapping/></plugin-state>)");
    auto data = juce::MemoryBlock();
    pluginProcessor.copyXmlToBinary(*xml, data);
    pluginProcessor.setStateInformation(data.begin(), data.getSize());
    auto midiBuffer = juce::MidiBuffer();

    for (int i = 0; i < 100000; ++i) {
        pluginProcessor.processBlock(samples, midiBuffer);
    }
    float i = 0;
    for (auto channel = 0; channel < samples.getNumChannels(); ++channel) {
        for (auto sample = 0; sample < samples.getNumSamples(); ++sample) {
            i += samples.getSample(channel, sample);
        }
    }
    std::cout << i << "\n";

    return 0;
}