//
// Created by scot on 02/11/23.
//

#include "Converter.h"

Graph Converter::convert_regex_to_nfa(std::string regex) {
    if (regex.empty()) return {};

    // Initialise a Random Object for pos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1000);

    // ɛ = epsilon
    Graph nfa = Graph();
    std::stack<GraphNode*> node_stack = std::stack<GraphNode*>();
    nfa.add_node(new GraphNode(ImVec2((float) dis(gen), (float) dis(gen)), 50, regex, true));
    node_stack.push(nfa.get_node(0));
    for (int i = 0; i < regex.length(); i++) {
        int lastIdx = nfa.get_node_index(node_stack.top());
        std::string letter = regex.substr(0, i);
        if (regex[i] == '*') {
            if (i==0) throw std::runtime_error("Invalid Regex");
            std::string term = regex.substr(0, i) + regex.substr(0, i+1);
            // A* Epsilon transition to an epsilon node, and a 'AA*' transition to the next node.
            auto epsilon_node = new GraphNode(ImVec2((float) dis(gen), (float) dis(gen)), 50, "ɛ", false, true);
            auto next_node = new GraphNode(ImVec2((float) dis(gen), (float) dis(gen)), 50, term, false);
        } else if (regex[i] == '|') {
            continue;
        } else if (regex[i] == '(') {
            continue;
        } else if (regex[i] == ')') {
            continue;
        } else {
                continue; // nfa.add_node(new GraphNode(ImVec2(0, 0), 50, std::string(1, regex[i]), false));
            }
    }
    nfa.add_node(new GraphNode(ImVec2((float) dis(gen), (float) dis(gen)), 50, "ɛ", false, true));
    nfa.add_edge({"ɛ", nfa.get_node_index(nfa.get_node(0)), nfa.get_node_index(nfa.get_node(1))});
    return nfa;
}
