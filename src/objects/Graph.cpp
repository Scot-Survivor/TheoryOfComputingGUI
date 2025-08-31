//
// Created by scot on 01/11/23.
//
#define _USE_MATH_DEFINES
#include "Graph.h"


Graph::Graph() {
    this->q = sycl::queue(sycl::default_selector_v);
}

std::set<std::string> Graph::get_alphabet()
{
    std::set<std::string> alphabet;
    for (auto edge: this->edges)
    {
        if (edge.label !=  std::string{_EPSILON}) alphabet.insert(edge.label);
    }
    return alphabet;
}

std::set<GraphNode*> Graph::get_targets(GraphNode* start, std::string literal)
{
    std::set<GraphNode*> targets;
    int start_idx = this->get_node_index(start);
    auto edges = this->get_edges();
    for (auto edge: edges)
    {
        if (edge.n2 == start_idx) continue;
        if (edge.label != literal) continue;
        targets.insert(this->get_node(edge.n2));
    }
    return targets;
}


void Graph::draw_edges(ImDrawList *drawList) {
    if (this->nodes.empty()) return;
    for (GraphNode* node : nodes) {
        int nodeIdx = this->get_node_index(node);
        for (GraphNode* connected_node : node->get_connected_nodes()) {
            int cNodeIdx = this->get_node_index(connected_node);
            if (cNodeIdx == -1) {
                std::cerr << "Warning Child Node not found in list." << std::endl;
                continue;
            }
            if (node != connected_node) {
                ImVec2 start = node->get_position();
                ImVec2 end = connected_node->get_position();
                draw_edge(drawList, start, end, get_edge(nodeIdx, cNodeIdx));
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

void Graph::add_node(GraphNode *node, const std::vector<EdgeData>& parent_nodes) {
    for (EdgeData parent_node : parent_nodes) {
        parent_node.n2 = nodes.size();
        nodes[parent_node.n1]->add_connection(node);
        edges.push_back(parent_node);
    }
    nodes.push_back(node);
    // edges.insert(edges.end(), parent_nodes.begin(), parent_nodes.end());
}

void Graph::add_node(GraphNode *node) {
    if (this->nodes.empty()) root_node = node;
    nodes.push_back(node);
    if (!node->get_connected_nodes().empty()) {
        // Build edge data from connected nodes
        std::vector<EdgeData> edge_data;
        int nodeIdx = nodes.size()-1;  // +1 once the node is added later
        for (GraphNode* connected_node : node->get_connected_nodes()) {
            int connected_node_idx = get_node_index(connected_node);
            if (connected_node_idx == -1) {
                nodes.push_back(connected_node);
                connected_node_idx = nodes.size()-1;
            };
            edge_data.push_back({node->get_label(), nodeIdx, connected_node_idx});
        }
    }
}

void Graph::add_nodes(const std::vector<GraphNode*>& nodes)
{
    for (const auto node: nodes)
    {
        add_node(node);
    }
}

GraphNode* Graph::get_start_node()
{
    for (auto node: nodes)
    {
        if (node->get_is_start_node()) return node;
    }
}

GraphNode* Graph::get_final_node()
{
    for (auto node: nodes)
    {
        if (node->get_is_final_node()) return node;
    }
}

std::set<GraphNode*> Graph::compute_epsilon_closure(std::vector<GraphNode*> nodes)
{
    std::set epsilon_closure(nodes.begin(), nodes.end());
    std::queue<GraphNode*> to_visit;
    for (auto node : nodes) to_visit.push(node);

    while (!to_visit.empty()) {
        GraphNode* current = to_visit.front();
        to_visit.pop();
        auto edges = this->get_edges(current);
        for (const auto& edge : edges) {
            if (edge.label == std::string{_EPSILON}) {
                GraphNode* next = this->get_node(edge.n2);
                if (epsilon_closure.insert(next).second) {
                    to_visit.push(next);
                }
            }
        }
    }
    return epsilon_closure;
}

void Graph::merge(Graph& other)
{
    // Add all nodes from 'other' to this graph
    for (auto node : other.nodes) {
        if (!this->has_node(node)) {
            this->add_node(node); // Deep copy
        }
    }
    // Add all edges from 'other' to this graph
    for (const auto& edge : other.edges) {
        this->add_edge(EdgeData{edge.label, get_node_index(other.nodes[edge.n1]),
            get_node_index(other.nodes[edge.n2])});
    }
}

bool Graph::has_node(GraphNode* node)
{
    return std::find(nodes.begin(), nodes.end(), node) != nodes.end();
}

void Graph::draw(ImDrawList *drawList) {
    this->draw_edges(drawList);
    for (GraphNode* node : nodes) {
        this->draw_edges(drawList);
        ImVec2 position = node->get_position();
        drawList->AddCircleFilled(position, node->get_radius(), node_color);
        drawList->AddCircle(position, node->get_radius(), border_color, 12, border_size);

        if (node->get_is_start_node()) {
            drawList->AddCircle(position, node->get_radius() - 5, border_color, 12, border_size);
            // Off set an arrow by the radius of node since start state
            float arrow_head_length = 10;
            float arrow_head_angle = 0.5;
            float arrow_head_angle_x = arrow_head_length * cosf(arrow_head_angle);
            float arrow_head_angle_y = arrow_head_length * sinf(arrow_head_angle);
            const float padding = (node->get_radius() - (border_size*2));
            ImVec2 arrow_head_start = {(position.x - 2*padding) - arrow_head_angle_x, (position.y - 2*padding) - arrow_head_angle_y};
            drawList->AddLine({position.x - padding,position.y - padding},
                              arrow_head_start, this->border_color, this->border_size);
            arrow_head_angle_x = arrow_head_length * cosf(arrow_head_angle);
            arrow_head_angle_y = arrow_head_length * sinf(arrow_head_angle);
            arrow_head_start = {(position.x - 2*padding) - arrow_head_angle_x, (position.y - 2*padding) - arrow_head_angle_y};
            drawList->AddLine({position.x - padding,position.y - padding},
                              arrow_head_start, ImColor(0, 0 ,220), this->border_size);
        }
        if (node->get_is_final_node()) {
            drawList->AddCircle(position, node->get_radius() - 5, border_color, 12, border_size);
        }

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
    float width = ImGui::GetIO().DisplaySize.x;
    float height = ImGui::GetIO().DisplaySize.y;
    root_node->set_position({width / 2, height / 2});  // Bind to center screen.
    if (!root_node->get_is_pinned()) root_node->toggle_is_pinned();
    sycl::buffer<GraphNode*> node_buffer = this->get_node_buffer();
    sycl::buffer<EdgeData> edge_buffer = this->get_edge_buffer();
    for (int i = 0; i <= PHYSICS_ITERATIONS; i ++) {
        for (GraphNode* node : nodes) {
            if (node->get_is_pinned()) continue;

            float dx = node->get_position().x - width / 2;
            float dy = node->get_position().y - height / 2;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist == 0) continue;
            float force = 1 / dist * 0.3;
            dx *= force;
            dy *= force;
            node->set_position({node->get_position().x + dx, node->get_position().y + dy});
            this->sycl_node_data_pass(node, node_buffer);
            this->sycl_edge_data_pass(edge_buffer, node_buffer);

            ImVec2 pos = node->get_position();
            pos.x = std::clamp(pos.x, node->get_radius(), width - node->get_radius());
            pos.y = std::clamp(pos.y, node->get_radius(), height - node->get_radius());
            node->set_position(pos);
        }
    }
}

void Graph::sycl_node_data_pass(GraphNode* node, sycl::buffer<GraphNode*> node_buffer) {
    q.submit([&](sycl::handler &h) {
        auto node_accessor = node_buffer.get_access<sycl::access::mode::read_write>(h);
        h.parallel_for(sycl::range<1>(node_accessor.size()), [=](sycl::id<1> idx) {
            GraphNode* other_node = node_accessor[idx];
            if (other_node == node || other_node->get_is_pinned()) return;
            float dx = node->get_position().x - other_node->get_position().x;
            float dy = node->get_position().y - other_node->get_position().y;
            float dist = sycl::sqrt(dx * dx + dy * dy);
            if (dist < MINIMUM_NODE_DISTANCE) {
                float force = (MINIMUM_NODE_DISTANCE - dist) / (dist / 2);
                dx *= force;
                dy *= force;
                node->set_position({node->get_position().x + dx, node->get_position().y + dy});
            }
        });
    });
}

void Graph::sycl_edge_data_pass(sycl::buffer<EdgeData> edge_buffer, sycl::buffer<GraphNode*> node_buffer) {
    q.submit([&](sycl::handler &h) {
        auto edge_accessor = edge_buffer.get_access<sycl::access::mode::read_write>(h);
        auto node_accessor = node_buffer.get_access<sycl::access::mode::read_write>(h);
        h.parallel_for(sycl::range<1>(edge_accessor.size()), [=](sycl::id<1> idx) {
            EdgeData edge = edge_accessor[idx];
            GraphNode* begin = node_accessor[edge.n1];
            GraphNode* end = node_accessor[edge.n2];
            if (begin->get_is_pinned() && end->get_is_pinned()) return;
            float dx = end->get_position().x - begin->get_position().x;
            float dy = end->get_position().y - begin->get_position().y;
            float dist = sycl::sqrt(dx * dx + dy * dy);
            if (dist == 0) return;
            float force = 2 * (dist - EDGE_LENGTH) * sycl::pown(dist, -2);
            dx *= force;
            dy *= force;
            if (!begin->get_is_pinned()) begin->set_position({begin->get_position().x + dx, begin->get_position().y + dy});
            if (!end->get_is_pinned()) end->set_position({end->get_position().x - dx, end->get_position().y - dy});
        });
    });
}

void Graph::add_edge(const EdgeData& edge) {
    nodes.at(edge.n1)->add_connection(nodes.at(edge.n2));
    edges.push_back(edge);
}

void Graph::add_edge(const std::vector<EdgeData>& p_edges) {
    for (const EdgeData& edge : p_edges) {
        add_edge(edge);
    }
}

void Graph::add_edge(std::string label, GraphNode* n1, GraphNode* n2)
{
    add_edge(EdgeData{label, get_node_index(n1), get_node_index(n2)});
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

GraphNode* Graph::get_last_node() const
{
    return nodes.back();
}

sycl::buffer<GraphNode *> Graph::get_node_buffer() {
    return {nodes.data(), nodes.size()};
}

sycl::buffer<EdgeData> Graph::get_edge_buffer() {
    return {edges.data(), edges.size()};
}

std::vector<GraphNode*> Graph::get_childless_nodes()
{
    std::vector<GraphNode*> childless;
    for (GraphNode* node : nodes) {
        if (node->get_connected_nodes().empty()) {
            childless.push_back(node);
        }
    }
    return childless;
}

std::vector<EdgeData> Graph::get_edges(GraphNode* node)
{
    std::vector<EdgeData> edges;
    int idx = this->get_node_index(node);
    for (auto edge : edges)
    {
        if (edge.n1 == idx || edge.n2 == idx)
        {
            edges.push_back(edge);
        }
    }
    return edges;
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

void Graph::draw_edge(ImDrawList *drawList, ImVec2 start, ImVec2 end, EdgeData edgeData) {
    ImVec2 diff = {end.x - start.x, end.y - start.y};
    float length = sqrtf(diff.x * diff.x + diff.y * diff.y);
    float angle = atan2f(diff.y, diff.x);
    float line_length = length - (root_node->get_radius() + root_node->get_radius());
    float line_length_x = line_length * cosf(angle);
    float line_length_y = line_length * sinf(angle);
    ImVec2 line_start = {start.x + (root_node->get_radius() * cosf(angle)), start.y + (root_node->get_radius() * sinf(angle))};
    ImVec2 line_end = {line_start.x + line_length_x, line_start.y + line_length_y};
    drawList->AddLine(line_start, line_end, this->border_color, this->border_size);


    // Add Label Text
    ImVec2 text_size = ImGui::CalcTextSize(edgeData.label.c_str());
    if (text_size.y == 0) text_size.y = 1; // Avoid zero-division.
    if (text_size.y > line_length) text_size.y = line_length;
    ImVec2 text_pos = {line_start.x + (line_length_x / 2) - (text_size.x / 2), line_start.y + (line_length_y / 2) - (text_size.y / 2)};
    // drawList->AddText(text_pos, ImColor(0, 0, 0), edgeData.label.c_str());
    ImGui::SetCursorPos(text_pos);
    ImGui::Text(edgeData.label.c_str(), nullptr, true);


    // Add arrow head parts
    float arrow_head_length = 10;
    float arrow_head_angle = 0.5;
    float arrow_head_angle_x = arrow_head_length * cosf(angle + arrow_head_angle);
    float arrow_head_angle_y = arrow_head_length * sinf(angle + arrow_head_angle);
    ImVec2 arrow_head_start = {line_end.x - arrow_head_angle_x, line_end.y - arrow_head_angle_y};
    drawList->AddLine(line_end, arrow_head_start, this->border_color, this->border_size);
    arrow_head_angle_x = arrow_head_length * cosf(angle - arrow_head_angle);
    arrow_head_angle_y = arrow_head_length * sinf(angle - arrow_head_angle);
    arrow_head_start = {line_end.x - arrow_head_angle_x, line_end.y - arrow_head_angle_y};
    drawList->AddLine(line_end, arrow_head_start, this->border_color, this->border_size);

}

EdgeData Graph::get_edge(int n1, int n2) {
    for (const EdgeData& edge : edges) {
        if (edge.n1 == n1 && edge.n2 == n2) {
            return edge;
        }
    }
    return {"", -1, -1};
}

EdgeData Graph::get_edge(GraphNode *n1, GraphNode *n2) {
    return get_edge((int) (nodes.begin() - std::find(nodes.begin(), nodes.end(), n1)),
                    (int) (nodes.begin() - std::find(nodes.begin(), nodes.end(), n2)));
}

int Graph::get_node_index(GraphNode *node) {
    for (int i = 0; i < nodes.size(); i++) {
        if (node == nodes.at(i)) return i;
    }
    return -1;
}
