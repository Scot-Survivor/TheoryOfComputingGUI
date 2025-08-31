//
// Created by scot on 26/10/23.
//

#include <algorithm>
#include "GraphNode.h"

GraphNode::GraphNode(std::string label, bool is_start_node, bool is_final_node)
{
    this -> position = ImVec2(0, 0);
    this->radius = DEFAULT_NODE_SIZE;
    this->is_start_node = is_start_node;
    this->is_final_node = is_final_node;
    this->label = label;
}

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

GraphNode::GraphNode(const std::string& string)
{
    this -> position = ImVec2(0, 0);
    this->radius = DEFAULT_NODE_SIZE;
    this->is_start_node = false;
    this->is_final_node = false;
    this->label = string;
}
