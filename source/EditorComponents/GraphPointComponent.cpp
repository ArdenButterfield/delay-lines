//
// Created by arden on 7/28/24.
//

#include "GraphPointComponent.h"
#include "PlaygroundComponent.h"

GraphPointComponent::GraphPointComponent (DelayGraph& _delayGraph, PlaygroundComponent* _playgroundComponent, int id)
    : delayGraph(_delayGraph), playgroundComponent(_playgroundComponent), identifier(id) {

}

void GraphPointComponent::paint (juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    auto p = delayGraph.getPoint(identifier);
    if (p) {
        g.drawEllipse(p->x - 10, p->y - 10, 20, 20, 2);
    }
}

void GraphPointComponent::resized() {

}

bool GraphPointComponent::hitTest (int x, int y) {
    return false;
}