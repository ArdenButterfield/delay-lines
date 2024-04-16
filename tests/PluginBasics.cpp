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
            Catch::Matchers::Equals ("Delay Lines"));
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

TEST_CASE("Import XML", "[importxml]")
{
    auto xmlText = "<plugin-state>\n"
                   "  <graph>\n"
                   "    <points>\n"
                   "      <point id=\"1\" x=\"100.0\" y=\"100.0\" type=\"2\"/>\n"
                   "      <point id=\"2\" x=\"400.0\" y=\"100.0\" type=\"0\"/>\n"
                   "      <point id=\"3\" x=\"50.0\" y=\"0.0\" type=\"1\"/>\n"
                   "    </points>\n"
                   "    <lines>\n"
                   "      <line id=\"2\" start=\"1\" end=\"2\">\n"
                   "        <parameters mutebypass=\"0.0\" lengthenvelopefollow=\"0.5\" moddepth=\"0.0\" modrate=\"0.03010033443570137\"\n"
                   "                    distortion=\"0.0\" hicut=\"1.0\" locut=\"0.5\" gain=\"0.5\" invert=\"0.0\"\n"
                   "                    gainenvelopefollow=\"0.5\" feedback=\"0.0\">\n"
                   "          <delayLength mode=\"0\" samples=\"0.0\" milliseconds=\"500.0\" hertz=\"10.0\" pitch=\"100.0\"\n"
                   "                       numerator=\"3.0\" denominator=\"4.0\"/>\n"
                   "        </parameters>\n"
                   "      </line>\n"
                   "    </lines>\n"
                   "  </graph>\n"
                   "</plugin-state>";
    auto xmlElement = juce::parseXML(xmlText);

    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI {};

    DelayGraph delayGraph;

    delayGraph.importFromXml(xmlElement.get());
    auto finalXml = juce::XmlElement("plugin-state");
    delayGraph.exportToXml(&finalXml);
    REQUIRE( xmlElement->isEquivalentTo(&finalXml, true));
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
    // std::cout << finalXml.toString();
    auto desired = juce::parseXML(R"(<?xml version="1.0" encoding="UTF-8"?>
                   <plugin-state>
                   <graph>
                   <points>
                   <point id="1" x="564.0" y="68.0" type="1"/>
                   <point id="2" x="277.0" y="116.0" type="2"/>
                   <point id="3" x="539.0" y="281.0" type="0"/>
                   </points>
                   <lines>
                   <line id="1" start="1" end="2">
                   <parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137"
                       distortion="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0"
                       gainenvelopefollow="0.5" feedback="0.0">
                   <delayLength mode="0" samples="0.0" milliseconds="1028.791870117188" hertz="10.0"
                       pitch="100.0" numerator="1.0" denominator="4.0"/>
                   </parameters>
                   </line>
                   <line id="3" start="2" end="3">
                   <parameters mutebypass="0.0" lengthenvelopefollow="0.5" moddepth="0.0" modrate="0.03010033443570137"
                       distortion="0.0" hicut="1.0" locut="0.0" gain="0.5" invert="0.0"
                       gainenvelopefollow="0.5" feedback="0.0">
                   <delayLength mode="0" samples="0.0" milliseconds="500.0" hertz="10.0" pitch="100.0"
                       numerator="1.0" denominator="4.0"/>
                   </parameters>
                   </line>
                   <line id="2" start="3" end="1">
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
    REQUIRE( desired->isEquivalentTo(&finalXml, true));
}

#ifdef PAMPLEJUCE_IPP
    #include <ipp.h>

TEST_CASE ("IPP version", "[ipp]")
{
    CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2021.10.1 (r0x8e799c51)"));
}
#endif
