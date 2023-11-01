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
};
#endif //REGEXTOOL_STATES_H
