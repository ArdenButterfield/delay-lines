#include "helpers/test_helpers.h"
#include <PluginProcessor.h>
#include <PluginEditor.h>
#include <DelayGraph.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE ("one is equal to one", "[dummy]")
{
    REQUIRE (1 == 1);
}

TEST_CASE ("Plugin instance", "[instance]")
{
    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    PluginProcessor testPlugin;


    SECTION ("name")
    {
        CHECK_THAT (testPlugin.getName().toStdString(),
            Catch::Matchers::Equals ("EchoLocation"));
    }
}

TEST_CASE("Basic xml export", "[xmlbasic]")
{
    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};
    DelayGraph delayGraph;

    auto xml = juce::XmlElement("plugin-state");
    delayGraph.exportToXml(&xml);
}

TEST_CASE("Import to point", "[xmlpointimport]")
{
    auto gui = juce::ScopedJuceInitialiser_GUI {};
    auto p = GraphPoint({1.f, 1.f}, GraphPoint::inner, 42);

    auto xmlText =                    "    <points>\n"
                   "      <point id=\"42\" x=\"100.0\" y=\"100.0\" type=\"2\"/>\n"
                   "      <point id=\"2\" x=\"400.0\" y=\"100.0\" type=\"0\"/>\n"
                   "      <point id=\"3\" x=\"50.0\" y=\"0.0\" type=\"1\"/>\n"
                   "    </points>\n";

    auto xmlElement = juce::parseXML(xmlText);
    REQUIRE(p.importFromXml(xmlElement.get()));
}

TEST_CASE("Import to line", "[importtoline]")
{
    auto gui = juce::ScopedJuceInitialiser_GUI {};
    auto delayGraph = DelayGraph();
    auto line = delayGraph.getLine(1);
    auto element = juce::XmlElement("lines");
    line->exportToXml(&element);
    element.getChildElement(0)->getChildByName("parameters")->setAttribute("gain", 0.25);

    line->importFromXml(&delayGraph, &element);

    auto element2 = juce::XmlElement("lines");
    line->exportToXml(&element2);
    REQUIRE(element.isEquivalentTo(&element2, false));
//    line->importFromXml(&delayGraph, xml.get());
}

TEST_CASE("Import to point constructor", "[xmlpointimportconstructor]")
{
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    auto xmlText =                    "    <points>\n"
                   "      <point id=\"42\" x=\"100.0\" y=\"100.0\" type=\"2\"/>\n"
                   "      <point id=\"2\" x=\"400.0\" y=\"100.0\" type=\"0\"/>\n"
                   "      <point id=\"3\" x=\"50.0\" y=\"0.0\" type=\"1\"/>\n"
                   "    </points>\n";

    auto xmlElement = juce::parseXML(xmlText);
    auto p = GraphPoint(xmlElement->getChildElement(0));
    REQUIRE(p.identifier == 42);
}

TEST_CASE("Export XML", "[exportxml]")
{
    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    DelayGraph delayGraph;

    auto xml = juce::XmlElement("plugin-state");
    delayGraph.exportToXml(&xml);


    auto point  = xml.getChildByName("graph")->getChildByName("points")->getChildElement(0);
    point->setAttribute("x", 50.0);


    auto params = xml.getChildByName("graph")->getChildByName("lines")->getChildElement(0)->getChildByName("parameters");
    params->setAttribute("locut", 0.5);
    auto delayLength = params->getChildByName("delayLength");
    delayLength->setAttribute("numerator", 3.0);

    delayGraph.importFromXml(&xml);
    auto finalXml = juce::XmlElement("plugin-state");
    delayGraph.exportToXml(&finalXml);
    REQUIRE( xml.isEquivalentTo(&finalXml, true));
}

TEST_CASE("Import blank XML", "[importinvalidxml]")
{
    auto xmlText = "<plugin-state>\n"
                   "</plugin-state>";
    auto xmlElement = juce::parseXML(xmlText);

    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    DelayGraph delayGraph;

    REQUIRE_FALSE(delayGraph.importFromXml(xmlElement.get()));
}

