//
// Created by scot on 03/11/23.
//

#ifndef REGEXTOOL_AST_H
#define REGEXTOOL_AST_H
#include <vector>
#include <string>
#include <stack>
#include <stdexcept>


enum class ASTType {
    KLEENE_STAR,
    PIPE_OR,
    GROUP,
    GROUP_OPEN,
    GROUP_CLOSE,
    LITERAL,
    EMPTY
};

struct ASTNode {
    ASTType type = ASTType::EMPTY;
    char literal = '\0';
    std::vector<ASTNode> children = std::vector<ASTNode>();
    ASTNode() = default;
    explicit ASTNode(ASTType type) : type(type) {};
    explicit ASTNode(char literal) : type(ASTType::LITERAL), literal(literal) {};
};

class AST {
public:
    static ASTType get_type(char c);
    static std::vector<ASTNode> parse(std::string regex);
};


#endif //REGEXTOOL_AST_H
