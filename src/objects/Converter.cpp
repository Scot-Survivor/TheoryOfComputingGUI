//
// Created by scot on 02/11/23.
//

#include "Converter.h"

#include <map>
#include <queue>
#include <stack>

/**
 *  Handle special characters, * and | with shunting yard algorithm
 * @param c the character to handle
 * @param operators the operator stack
 * @param postfix the postfix string to append to
 */
void handle_specials(char c, std::stack<char>* operators, std::string& postfix)
{
    while (!operators->empty() && ASTPrecedence.find(AST::get_type(operators->top())) != ASTPrecedence.end()
        && ASTPrecedence.at(AST::get_type(operators->top())) >= ASTPrecedence.at(AST::get_type(c)))
    {
        postfix += operators->top();
        operators->pop();
    }
    operators->push(c);
}

std::string filter_operators(std::string postfix)
{
    std::string filtered;
    for (char c : postfix)
    {
        if (ASTPrecedence.find(AST::get_type(c)) == ASTPrecedence.end())
        {
            filtered += c;
        }
    }
    return filtered;
}

std::string Converter::shunting_yard(const std::string& regex)
{
    std::string postfix;
    std::stack<char> operators;

    for (char c: regex)
    {
        switch (c)
        {
            case '(':
                operators.push(c);
                break;
            case ')' :
            while (operators.top() != '(' && !operators.empty())
            {
                postfix += operators.top();
                operators.pop();
            }
            if (!operators.empty()) operators.pop();
            break;

        case '*':
            handle_specials(c, &operators, postfix);
            break;

        case '|':
            handle_specials(c, &operators, postfix);
            break;

        default:
            postfix += c;
            break;
        }
    }
    while (!operators.empty())
    {
        postfix += operators.top();
        operators.pop();
    }
    return postfix;
}

Graph parse_node(ASTNode node, GraphNode* start, GraphNode* end)
{
    Graph g;
    g.add_node(start);
    g.add_node(end);
    switch (node.type)
    {
    case ASTType::EMPTY:
        {
            g.add_edge(std::string{_EPSILON}, start, end);
            break;
        }
    case ASTType::LITERAL:
        {
            g.add_edge(std::string{node.literal}, start, end);
            break;
        }
    case ASTType::PIPE_OR:
        {
            // Create new nodes for left and right branches
            auto left_start = new GraphNode("");
            auto left_end = new GraphNode("");
            auto right_start = new GraphNode("");
            auto right_end = new GraphNode("");

            // Add epsilon transitions from start to both branch starts
            g.add_node(left_start);
            g.add_edge(std::string{_EPSILON}, start, left_start);
            g.add_node(right_start);
            g.add_edge(std::string{_EPSILON}, start, right_start);

            // Recursively build left and right subgraphs
            Graph left = parse_node(node.children[0], left_start, left_end);
            Graph right = parse_node(node.children[1], right_start, right_end);

            g.merge(left);
            g.merge(right);

            // Add epsilon transitions from both branch ends to end
            g.add_edge(std::string{_EPSILON}, left_end, end);
            g.add_edge(std::string{_EPSILON}, right_end, end);
            break;
        }
    case ASTType::KLEENE_STAR:
        {
            auto mid_start = new GraphNode("");
            auto mid_end = new GraphNode("");
            Graph mid = parse_node(node.children[0], mid_start, mid_end);
            g.merge(mid);
            g.add_edge(std::string{_EPSILON}, start, end);
            g.add_edge(std::string{_EPSILON}, mid_start, end);
            g.add_edge(std::string{_EPSILON}, start, mid_start);
            g.add_edge(std::string{_EPSILON}, mid_end, mid_start);
            break;
        }
    case ASTType::GROUP:
        {
            Graph group = parse_node(node.children[0], start, end);
            g.merge(group);
            break;
        }
    }
    return g;
}

Graph Converter::convert_regex_to_nfa(const std::string& regex)
{
    std::vector<ASTNode> ast = AST::parse(regex);
    auto start = new GraphNode("", true, false);
    auto end = new GraphNode("", false, true);
    Graph g =  parse_node(ast[0], start, end);
    for (auto ast1 : std::vector(ast.begin()+1, ast.end()))
    {
        // implicitly concat
        // the start of new should be end of old (old end is not final)
        end->toggle_is_final_node();
        auto new_end = new GraphNode("", false, true);
        Graph g2 = parse_node(ast1, end, new_end);
        g.merge(g2);
        end = new_end;
    }
    // ensure all other nodes except the above are not start or end nodes
    for (GraphNode* node : g.get_nodes()) {
        if (node != start && node != end) {
            if (node->get_is_start_node()) node->toggle_is_start_node();
            if (node->get_is_final_node()) node->toggle_is_final_node();
        }
    }
    return g;
}

Graph Converter::convert_nfa_to_dfa(Graph nfa)
{
    std::map<std::set<GraphNode*>, GraphNode*> dfa_states;
    std::queue<std::set<GraphNode*>> to_process;
    Graph dfa;

    auto nfa_start = nfa.get_start_node();
    std::set<GraphNode*> start_set = nfa.compute_epsilon_closure({nfa_start});
    auto dfa_start = new GraphNode("", true, false);
    dfa_states[start_set] = dfa_start;
    dfa.add_node(dfa_start);
    to_process.push(start_set);

    while (!to_process.empty()) {
        auto current_set = to_process.front();
        to_process.pop();
        auto current_dfa_node = dfa_states[current_set];

        // For each input symbol (excluding epsilon)
        for (const std::string& symbol : nfa.get_alphabet()) {
            if (symbol == _EPSILON) continue;
            std::set<GraphNode*> next_set;
            for (auto nfa_node : current_set) {
                auto targets = nfa.get_targets(nfa_node, symbol);
                auto closure = nfa.compute_epsilon_closure(std::vector(targets.begin(), targets.end()));
                next_set.insert(closure.begin(), closure.end());
            }
            if (next_set.empty()) continue;

            // If this set is new, create a DFA node
            if (dfa_states.find(next_set) == dfa_states.end()) {
                bool is_final = false;
                for (auto node : next_set) {
                    if (node->get_is_final_node()) {
                        is_final = true;
                        break;
                    }
                }
                auto new_dfa_node = new GraphNode("DFA_" + std::to_string(dfa_states.size()), false, is_final);
                dfa_states[next_set] = new_dfa_node;
                dfa.add_node(new_dfa_node);
                to_process.push(next_set);
            }
            // Add DFA transition
            dfa.add_edge(symbol, current_dfa_node, dfa_states[next_set]);
        }
    }
    return dfa;
}
