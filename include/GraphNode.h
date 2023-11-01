//
// Created by scot on 26/10/23.
//

#ifndef REGEXTOOL_GRAPHNODE_H
#define REGEXTOOL_GRAPHNODE_H

#define ITERATIONS 1000
#include <string>
#include <utility>
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"

struct node_data {
    float x, y, width, height;
};

/**
 * A graph node is a node in a graph.
 * It uses circles in Dear ImGUI on render via the draw function.
 */
class GraphNode {
private:
    ImVec2 position;
    float radius;
    ImColor color;
    ImColor border_color;
    float border_size;
    std::string label;
    ImFont* font;
    float font_scale = 40;
    ImVec2 text_size;
    std::vector<GraphNode*> connected_nodes;
    bool has_drawn = false;
public:
    GraphNode(ImVec2 position, float radius, ImColor color, ImColor border_color, float border_size, std::string label);
    GraphNode(ImVec2 position, float radius, ImColor color, ImColor border_color, float border_size, std::string label, float font_size) : GraphNode(position, radius, color, border_color, border_size, std::move(label)) {
        this->font_scale = font_size;
    }
    void draw(ImDrawList* drawList);

    /**
     * Data pass, this computes the positions of nodes.
     */
     void data_pass();

    /**
     * Evenly place nodes around the circle.
     * @param drawList
     */
    void draw_edges(ImDrawList* drawList);
    void calc_text_size() {
        text_size = ImGui::CalcTextSize(label.c_str(), nullptr, true);
    }
    [[nodiscard]] ImVec2 get_text_size() const { return text_size; }
    [[nodiscard]] ImVec2 get_position() const { return position; }
    void set_position(ImVec2 p_position) { this->position = p_position; }
    [[nodiscard]] ImVec2 get_text_position();
    void add_connection(GraphNode* node) {
        connected_nodes.push_back(node);
    }
    [[nodiscard]] std::vector<GraphNode*> get_connected_nodes() const {
        return connected_nodes;
    }
    std::vector<GraphNode*> get_in_depth_connected_nodes();
    std::vector<GraphNode*> get_unique_nodes();

    void set_drawn(bool value) {
        has_drawn = value;
    }

    void toggle_drawn() {
        has_drawn = !has_drawn;
    }
    [[nodiscard]] bool get_has_drawn() const {
        return has_drawn;
    }

    void cascade_set_drawn(bool value) {
        for (GraphNode* node : this->get_unique_nodes()) {
            node->set_drawn(value);
        }
    }
};
#endif //REGEXTOOL_GRAPHNODE_H
