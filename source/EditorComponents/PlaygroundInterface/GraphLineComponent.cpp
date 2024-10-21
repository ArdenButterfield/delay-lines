//
// Created by arden on 7/28/24.
//

#include "GraphLineComponent.h"
#include "../../DelayGraph.h"
#include "../LineEditor/LineEditor.h"
#include "PlaygroundInterface.h"

#include "juce_dsp/juce_dsp.h"

static juce::AffineTransform makeTransform(juce::Point<float> start, juce::Point<float> end, int channel) {
    auto transform = juce::AffineTransform();
    transform = transform.scaled(start.getDistanceFrom(end), channel ? 50 : -50);
    transform = transform.rotated(start.getAngleToPoint(end) - juce::MathConstants<float>::halfPi);
    transform = transform.translated(start);
    return transform;
}


GraphLineComponent::GraphLineComponent (ModulationMappingEngine& me, DelayGraph& _delayGraph, PlaygroundInterface* _playgroundInterface, int _id)
    : playgroundInterface(_playgroundInterface), delayGraph(_delayGraph), id(_id), mappingEngine(me) {
}

GraphLineComponent::~GraphLineComponent()
{
}

void GraphLineComponent::paint (juce::Graphics& g)
{
    auto line = delayGraph.getLine(id);
    if (!line) {
        return;
    }

    int loopingBackIndex;
    float radius;
    bool lineLoopsBack = getLoopbackStatus(loopingBackIndex, radius);


    auto linesSharingSpace = delayGraph.getAllLinesBetweenPoints(line->start, line->end);
    bool isHovered = false;
    auto startPoint = *(line->start) + playgroundInterface->getGlobalOffset();
    auto endPoint = *(line->end) + playgroundInterface->getGlobalOffset();


    if (delayGraph.interactionState == DelayGraph::lineHover && delayGraph.activeLine == line) {
        isHovered = true;
    }

    auto lineColourWithHover = (isHovered || (lineEditor && lineEditor->isVisible())) ? line->getColor().withMultipliedLightness(2.0) : line->getColor();

    if (line->parameters.isMuted()) {
        g.setColour(lineColourWithHover.withAlpha(0.1f));
        if (lineLoopsBack) {
            g.drawEllipse(startPoint.x - radius, startPoint.y - radius, 2 * radius, 2 * radius, 3);
        } else {
            g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 3);
        }
        return;
    }

    if (line->parameters.isBypassed()) {
        g.setColour(lineColourWithHover.withAlpha(0.5f));
        if (lineLoopsBack) {
            g.drawEllipse(startPoint.x - radius, startPoint.y - radius, 2 * radius, 2 * radius, 3);
        } else {
            g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 3);
        }
        return;
    }

    g.setColour(lineColourWithHover);

    if (lineLoopsBack) {
        auto linePath = juce::Path();
        linePath.startNewSubPath(startPoint.translated(0, radius));
        auto numSteps = 100;
        for (auto step = 0; step < numSteps; ++step) {
            float l, r;
            auto proportion = static_cast<float> (step) / static_cast<float> (numSteps);
            line->getEnvelope (proportion, l, r);
            linePath.lineTo (juce::Point<float> (0, juce::dsp::FastMathApproximations::tanh ((l + r) / 2) * loopingBackStep + radius)
                                 .rotatedAboutOrigin (juce::MathConstants<float>::twoPi * proportion)
                             + startPoint);
        }
        for (auto step = 0; step < numSteps; ++step) {
            auto proportion = static_cast<float> (step) / static_cast<float> (numSteps);
            linePath.lineTo(juce::Point<float>(0, radius)
                .rotatedAboutOrigin(-juce::MathConstants<float>::twoPi * proportion) + startPoint);
        }
        linePath.closeSubPath();
        g.fillPath(linePath);
    } else {
        auto leftLinePath = juce::Path();
        auto rightLinePath = juce::Path();
        auto innerLeftPath = juce::Path();
        auto innerRightPath = juce::Path();
        leftLinePath.startNewSubPath(0,-0.005);
        rightLinePath.startNewSubPath(0,-0.005);
        innerLeftPath.startNewSubPath(0,-0.005);
        innerRightPath.startNewSubPath(0,-0.005);
        float l,r;
        auto numSteps = static_cast<int>(startPoint.getDistanceFrom({endPoint.x, endPoint.y}));
        for (auto step = 0; step < numSteps; step += 1) {
            auto proportion = static_cast<float>(step) / static_cast<float>(numSteps);
            line->getEnvelope(proportion, l, r);
            l = std::max(l, 0.01f);
            r = std::max(r, 0.01f);
            auto window = juce::dsp::FastMathApproximations::sin(juce::MathConstants<float>::pi * proportion);
            leftLinePath.lineTo(proportion, juce::dsp::FastMathApproximations::tanh(l * window));
            rightLinePath.lineTo(proportion, juce::dsp::FastMathApproximations::tanh(r * window));
            innerLeftPath.lineTo(proportion, juce::dsp::FastMathApproximations::tanh(l * 0.5f * window));
            innerRightPath.lineTo(proportion, juce::dsp::FastMathApproximations::tanh(r * 0.5f * window));
        }
        leftLinePath.lineTo(1,-0.005);
        rightLinePath.lineTo(1,-0.005);
        innerLeftPath.lineTo(1,-0.005);
        innerRightPath.lineTo(1,-0.005);
        leftLinePath.closeSubPath();
        rightLinePath.closeSubPath();
        innerLeftPath.closeSubPath();
        innerRightPath.closeSubPath();

        if (linesSharingSpace.size() == 1) {
            g.setColour(lineColourWithHover);
            g.fillPath(leftLinePath, makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 0));
            g.fillPath(rightLinePath, makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 1));
            g.setColour(lineColourWithHover.withMultipliedBrightness(0.5f));
            g.fillPath(innerLeftPath, makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 0));
            g.fillPath(innerRightPath, makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 1));

        } else if (linesSharingSpace.size() > 1) {
            const float numMiddleLines = linesSharingSpace.size() - 2;
            auto idPosition = std::find(linesSharingSpace.begin(), linesSharingSpace.end(), id);
            jassert(idPosition != linesSharingSpace.end());
            if (idPosition - linesSharingSpace.begin() >= 2) {
                // Inner line
                auto path = juce::Path();
                auto innerIndex = (idPosition - linesSharingSpace.begin() - 2);
                auto lowY = -preTransformInnerLineWidth * numMiddleLines * 0.5f + innerIndex * preTransformInnerLineWidth;
                auto highY = lowY + preTransformInnerLineWidth;
                path.startNewSubPath(0, lowY);
                path.lineTo(0, highY);
                path.lineTo(1, highY);
                path.lineTo(1, lowY);
                path.closeSubPath();
                auto transform = makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, line->isGoingBackwards());
                g.setColour(lineColourWithHover);
                g.fillPath(path, transform);
            } else if ((id == linesSharingSpace[0]) == line->isGoingBackwards()) {
                auto transform = juce::AffineTransform()
                                     .translated(0, numMiddleLines * preTransformInnerLineWidth * 0.5f)
                                     .followedBy(makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 0));

                g.setColour(lineColourWithHover);
                g.fillPath(leftLinePath, transform);
                g.setColour(lineColourWithHover.withMultipliedBrightness(0.5f));
                g.fillPath(innerLeftPath, transform);
            } else {
                auto transform = juce::AffineTransform()
                                     .translated(0, numMiddleLines * preTransformInnerLineWidth * 0.5f)
                                     .followedBy(makeTransform(startPoint + line->start->offset, endPoint + line->end->offset, 1));

                g.setColour(lineColourWithHover);
                g.fillPath(rightLinePath, transform);
                g.setColour(lineColourWithHover.withMultipliedBrightness(0.5f));
                g.fillPath(innerRightPath, transform);
            }
        }
    }
}

