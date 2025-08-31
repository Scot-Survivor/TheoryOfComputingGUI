//
// Created by scot on 03/11/23.
//

#ifndef REGEXTOOL_AST_H
#define REGEXTOOL_AST_H
#define _EPSILON "ɛ"
#include <vector>
#include <string>
#include <stack>
#include <stdexcept>
#include <unordered_map>


enum class ASTType {
    KLEENE_STAR, // *
    PIPE_OR, // |
    GROUP, // ()
    GROUP_OPEN, // (
    GROUP_CLOSE, // )
    LITERAL, // a-z, A-Z, 0-9
    EMPTY // ɛ
};

const static std::unordered_map<ASTType, int> ASTPrecedence = {
        {ASTType::KLEENE_STAR, 3},
        {ASTType::PIPE_OR, 1},
        {ASTType::GROUP, 2},
        {ASTType::LITERAL, 4},
        {ASTType::EMPTY, 5}
};

struct ASTNode {
    ASTType type = ASTType::EMPTY;
    char literal = '\0';
    std::vector<ASTNode> children = std::vector<ASTNode>();
    ASTNode() = default;
    explicit ASTNode(ASTType type) : type(type) {};
    explicit ASTNode(char literal) : type(ASTType::LITERAL), literal(literal) {};
};

namespace AST {
    ASTType get_type(char c);
     std::vector<ASTNode> parse(std::string regex);
};


#endif //REGEXTOOL_AST_H
