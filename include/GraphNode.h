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

    void calc_text_size() {
        text_size = ImGui::CalcTextSize(label.c_str(), nullptr, true);
    }
    [[nodiscard]] ImVec2 get_text_size() const { return text_size; }

    [[nodiscard]] ImVec2 get_position() const { return position; }
    void set_position(ImVec2 p_position) { this->position = p_position; }

    [[nodiscard]] ImVec2 get_text_position();

    [[nodiscard]] float get_radius() const { return radius; }
    [[nodiscard]] ImColor get_color() const { return color; }
    [[nodiscard]] ImColor get_border_color() const { return border_color; }
    [[nodiscard]] float get_border_size() const { return border_size; }
    [[nodiscard]] std::string get_label() const { return label; }
    [[nodiscard]] bool get_has_drawn() const { return has_drawn; }

    void add_connection(GraphNode* node) {
        connected_nodes.push_back(node);
    }
    [[nodiscard]] std::vector<GraphNode*> get_connected_nodes() const {
        return connected_nodes;
    }
};
#endif //REGEXTOOL_GRAPHNODE_H