void GraphLineComponent::resized()
{
    Component::resized();
}

bool GraphLineComponent::hitTest (int x, int y)
{
    if (lineEditor && lineEditor->isVisible()) {
        return true;
    }
    auto line = delayGraph.getLine(id);
    if (line == nullptr) {
        return false;
    }

    int loopBackIndex;
    float radius;
    if (getLoopbackStatus(loopBackIndex, radius)) {
        auto dist = line->start->getDistanceSquaredFrom({static_cast<float>(x),static_cast<float>(y)});
        return ((dist >= radius * radius) && (dist < (radius + loopingBackStep) * (radius + loopingBackStep)));
    }

    if (juce::approximatelyEqual(line->start->getDistanceSquaredFrom(*line->end), 0.f)) {
        return false;
    }

    auto linesSharingSpace = delayGraph.getAllLinesBetweenPoints(line->start, line->end);
    auto startPoint = *(line->start) + playgroundInterface->getGlobalOffset() + line->start->offset;
    auto endPoint = *(line->end) + playgroundInterface->getGlobalOffset() + line->end->offset;
    auto backwards = line->isGoingBackwards();
    auto reverseTransform = makeTransform(backwards ? endPoint : startPoint, backwards ? startPoint : endPoint, 0).inverted();

    auto transx = static_cast<float>(x);
    auto transy = static_cast<float>(y);
    reverseTransform.transformPoint(transx, transy);
    if (0 < transx && transx < 1) {
        auto above = false;
        auto below = false;
        if (0 <= transy && transy < juce::dsp::FastMathApproximations::sin(juce::MathConstants<float>::pi * transx)) {
            above = true;
        } else if (-juce::dsp::FastMathApproximations::sin(juce::MathConstants<float>::pi * transx) < transy && transy <= 0) {
            below = true;
        }
        auto linesSharingSpace = delayGraph.getAllLinesBetweenPoints(line->start, line->end);
        if (linesSharingSpace.size() < 2) {
            return above || below;
        } else if (linesSharingSpace.size() == 2) {
            auto shouldBeAbove = (id == linesSharingSpace[1]);
            return (above && shouldBeAbove) || (below && !shouldBeAbove);
        } else {
            auto idPosition = std::find(linesSharingSpace.begin(), linesSharingSpace.end(), id);
            jassert(idPosition != linesSharingSpace.end());
            const auto innersRadius = preTransformInnerLineWidth * 0.5f * (linesSharingSpace.size() - 2);
            if (idPosition - linesSharingSpace.begin() == 0) {
                auto isIn = (-innersRadius > transy) &&
                            (transy > -innersRadius - juce::dsp::FastMathApproximations::sin(juce::MathConstants<float>::pi * transx));
                return isIn;
            } else if (idPosition - linesSharingSpace.begin() == 1) {
                auto isIn = (innersRadius < transy) &&
                            (transy < innersRadius + juce::dsp::FastMathApproximations::sin(juce::MathConstants<float>::pi * transx));
                return isIn;
            } else {
                auto below = -innersRadius + preTransformInnerLineWidth * (idPosition - linesSharingSpace.begin() - 2);
                auto isIn = below < transy && transy < below + preTransformInnerLineWidth;
                return isIn;
            }
        }
    }
    return false;
}

