//
// Created by scot on 01/11/23.
//

#include "Graph.h"


Graph::Graph() {
    this->q = sycl::queue(sycl::default_selector{});
}


void Graph::draw_edges(ImDrawList *drawList) {
    if (this->nodes.empty()) return;

    for (GraphNode* node : nodes) {
        for (GraphNode* connected_node : node->get_connected_nodes()) {
            if (node != connected_node) {
                ImVec2 start = node->get_position();
                ImVec2 end = connected_node->get_position();
                ImVec2 diff = {end.x - start.x, end.y - start.y};
                float length = sqrtf(diff.x * diff.x + diff.y * diff.y);
                float angle = atan2f(diff.y, diff.x);
                float line_length = length - (node->get_radius() + connected_node->get_radius());
                float line_length_x = line_length * cosf(angle);
                float line_length_y = line_length * sinf(angle);
                ImVec2 line_start = {start.x + (node->get_radius() * cosf(angle)), start.y + (node->get_radius() * sinf(angle))};
                ImVec2 line_end = {line_start.x + line_length_x, line_start.y + line_length_y};
                drawList->AddLine(line_start, line_end, this->border_color, this->border_size);
            } else {
                // Special Curve to point to itself
                ImVec2 start = node->get_position();
                ImVec2 end = start;

                float angle = 0;
                float angle_increment = (2 * (float)M_PI) / (float)node->get_connected_nodes().size();
                float radius = node->get_radius() + 10;
                float x = start.x + (radius * cosf(angle));
                float y = start.y + (radius * sinf(angle));
                ImVec2 line_start = {x, y};
                ImVec2 line_end = {x + 10, y + 10};
                drawList->AddBezierCubic(line_start, {x + 10, y + 10}, {x + 10, y + 10}, line_end, this->border_color, this->border_size);
            }
        }
    }
}

void Graph::add_node(GraphNode *node, const std::vector<int>& parent_nodes) {
    for (int parent_node : parent_nodes) {
        nodes[parent_node]->add_connection(node);
    }
    nodes.push_back(node);
}

void Graph::add_node(GraphNode *node) {
    if (this->nodes.empty()) root_node = node;
    nodes.push_back(node);
}

void Graph::draw(ImDrawList *drawList) {
    this->draw_edges(drawList);
    for (GraphNode* node : nodes) {
        this->draw_edges(drawList);
        ImVec2 position = node->get_position();
        drawList->AddCircleFilled(position, node->get_radius(), node_color);
        drawList->AddCircle(position, node->get_radius(), border_color, 12, border_size);

        font = ImGui::GetFont();
        float old_size = font->Scale;
        font->Scale *= font_scale;

        ImGui::PushFont(font);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0));
        ImGui::SetCursorPos(get_text_position(node));
        ImGui::Text(node->get_label().c_str(), nullptr, true);
        font->Scale = old_size;
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
}


ImVec2 Graph::get_text_position(int node_index) {
    return get_text_position(nodes[node_index]);
}

ImVec2 Graph::get_text_position(GraphNode *node) {
    ImVec2 text_size = ImGui::CalcTextSize(node->get_label().c_str());
    if (text_size.y == 0) text_size.y = 1; // Avoid zero-division.
    if (text_size.y > node->get_radius() * 2) text_size.y = node->get_radius() * 2;
    return {node->get_position().x - (text_size.x  / 2), node->get_position().y - ( text_size.y / 2)};
}

void Graph::data_pass() {
    // Place nodes around the circle.
    size_t num_nodes = nodes.size();
    if (num_nodes == 0) {
        return;
    }
    float padding = border_size / 2;
    float angle = 0;
    float angle_increment = (2 * (float)M_PI) / (float)num_nodes;

    std::vector<GraphNode*> seen_nodes;
    for (GraphNode* node : nodes) {
        for (GraphNode* connected_node : node->get_connected_nodes()) {
            if (std::find(seen_nodes.begin(), seen_nodes.end(), connected_node) != seen_nodes.end()) continue;
            seen_nodes.push_back(connected_node);
            float x = node->get_position().x + ((node->get_radius() + connected_node->get_radius()) * padding * cosf(angle));
            float y = node->get_position().y + ((node->get_radius() + connected_node->get_radius()) * padding * sinf(angle));
            connected_node->set_position({x, y});
            angle += angle_increment;
        }
    }

    // Check and move nodes that are too close to each other.
    for (GraphNode* node : nodes) {
        for (GraphNode* connected_node : node->get_connected_nodes()) {
            if (node == connected_node) continue;
            float distance = sqrtf(powf(node->get_position().x - connected_node->get_position().x, 2) + powf(node->get_position().y - connected_node->get_position().y, 2));
            if (distance < (node->get_radius() + connected_node->get_radius()) * padding) {
                float x = node->get_position().x + ((node->get_radius() + connected_node->get_radius()) * padding * cosf(angle));
                float y = node->get_position().y + ((node->get_radius() + connected_node->get_radius()) * padding * sinf(angle));
                connected_node->set_position({x, y});
                angle += angle_increment;
            }
        }
    }
}

void Graph::add_edge(EdgeData edge) {
    nodes.at(edge.n1)->add_connection(nodes.at(edge.n2));
}

void Graph::add_edge(int n1, int n2) {
    nodes.at(n1)->add_connection(nodes.at(n2));
}

void Graph::add_edge(const std::vector<EdgeData>& edges) {
    for (EdgeData edge : edges) {
        add_edge(edge);
    }
}

GraphNode *Graph::get_node(ImVec2 position) {
    auto node_buffer = this->get_node_buffer();
    sycl::buffer<int> ans(1);
    ans.get_host_access()[0] = -1;
    q.submit([&](sycl::handler &h) {
        auto node_accessor = node_buffer.get_access<sycl::access::mode::read>(h);
        auto ans_accessor = ans.get_access<sycl::access::mode::write>(h);
        h.parallel_for(sycl::range<1>(node_accessor.size()), [=](sycl::id<1> idx) {
            GraphNode* node = node_accessor[idx];
            float distance = sqrtf(powf(node->get_position().x - position.x, 2) + powf(node->get_position().y - position.y, 2));
            if (distance < node->get_radius()) {
                ans_accessor[0] = (int)idx[0];
            }
        });
    });
    int idx = ans.get_host_access()[0];
    if (idx == -1) return nullptr;
    return nodes[idx];
}

sycl::buffer<GraphNode *> Graph::get_node_buffer() {
    return {nodes.data(), nodes.size()};
}

std::vector<GraphNode *> Graph::select_nodes(ImVec2 p1, ImVec2 p2) {
    std::vector<GraphNode*> selected_nodes;
    // Copy nodes into selected_nodes
    std::copy(nodes.begin(), nodes.end(), std::back_inserter(selected_nodes));
    sycl::buffer<GraphNode*> selected_nodes_buffer(selected_nodes.data(), selected_nodes.size());
    q.submit([&](sycl::handler &h) {
        auto selected_nodes_accessor = selected_nodes_buffer.get_access<sycl::access::mode::read_write>(h);
        h.parallel_for(sycl::range<1>(selected_nodes_accessor.size()), [=](sycl::id<1> idx) {
            GraphNode* node = selected_nodes_accessor[idx];
            if (node->get_position().x < p1.x || node->get_position().x > p2.x || node->get_position().y < p1.y || node->get_position().y > p2.y) {
                selected_nodes_accessor[idx] = nullptr;
            }
        });
    });
    return selected_nodes;
}
