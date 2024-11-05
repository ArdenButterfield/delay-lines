#include "PluginEditor.h"
#include "catch2/benchmark/catch_benchmark_all.hpp"
#include "catch2/catch_test_macros.hpp"
#include "juce_audio_basics/juce_audio_basics.h"

TEST_CASE ("Boot performance")
{
    BENCHMARK_ADVANCED ("Processor constructor")
    (Catch::Benchmark::Chronometer meter)
    {
        auto gui = juce::ScopedJuceInitialiser_GUI {};
        std::vector<Catch::Benchmark::storage_for<PluginProcessor>> storage (size_t (meter.runs()));
        meter.measure ([&] (int i) { storage[(size_t) i].construct(); });
    };

    BENCHMARK_ADVANCED ("Processor destructor")
    (Catch::Benchmark::Chronometer meter)
    {
        auto gui = juce::ScopedJuceInitialiser_GUI {};
        std::vector<Catch::Benchmark::destructable_object<PluginProcessor>> storage (size_t (meter.runs()));
        for (auto& s : storage)
            s.construct();
        meter.measure ([&] (int i) { storage[(size_t) i].destruct(); });
    };

    BENCHMARK_ADVANCED ("Editor open and close")
    (Catch::Benchmark::Chronometer meter)
    {
        auto gui = juce::ScopedJuceInitialiser_GUI {};

        PluginProcessor plugin;

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            auto editor = plugin.createEditorIfNeeded();
            plugin.editorBeingDeleted (editor);
            delete editor;
            return plugin.getActiveEditor();
        });
    };
    BENCHMARK_ADVANCED("Process block")
    (Catch::Benchmark::Chronometer meter)
    {
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

        auto midiBuffer = juce::MidiBuffer();

        meter.measure ([&] (int /* i */) {
            for (int i = 0; i < 100; ++i) {
                pluginProcessor.processBlock(samples, midiBuffer);
            }
        });
    };
    BENCHMARK_ADVANCED("Process block with distortion")
    (Catch::Benchmark::Chronometer meter)
    {
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
        auto xml = juce::parseXML(R"(<?xml version="1.0" encoding="UTF-8"?> <plugin-state apvts="&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;&#13;&#10;&#13;&#10;&lt;DELAY-LINE-PARAMETERS&gt;&#13;&#10;  &lt;PARAM id=&quot;clear&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mix&quot; value=&quot;50.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod0&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod1&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod2&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod3&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod4&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod5&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod6&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod7&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod8&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod9&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;stretchtime&quot; value=&quot;0.09999999403953552&quot;/&gt;&#13;&#10;&lt;/DELAY-LINE-PARAMETERS&gt;&#13;&#10;"><graph><points><point id="1" x="0.0" y="0.0" type="1"/><point id="2" x="100.0" y="100.0" type="2"/></points><lines><line id="1" color="ff779966" start="1" end="2"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.6899999976158142" distortiontype="0.4000000059604645" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line></lines></graph><mod-mapping/></plugin-state>)");
        auto data = juce::MemoryBlock();
        pluginProcessor.copyXmlToBinary(*xml, data);
        pluginProcessor.setStateInformation(data.begin(), data.getSize());

        auto midiBuffer = juce::MidiBuffer();

        meter.measure ([&] (int /* i */) {
            for (int i = 0; i < 100; ++i) {
                pluginProcessor.processBlock(samples, midiBuffer);
            }
        });
    };
    BENCHMARK_ADVANCED("Process block with zero stretch")
    (Catch::Benchmark::Chronometer meter)
    {
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
        auto xml = juce::parseXML(R"(<?xml version="1.0" encoding="UTF-8"?> <plugin-state apvts="&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;&#13;&#10;&#13;&#10;&lt;DELAY-LINE-PARAMETERS&gt;&#13;&#10;  &lt;PARAM id=&quot;clear&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mix&quot; value=&quot;50.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod0&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod1&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod2&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod3&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod4&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod5&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod6&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod7&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod8&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod9&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;stretchtime&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;&lt;/DELAY-LINE-PARAMETERS&gt;&#13;&#10;"><graph><points><point id="1" x="0.0" y="0.0" type="1"/><point id="2" x="100.0" y="100.0" type="2"/></points><lines><line id="1" color="ff64a758" start="1" end="2"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line></lines></graph><mod-mapping/></plugin-state>)");
        auto data = juce::MemoryBlock();
        pluginProcessor.copyXmlToBinary(*xml, data);
        pluginProcessor.setStateInformation(data.begin(), data.getSize());

        auto midiBuffer = juce::MidiBuffer();

        meter.measure ([&] (int /* i */) {
            for (int i = 0; i < 100; ++i) {
                pluginProcessor.processBlock(samples, midiBuffer);
            }
        });
    };
    BENCHMARK_ADVANCED("Process block with modulation")
    (Catch::Benchmark::Chronometer meter)
    {
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
        auto xml = juce::parseXML(R"(<?xml version="1.0" encoding="UTF-8"?> <plugin-state apvts="&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;&#13;&#10;&#13;&#10;&lt;DELAY-LINE-PARAMETERS&gt;&#13;&#10;  &lt;PARAM id=&quot;clear&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mix&quot; value=&quot;50.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod0&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod1&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod2&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod3&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod4&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod5&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod6&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod7&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod8&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod9&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;stretchtime&quot; value=&quot;0.7299999594688416&quot;/&gt;&#13;&#10;&lt;/DELAY-LINE-PARAMETERS&gt;&#13;&#10;"><graph><points><point id="1" x="0.0" y="0.0" type="1"/><point id="2" x="100.0" y="100.0" type="2"/></points><lines><line id="1" color="ff64a758" start="1" end="2"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.7099999785423279" modrate="0.1685618758201599" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line></lines></graph><mod-mapping/></plugin-state>)");
        auto data = juce::MemoryBlock();
        pluginProcessor.copyXmlToBinary(*xml, data);
        pluginProcessor.setStateInformation(data.begin(), data.getSize());

        auto midiBuffer = juce::MidiBuffer();

        meter.measure ([&] (int /* i */) {
            for (int i = 0; i < 100; ++i) {
                pluginProcessor.processBlock(samples, midiBuffer);
            }
        });
    };
    BENCHMARK_ADVANCED("Process block with realistic graph")
    (Catch::Benchmark::Chronometer meter)
    {
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

        meter.measure ([&] (int /* i */) {
            for (int i = 0; i < 100; ++i) {
                pluginProcessor.processBlock(samples, midiBuffer);
            }
        });
    };

    BENCHMARK_ADVANCED("Process block with ten lines")
    (Catch::Benchmark::Chronometer meter)
    {
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
        auto xml = juce::parseXML(R"(<?xml version="1.0" encoding="UTF-8"?> <plugin-state apvts="&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;&#13;&#10;&#13;&#10;&lt;DELAY-LINE-PARAMETERS&gt;&#13;&#10;  &lt;PARAM id=&quot;clear&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mix&quot; value=&quot;50.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod0&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod1&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod2&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod3&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod4&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod5&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod6&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod7&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod8&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod9&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;stretchtime&quot; value=&quot;0.09999999403953552&quot;/&gt;&#13;&#10;&lt;/DELAY-LINE-PARAMETERS&gt;&#13;&#10;"><graph><points><point id="1" x="90.0" y="63.0" type="1"/><point id="2" x="190.0" y="163.0" type="2"/><point id="3" x="214.0" y="11.0" type="0"/><point id="4" x="356.0" y="62.0" type="0"/><point id="5" x="367.0" y="181.0" type="0"/><point id="6" x="258.0" y="345.0" type="0"/><point id="7" x="109.0" y="334.0" type="0"/><point id="8" x="40.0" y="239.0" type="0"/><point id="9" x="58.0" y="144.0" type="0"/><point id="10" x="176.0" y="95.0" type="0"/><point id="11" x="244.0" y="170.0" type="0"/></points><lines><line id="1" color="ffd8276b" start="1" end="3"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="2" color="fff5af0a" start="3" end="4"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="3" color="ffff0016" start="4" end="5"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="4" color="ff46b982" start="5" end="6"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="5" color="ffbdff00" start="6" end="7"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="6" color="ff022efd" start="7" end="8"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="7" color="ff27d885" start="8" end="9"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="8" color="ff3eef10" start="9" end="10"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="9" color="fffe01e9" start="10" end="11"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line><line id="10" color="ff01fe61" start="11" end="2"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line></lines></graph><mod-mapping/></plugin-state>)");
        auto data = juce::MemoryBlock();
        pluginProcessor.copyXmlToBinary(*xml, data);
        pluginProcessor.setStateInformation(data.begin(), data.getSize());

        auto midiBuffer = juce::MidiBuffer();

        meter.measure ([&] (int /* i */) {
            for (int i = 0; i < 100; ++i) {
                pluginProcessor.processBlock(samples, midiBuffer);
            }
        });
    };
}
