//
// Created by scot on 26/10/23.
//

#include <algorithm>
#include "GraphNode.h"

GraphNode::GraphNode(ImVec2 position, float radius, std::string label) {
    this->position = position;
    this->radius = radius;
    this->label = std::move(label);
    this->connected_nodes = std::vector<GraphNode*>();
}

GraphNode::GraphNode(ImVec2 position, float radius, std::string label, bool is_start_node) {
    this->position = position;
    this->radius = radius;
    this->label = std::move(label);
    this->connected_nodes = std::vector<GraphNode*>();
    this->is_start_node = is_start_node;
}

GraphNode::GraphNode(ImVec2 position, float radius, std::string label, bool is_start_node, bool is_final_node) {
    this->position = position;
    this->radius = radius;
    this->label = std::move(label);
    this->connected_nodes = std::vector<GraphNode*>();
    this->is_start_node = is_start_node;
    this->is_final_node = is_final_node;
}