void GraphLineComponent::mouseEnter (const juce::MouseEvent& event)
{
    delayGraph.activeLine = delayGraph.getLine(id);
    delayGraph.interactionState = DelayGraph::lineHover;
}

void GraphLineComponent::mouseExit (const juce::MouseEvent& event)
{
    delayGraph.interactionState = DelayGraph::none;
}

void GraphLineComponent::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasClicked() && lineEditor && lineEditor->isVisible()) {
        playgroundInterface->removeChildComponent(lineEditor.get());
        lineEditor.reset();
    } else if (event.mouseWasClicked() && event.mods.isShiftDown()) {
        delayGraph.activeLine->toggleEnabled();
    } else if (event.mouseWasClicked() && event.mods.isRightButtonDown()) {
        delayGraph.interactionState = DelayGraph::none;
        delayGraph.deleteLine(delayGraph.activeLine);
    } else if (event.mouseWasClicked()) {
        lineEditor = std::make_unique<LineEditor>(mappingEngine, delayGraph, id);
        playgroundInterface->addAndMakeVisible(*lineEditor);
        lineEditor->setBounds(LineEditor::getDesiredBounds());
    }
}

bool GraphLineComponent::getLoopbackStatus (int& index, float& radius)
{
    auto line = delayGraph.getLine(id);
    if (!line) {
        return false;
    }

    bool lineLoopsBack = false;
    radius = 0.f;
    index = 0;
    if (line->start == line->end) {
        radius += loopingBackRadius;
        auto linesSharingPoint = delayGraph.getAllLinesStartingAndEndingAtSamePoint(line->start);
        if (!linesSharingPoint.empty()) {
            for (auto& l : linesSharingPoint) {
                if (l == line->identifier) {
                    lineLoopsBack = true;
                    break;
                }
                ++index;
                radius += loopingBackStep;
            }
        }
    }
    return lineLoopsBack;
}
