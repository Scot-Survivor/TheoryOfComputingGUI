//
// Created by scot on 01/11/23.
//

#ifndef REGEXTOOL_GRAPH_H
#define REGEXTOOL_GRAPH_H

#define PHYSICS_ITERATIONS 100
#define MINIMUM_NODE_DISTANCE 50
#define EDGE_LENGTH 300

#include "CL/sycl.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include "GraphNode.h"

/**
 * EdgeData
 */
struct EdgeData {
    std::string label;  // Edge label
    int n1, n2;  // Connecting nodes
};


/**
 * A graph is a collection of nodes, its purpose is to properly call the draw functions,
 * actual logic and data is handled by the graph node class.
 */
class Graph {
private:
    std::vector<GraphNode*> nodes;
    std::vector<EdgeData> edges;
    GraphNode *root_node;
    ImFont *font;
    float font_scale = 2.0f;
    ImColor border_color = ImColor(0, 0, 0);
    float border_size = 4.0f;
    ImColor node_color = ImColor(255, 255, 255);
    sycl::queue q;


    /**
     * Edges require an arrow head, this function draws the arrow head, and the edge.
     * @param drawList ImGUI Draw List
     * @param start Start pos
     * @param end End pos
     */
    void draw_edge(ImDrawList* drawList, ImVec2 start, ImVec2 end, EdgeData edgeData);

    /**
     * Get the edge data of a given edge.
     * @param n1
     * @param n2
     * @return
     */
    EdgeData get_edge(GraphNode* n1, GraphNode* n2);
    EdgeData get_edge(int n1, int n2);
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
    void add_node(GraphNode* node, const std::vector<EdgeData>& parent_nodes);
    void add_node(GraphNode* node);

    /**
     * Methods to get nodes from internal node list.
     */
     GraphNode* get_node(int node_index) {
        return nodes[node_index];
     }
     GraphNode* get_node(ImVec2 position);
     /**
      * Select nodes in a given area.
      * @param p1 Point 1
      * @param p2 Point 2
      * @return List of nodes in the area.
      */
     std::vector<GraphNode*> select_nodes(ImVec2 p1, ImVec2 p2);

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
     * Set Border Colour
     */
    void set_border_color(ImColor color) {
        this->border_color = color;
    }

    /**
     * Data pass, is for computing the position of the nodes, as well as generating the metadata struct lists.
     */
     void data_pass();

     /**
      * Add a new edge between two or more nodes.
      * @param n1, n2...
      */
     void add_edge(const EdgeData& edge);
     void add_edge(const std::vector<EdgeData>& edges);

     /**
      * Get a buffer of the nodes.
      */
      sycl::buffer<GraphNode*> get_node_buffer();

      /**
       * Clear the graph.
       */
      void clear() {
          nodes.clear();
      }

};


#endif //REGEXTOOL_GRAPH_H