TEST_CASE("Import XML", "[importxml]")
{
    auto xmlText = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <plugin-state apvts=\"&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;&#13;&#10;&#13;&#10;&lt;DELAY-LINE-PARAMETERS&gt;&#13;&#10;  &lt;PARAM id=&quot;clear&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mix&quot; value=&quot;22.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod0&quot; value=&quot;0.25&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod1&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod2&quot; value=&quot;0.2&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod3&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod4&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod5&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod6&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod7&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod8&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod9&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;stretchtime&quot; value=&quot;1.5&quot;/&gt;&#13;&#10;&lt;/DELAY-LINE-PARAMETERS&gt;&#13;&#10;\"><graph><points><point id=\"1\" x=\"0.0\" y=\"0.0\" type=\"1\"/><point id=\"2\" x=\"100.0\" y=\"100.0\" type=\"2\"/><point id=\"3\" x=\"512.0\" y=\"128.4\" type=\"0\"/><point id=\"4\" x=\"460.0\" y=\"300.0\" type=\"0\"/><point id=\"5\" x=\"713.0\" y=\"197.0\" type=\"0\"/></points><lines><line id=\"1\" color=\"ffe68d19\" start=\"1\" end=\"3\"><parameters mutebypass=\"0.0\" lengthenvelopefollow=\"0.5\" moddepth=\"0.5\" modrate=\"0.5\" distortion=\"1.0\" distortiontype=\"0.0\" hicut=\"0.25\" locut=\"0.0\" gain=\"1.0\" invert=\"1.0\" gainenvelopefollow=\"0.5\" feedback=\"0.75\"><delayLength mode=\"3\" samples=\"0.0\" milliseconds=\"500.0\" hertz=\"10.0\" pitch=\"20.0\" numerator=\"1.0\" denominator=\"4.0\"/></parameters></line><line id=\"2\" color=\"ffc13e76\" start=\"3\" end=\"4\"><parameters mutebypass=\"0.0\" lengthenvelopefollow=\"0.5\" moddepth=\"0.0\" modrate=\"0.0\" distortion=\"0.0\" distortiontype=\"0.0\" hicut=\"1.0\" locut=\"0.0\" gain=\"0.5\" invert=\"0.0\" gainenvelopefollow=\"0.5\" feedback=\"0.0\"><delayLength mode=\"0\" samples=\"0.0\" milliseconds=\"500.0\" hertz=\"10.0\" pitch=\"100.0\" numerator=\"1.0\" denominator=\"4.0\"/></parameters></line><line id=\"3\" color=\"ff4dd12e\" start=\"3\" end=\"5\"><parameters mutebypass=\"0.0\" lengthenvelopefollow=\"0.5\" moddepth=\"0.0\" modrate=\"0.5\" distortion=\"0.0\" distortiontype=\"0.0\" hicut=\"1.0\" locut=\"0.0\" gain=\"0.5\" invert=\"0.0\" gainenvelopefollow=\"0.5\" feedback=\"0.0\"><delayLength mode=\"0\" samples=\"0.0\" milliseconds=\"500.0\" hertz=\"10.0\" pitch=\"100.0\" numerator=\"1.0\" denominator=\"4.0\"/></parameters></line><line id=\"4\" color=\"ffa65984\" start=\"4\" end=\"2\"><parameters mutebypass=\"0.0\" lengthenvelopefollow=\"0.5\" moddepth=\"0.0\" modrate=\"0.5\" distortion=\"0.0\" distortiontype=\"0.0\" hicut=\"1.0\" locut=\"0.0\" gain=\"0.5\" invert=\"0.0\" gainenvelopefollow=\"0.5\" feedback=\"0.0\"><delayLength mode=\"0\" samples=\"0.0\" milliseconds=\"500.0\" hertz=\"10.0\" pitch=\"100.0\" numerator=\"1.0\" denominator=\"4.0\"/></parameters></line></lines></graph></plugin-state>";
    auto xmlElement = juce::parseXML(xmlText);

    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};
    PluginProcessor p;
    auto data = juce::MemoryBlock();
    p.getStateInformation(data);
    auto xml = p.getXmlFromBinary(data.begin(), data.getSize());
    // REQUIRE( xmlElement->isEquivalentTo(xml.get(), true));
}

