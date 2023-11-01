//
// Created by scot on 01/11/23.
//

#ifndef REGEXTOOL_GRAPH_H
#define REGEXTOOL_GRAPH_H
#include "CL/sycl.hpp"
#include <vector>
#include <algorithm>
#include "GraphNode.h"

/**
 * Used in the logic of drawing for working out canvas size.
 */
struct node_data {
    float x, y, width, height;
};

/**
 * Simple Edge struct for storing edge data.
 */
struct EdgeData {
    int n1, n2;
};

/**
 * A graph is a collection of nodes, its purpose is to properly call the draw functions,
 * actual logic and data is handled by the graph node class.
 */
class Graph {
private:
    std::vector<GraphNode*> nodes;
    GraphNode *root_node;
    ImFont *font;
    float font_scale = 2.0f;
    ImColor border_color = ImColor(0, 0, 0);
    float border_size = 4.0f;
    ImColor node_color = ImColor(255, 255, 255);
    sycl::queue q;
public:
    Graph();
    ~Graph() = default;
    /**
     * Draw the graph.
     */
    void draw(ImDrawList* drawList);

    /**
     * Draw in the Edges of the nodes.
     * @param drawList
     */
    void draw_edges(ImDrawList* drawList);

    /**
     * Add a node to the node list, as well as connect it to the correct nodes.
     */
    void add_node(GraphNode* node, const std::vector<int>& parent_nodes);
    void add_node(GraphNode* node);

    /**
     * Methods to get nodes from internal node list.
     */
     GraphNode* get_node(int node_index) {
        return nodes[node_index];
     }
     GraphNode* get_node(ImVec2 position);

    /**
     * Get child nodes of a given node index.
     */
    [[nodiscard]] std::vector<GraphNode*> get_child_nodes(int node_index) const {
        return nodes[node_index]->get_connected_nodes();
    }


    /**
     * Get Text Position of a given node index.
     */
    [[nodiscard]] ImVec2 get_text_position(int node_index);
    [[nodiscard]] static ImVec2 get_text_position(GraphNode* node) ;

    /**
     * Data pass, is for computing the position of the nodes, as well as generating the metadata struct lists.
     */
     void data_pass();

     /**
      * Add a new edge between two or more nodes.
      * @param n1, n2...
      */
     void add_edge(EdgeData edge);
     void add_edge(int n1, int n2);
     void add_edge(const std::vector<EdgeData>& edges);

     /**
      * Get a buffer of the nodes.
      */
      sycl::buffer<GraphNode*> get_node_buffer();

};


#endif //REGEXTOOL_GRAPH_H
