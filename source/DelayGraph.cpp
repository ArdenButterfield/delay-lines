//
// Created by arden on 3/14/24.
//

#include "DelayGraph.h"

void DelayGraph::addPoint(const GraphPoint& point, bool connectToSelected) {
    points.push_back(std::make_unique<GraphPoint>(point));
    if (connectToSelected) {
        auto added = points[points.size() - 1].get();
        addLine({activePoint, added});
        activePoint = added;
        activePointAction = creatingLine;
    }
}
const std::vector<std::unique_ptr<GraphPoint>>& DelayGraph::getPoints() {
    return points;
}

void DelayGraph::addLine (const GraphLine& line)
{
    lines.push_back(line);
}

const std::vector<GraphLine>& DelayGraph::getLines()
{
    return lines;
}
void DelayGraph::deletePoint (const GraphPoint* point)
{
    for (auto iter = lines.begin(); iter != lines.end(); ) {
        if (iter->start == point || iter->end == point) {
            iter = lines.erase(iter);
        } else {
            ++iter;
        }
    }
    for (auto iter = points.begin(); iter != points.end(); ) {
        if (&(*iter->get()) == point) {
            iter = points.erase(iter);
        } else {
            ++iter;
        }
    }

}
