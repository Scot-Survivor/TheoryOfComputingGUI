//
// Created by scot on 26/10/23.
//

#include <algorithm>
#include "GraphNode.h"

GraphNode::GraphNode(ImVec2 position, float radius, std::string label) {
    this->position = position;
    this->radius = radius;
    this->label = std::move(label);
}
