//
// Created by scot on 03/11/23.
//

#include "AST.h"

ASTType AST::get_type(char c) {
    switch (c) {
        case '*':
            return ASTType::KLEENE_STAR;
        case '|':
            return ASTType::PIPE_OR;
        case '(':
            return ASTType::GROUP_OPEN;
        case ')':
            return ASTType::GROUP_CLOSE;
        default:
            return ASTType::LITERAL;
    }
}

std::vector<ASTNode> AST::parse(std::string regex) {
    std::vector<ASTNode> ast = std::vector<ASTNode>();
    std::stack<ASTNode> node_stack = std::stack<ASTNode>();
    for (int i = 0; i < regex.length(); i++) {
        char c = regex[i];
        ASTNode node{};
        ASTNode group_node{};
        std::string group;
        std::vector<ASTNode> group_ast = std::vector<ASTNode>();
        switch (get_type(c)) {
            case ASTType::KLEENE_STAR:
                if (i == 0) throw std::runtime_error("Invalid Regex");
                node = ASTNode(ASTType::KLEENE_STAR);
                node.children.push_back(node_stack.top());
                node_stack.pop();
                node_stack.emplace(node);
                break;
            case ASTType::PIPE_OR:
                if (i == 0) throw std::runtime_error("Invalid Regex");
                node = ASTNode(ASTType::PIPE_OR);
                node.children.push_back(node_stack.top());
                // Only valid chars after this is either literal or "("
                if (regex[i+1] != '(' && get_type(regex[i+1]) != ASTType::LITERAL) throw std::runtime_error("Invalid Regex");
                else {
                    if (regex[i+1] == '(') {
                        int old_i = i;
                        for (int j = i+1; j < regex.length(); j++) {
                            if (regex[j] == ')') {
                                i = j;
                                break;
                            }
                            group += regex[j];
                        }
                        if (old_i == i) throw std::runtime_error("Invalid Regex");
                        group_node = ASTNode(ASTType::GROUP);
                        group_ast = parse(group);
                        for (const auto & j : group_ast) {
                            group_node.children.push_back(j);
                        }
                        node.children.push_back(group_node);
                    }
                }
                node_stack.pop();
                node_stack.emplace(node);
                break;
            case ASTType::GROUP_OPEN:
                for (int j = i+1; j < regex.length(); j++) {
                    if (regex[j] == ')') {
                        i = j;
                        break;
                    }
                    group += regex[j];
                }
                group_ast = parse(group);
                group_node = ASTNode(ASTType::GROUP);
                for (const auto & j : group_ast) {
                    group_node.children.push_back(j);
                }
                node_stack.emplace(group_node);
                break;
            case ASTType::LITERAL:
                if (i != 0 && node_stack.top().type == ASTType::PIPE_OR && node_stack.top().children.size() == 1) {
                    node_stack.top().children.emplace_back(c);
                } else {
                    node_stack.emplace(c);
                }
                break;
            default:
                break;
        }
    }
    while (!node_stack.empty()) {
        ast.push_back(node_stack.top());
        node_stack.pop();
    }
    return ast;
}