TEST_CASE( "set preset", "[setpreset]")
{
    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    PluginProcessor testPlugin;
    auto p = PresetBrowser(testPlugin.delayGraph);
    p.presetMenu.setSelectedId(2, juce::NotificationType::sendNotificationSync);
    auto finalXml = juce::XmlElement("plugin-state");
    testPlugin.delayGraph.exportToXml(&finalXml);
    auto desired = juce::parseXML(R"(<?xml version="1.0" encoding="UTF-8"?>
                   <plugin-state>
                   <graph>
                   <points>
                   <point id="1" x="564.0" y="68.0" type="1"/>
                   <point id="2" x="277.0" y="116.0" type="2"/>
                   <point id="3" x="539.0" y="281.0" type="0"/>
                   </points>
                   <lines>
                   <line id="1" color="ff8fe800" start="1" end="2">
                   <parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137"
                       distortion="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0"
                       gainenvelopefollow="0.5" feedback="0.0">
                   <delayLength mode="0" samples="0.0" milliseconds="1028.791870117188" hertz="10.0"
                       pitch="100.0" numerator="1.0" denominator="4.0"/>
                   </parameters>
                   </line>
                   <line id="3" color="ff8fe800" start="2" end="3">
                   <parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137"
                       distortion="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0"
                       gainenvelopefollow="0.5" feedback="0.0">
                   <delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0"
                       numerator="1.0" denominator="4.0"/>
                   </parameters>
                   </line>
                   <line id="2" color="ff8fe800" start="3" end="1">
                   <parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137"
                       distortion="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0"
                       gainenvelopefollow="0.5" feedback="0.0">
                   <delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0"
                       numerator="1.0" denominator="4.0"/>
                   </parameters>
                   </line>
                   </lines>
                   </graph>
               </plugin-state>)");
    // REQUIRE( desired->isEquivalentTo(&finalXml, true));
}

TEST_CASE( "playing from preset", "[playpreset]")
{
    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    DelayGraph delayGraph;

    auto spec = juce::dsp::ProcessSpec();
    spec.sampleRate = 8000;
    spec.maximumBlockSize = 512;
    spec.numChannels = 2;

    delayGraph.prepareToPlay(spec);

    auto p = PresetBrowser(delayGraph);
    p.presetMenu.setSelectedId(2, juce::NotificationType::sendNotificationSync);

    auto sample = std::vector<float>(2);

    for (auto& line : delayGraph.getLines()) {
        line->setBypass(true);
        for (int i = 0; i < 100; ++i) {
            delayGraph.processSample(sample);
        }
        line->setBypass(false);
        for (int i = 0; i < 100; ++i) {
            delayGraph.processSample(sample);
        }
    }
}

TEST_CASE("send audio", "[sendaudio]")
{
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    DelayGraph delayGraph;

    auto spec = juce::dsp::ProcessSpec();
    spec.sampleRate = 8000;
    spec.maximumBlockSize = 512;
    spec.numChannels = 2;
    delayGraph.prepareToPlay(spec);
    auto sample = std::vector<float>(2);

    for (int i = 0; i < 100000; ++i) {
        sample[0] = 1;
        sample[1] = 1;
        delayGraph.processSample(sample);
    }

    REQUIRE((juce::approximatelyEqual(sample[0], 1.f) && juce::approximatelyEqual(sample[1], 1.f)));
}

TEST_CASE("delay line internal", "[delayinternal]")
{
    auto d = DelayLineInternal({44100, 512, 2}, 4, 100, nullptr);
    auto sample = std::vector<float>(2);
    for (auto i = 0; i < 10; ++i) {
        sample[0] = 1;
        sample[1] = 1;
        d.pushSample(sample);
        sample[0] = 0;
        sample[1] = 0;
        d.popSample(sample);
    }
    REQUIRE(juce::approximatelyEqual(sample[0], 1.0f));
    REQUIRE(juce::approximatelyEqual(sample[1], 1.0f));
}


