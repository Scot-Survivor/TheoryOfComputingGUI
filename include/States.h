//
// Created by scot on 01/11/23.
//

#ifndef REGEXTOOL_STATES_H
#define REGEXTOOL_STATES_H
struct GraphDrawnState {
    bool data_pass = false;
};

struct SelectedNodeState {
    GraphNode* selected_node;
    std::vector<GraphNode*> selected_nodes;
    std::vector<ImVec2> selected_nodes_start_positions;
    bool is_box_selecting = false;
    bool has_box_selected = false;
    bool has_set_start_pos = false;
    ImVec2 p1, p2;
    ImVec2 start_pos;
};
#endif //REGEXTOOL_STATES_H
