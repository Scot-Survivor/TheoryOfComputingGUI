//
// Created by scot on 26/10/23.
//

#ifndef REGEXTOOL_GRAPHNODE_H
#define REGEXTOOL_GRAPHNODE_H

#define ITERATIONS 1000
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include "imgui.h"
#include "imgui_internal.h"

/**
 * A graph node is a node in a graph.
 * It uses circles in Dear ImGUI on render via the draw function.
 */
class GraphNode {
private:
    ImVec2 position;
    float radius;
    std::string label;
    std::vector<GraphNode*> connected_nodes;
    bool is_start_node = false;
    bool is_final_node = false;
public:
    GraphNode(ImVec2 position, float radius, std::string label);
    GraphNode(ImVec2 position, float radius, std::string label, bool is_start_node);
    GraphNode(ImVec2 position, float radius, std::string label, bool is_start_node, bool is_final_node);
    ~GraphNode() = default;


    [[nodiscard]] ImVec2 get_position() const { return position; }
    void set_position(ImVec2 p_position) { this->position = p_position; }


    [[nodiscard]] float get_radius() const { return radius; }
    [[nodiscard]] std::string get_label() const { return label; }
    [[nodiscard]] bool get_is_start_node() const { return is_start_node; }
    [[nodiscard]] bool get_is_final_node() const { return is_final_node; }

    void add_connection(GraphNode* node) {
        if (std::find(connected_nodes.begin(), connected_nodes.end( ), node) == connected_nodes.end() && node != this) {
            connected_nodes.push_back(node);
        }
    }

    [[nodiscard]] std::vector<GraphNode*> get_connected_nodes() const {
        return connected_nodes;
    }
};
#endif //REGEXTOOL_GRAPHNODE_H