TEST_CASE("global inputs", "[globalinputs]")
{
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    DelayGraph delayGraph;

    auto xml = juce::parseXML("<plugin-state>\n"
                               "  <graph>\n"
                               "    <points>\n"
                               "      <point id=\"1\" x=\"0.0\" y=\"0.0\" type=\"1\"/>\n"
                               "      <point id=\"2\" x=\"378.0\" y=\"150.0\" type=\"2\"/>\n"
                               "      <point id=\"4\" x=\"190.0\" y=\"81.0\" type=\"0\"/>\n"
                               "    </points>\n"
                               "    <lines>\n"
                               "      <line id=\"1\" start=\"1\" end=\"4\">\n"
                               "        <parameters mutebypass=\"0.0\" lengthenvelopefollow=\"0.5\" moddepth=\"0.0\" modrate=\"0.03010033443570137\"\n"
                               "                    distortion=\"0.0\" hicut=\"1.0\" locut=\"0.0\" gain=\"0.5\" invert=\"0.0\"\n"
                               "                    gainenvelopefollow=\"0.5\" feedback=\"0.0\">\n"
                               "          <delayLength mode=\"1\" samples=\"1.0\" milliseconds=\"10\" hertz=\"10.0\"\n"
                               "                       pitch=\"100.0\" numerator=\"1.0\" denominator=\"4.0\"/>\n"
                               "        </parameters>\n"
                               "      </line>\n"
                               "      <line id=\"2\" start=\"4\" end=\"2\">\n"
                               "        <parameters mutebypass=\"0.0\" lengthenvelopefollow=\"0.5\" moddepth=\"0.0\" modrate=\"0.03010033443570137\"\n"
                               "                    distortion=\"0.0\" hicut=\"1.0\" locut=\"0.0\" gain=\"0.5\" invert=\"0.0\"\n"
                               "                    gainenvelopefollow=\"0.5\" feedback=\"0.0\">\n"
                               "          <delayLength mode=\"1\" samples=\"3.0\" milliseconds=\"10\" hertz=\"10.0\"\n"
                               "                       pitch=\"100.0\" numerator=\"1.0\" denominator=\"4.0\"/>\n"
                               "        </parameters>\n"
                               "      </line>\n"
                               "    </lines>\n"
                               "  </graph>\n"
                               "</plugin-state>");
    delayGraph.importFromXml(xml.get());

    delayGraph.setRealOutputs();

    auto start = delayGraph.startPoint;
    auto end = delayGraph.endPoint;
    auto middle = delayGraph.getPoint(4);

    for (auto& line : delayGraph.getLines()) {
        REQUIRE(line->realOutputs.size() == 1);
        if (line->identifier == 1) {
            REQUIRE((*line->realOutputs.begin())->identifier == 4);
            REQUIRE(line->end->identifier == 4);
            REQUIRE(line->start->identifier == 1);
            REQUIRE(line->start == start);
            REQUIRE(line->end == middle);
        } else if (line->identifier == 2) {
            REQUIRE((*line->realOutputs.begin())->identifier == 2);
            REQUIRE(line->start->identifier == 4);
            REQUIRE(line->end->identifier == 2);
            REQUIRE(line->start == middle);
            REQUIRE(line->end == end);
        }
    }

    auto spec = juce::dsp::ProcessSpec();
    spec.sampleRate = 44100;
    spec.maximumBlockSize = 512;
    spec.numChannels = 2;
    delayGraph.prepareToPlay(spec);
    auto sample = std::vector<float>(2);

    for (int i = 0; i < 10; ++i) {
        sample[0] = 1;
        sample[1] = 1;
        delayGraph.processSample(sample);
    }

    REQUIRE((juce::approximatelyEqual(sample[0], 1.f) && juce::approximatelyEqual(sample[1], 1.f)));
}

TEST_CASE("modulation", "[mod]")
{
    auto gui = juce::ScopedJuceInitialiser_GUI {};
    PluginProcessor p;
    auto lineid = p.delayGraph.getLines()[0]->identifier;
    p.modulationEngine.setMapping(4, {ModulatableKey::line, lineid, "locut"});
    p.modulationEngine.setParameterValue(4, 0.7f);
    REQUIRE(juce::approximatelyEqual(p.modulationEngine.getParameterValue(4), 0.7f));
    REQUIRE(juce::approximatelyEqual(p.delayGraph.getLines()[0]->parameters.loCut.convertTo0to1(p.delayGraph.getLines()[0]->parameters.loCut), 0.7f));
}

