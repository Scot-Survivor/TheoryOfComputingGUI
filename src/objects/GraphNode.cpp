//
// Created by scot on 26/10/23.
//

#include <algorithm>
#include "GraphNode.h"

void GraphNode::draw_edges(ImDrawList *drawList) {
    size_t num_nodes = connected_nodes.size();
    if (num_nodes == 0) {
        return;
    }

    for (GraphNode* connected_node : connected_nodes) {
        // Place nodes around the circle.
        ImVec2 start = this->position;
        ImVec2 end = connected_node->get_position();
        ImVec2 diff = {end.x - start.x, end.y - start.y};
        float length = sqrtf(diff.x * diff.x + diff.y * diff.y);
        float angle = atan2f(diff.y, diff.x);
        float line_length = length - (radius * 2);
        float line_length_x = line_length * cosf(angle);
        float line_length_y = line_length * sinf(angle);
        ImVec2 line_start = {start.x + (radius * cosf(angle)), start.y + (radius * sinf(angle))};
        ImVec2 line_end = {line_start.x + line_length_x, line_start.y + line_length_y};
        drawList->AddLine(line_start, line_end, border_color, border_size);
    }
}

void GraphNode::draw(ImDrawList *drawList) {
    if (this->has_drawn) return;
    this->data_pass();
    this->draw_edges(drawList);
    drawList->AddCircleFilled(position, radius, color);
    drawList->AddCircle(position, radius, border_color, 12, border_size);

    font = ImGui::GetFont();
    float old_size = font->Scale;
    font->Scale *= font_scale;

    ImGui::PushFont(font);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0));
    ImGui::SetCursorPos(get_text_position());
    ImGui::Text(label.c_str(), nullptr, true);
    font->Scale = old_size;
    ImGui::PopFont();
    ImGui::PopStyleColor();
    this->set_drawn(true);
    for (GraphNode* node : connected_nodes) {
        if (node == this || node->get_has_drawn()) continue;
        node->draw(drawList);
    }
}

GraphNode::GraphNode(ImVec2 position, float radius, ImColor color, ImColor border_color, float border_size,
                     std::string label) {
    this->position = position;
    this->radius = radius;
    this->color = color;
    this->border_color = border_color;
    this->border_size = border_size;
    this->label = std::move(label);
}

ImVec2 GraphNode::get_text_position() {
    this->calc_text_size();
    /* if (text_size.y == 0) text_size.y = 1; // Avoid zero-division.
    if (text_size.y > radius * 2) text_size.y = radius * 2; */
    return {position.x - (text_size.x  / 2), position.y - ( text_size.y / 2)};
}

std::vector<GraphNode *> GraphNode::get_unique_nodes() {
    std::vector<GraphNode*> nodes;
    for (GraphNode* node : this->connected_nodes) {
        nodes.push_back(node);
        std::vector<GraphNode*> child_nodes = node->get_in_depth_connected_nodes();
        for (GraphNode* connected_node : child_nodes) {
            if (connected_node != this && std::find(nodes.begin(), nodes.end(), connected_node) == nodes.end())
                nodes.push_back(connected_node);
        }
    }
    return nodes;
}

std::vector<GraphNode *> GraphNode::get_in_depth_connected_nodes() {
    std::vector<GraphNode*> nodes;
    for (GraphNode* node : this->connected_nodes) {
        nodes.push_back(node);
        std::vector<GraphNode*> child_nodes = node->get_in_depth_connected_nodes();
        for (GraphNode* connected_node : child_nodes) {
            nodes.push_back(connected_node);
        }
    }
    return nodes;
}

void GraphNode::data_pass() {
    // Place nodes around the circle.
    size_t num_nodes = connected_nodes.size();
    if (num_nodes == 0) {
        return;
    }
    float padding = border_size / 2;
    float angle = 0;
    float angle_increment = (2 * (float)M_PI) / (float)num_nodes;
    for (GraphNode* connected_node : connected_nodes) {
        float x = position.x + (radius * padding * cosf(angle));
        float y = position.y + (radius * padding * sinf(angle));
        connected_node->set_position({x, y});
        angle += angle_increment;
    }
}