TEST_CASE("sample delay", "[sampledelay]")
{
    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};
    PluginProcessor p;
    const auto samplesPerBlock = 512;
    p.prepareToPlay(44100, samplesPerBlock);

    for (auto sampleDelayLength : {5.0, 1.0, 10.0, 100.0}) {
        auto xmlText = juce::String(R"(<?xml version="1.0" encoding="UTF-8"?> <plugin-state apvts="&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;&#13;&#10;&#13;&#10;&lt;DELAY-LINE-PARAMETERS&gt;&#13;&#10;  &lt;PARAM id=&quot;clear&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mix&quot; value=&quot;100.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod0&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod1&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod2&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod3&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod4&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod5&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod6&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod7&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod8&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;mod9&quot; value=&quot;0.0&quot;/&gt;&#13;&#10;  &lt;PARAM id=&quot;stretchtime&quot; value=&quot;0.09999999403953552&quot;/&gt;&#13;&#10;&lt;/DELAY-LINE-PARAMETERS&gt;&#13;&#10;"><graph><points><point id="1" x="0.0" y="0.0" type="1"/><point id="2" x="100.0" y="100.0" type="2"/></points><lines><line id="1" color="ff64a758" start="1" end="2"><parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137" distortion="0.0" distortiontype="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0" gainenvelopefollow="0.5" feedback="0.0" pan="0.5"><delayLength mode="1" samples=")")
                       + juce::String(sampleDelayLength)
                       + juce::String(R"(" milliseconds="500.0" hertz="10.0" pitch="100.0" numerator="1.0" denominator="4.0" midiTrackPitchShift="0.0"/></parameters></line></lines></graph><mod-mapping/></plugin-state>)");

        auto xml = juce::parseXML(xmlText);
        REQUIRE(xml);
        auto data = juce::MemoryBlock();
        p.copyXmlToBinary(*xml, data);
        p.setStateInformation(data.begin(), data.getSize());
        auto buffer = juce::AudioBuffer<float>(p.getTotalNumInputChannels(), samplesPerBlock);
        REQUIRE(juce::approximatelyEqual(p.delayGraph.getLines()[0]->parameters.length.getSamplesLength(), static_cast<float>(sampleDelayLength)));
        auto midiBuffer = juce::MidiBuffer();
        buffer.clear();
        // flush a bunch
        for (auto i = 0; i < 100; ++i) {
            p.processBlock(buffer, midiBuffer);
            for (auto channel = 0; channel < buffer.getNumChannels(); ++channel) {
                auto minMax = buffer.findMinMax(channel, 0, buffer.getNumSamples());
                REQUIRE(juce::approximatelyEqual(minMax.getStart(), 0.f));
                REQUIRE(juce::approximatelyEqual(minMax.getEnd(), 0.f));
            }
        }
        buffer.setSample(0, 10, 1);
        p.processBlock(buffer, midiBuffer);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            if (juce::approximatelyEqual(static_cast<double>(i), 10 + sampleDelayLength)) {
                REQUIRE(juce::approximatelyEqual(buffer.getSample(0, i), 1.f));
            } else {
                REQUIRE(juce::approximatelyEqual(buffer.getSample(0, i), 0.f));
            }
        }
        buffer.clear();
        for (auto i = 0; i < 10000; ++i) {
            p.processBlock(buffer, midiBuffer);
            for (auto channel = 0; channel < buffer.getNumChannels(); ++channel) {
                auto minMax = buffer.findMinMax(channel, 0, buffer.getNumSamples());
                REQUIRE(juce::approximatelyEqual(minMax.getStart(), 0.f));
                REQUIRE(juce::approximatelyEqual(minMax.getEnd(), 0.f));
            }
        }
    }
}

#ifdef PAMPLEJUCE_IPP
    #include <ipp.h>

TEST_CASE ("IPP version", "[ipp]")
{
    CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2021.10.1 (r0x8e799c51)"));
}
#endif
