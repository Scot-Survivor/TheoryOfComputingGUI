//
// Created by scot on 02/11/23.
//

#ifndef REGEXTOOL_CONVERTER_H
#define REGEXTOOL_CONVERTER_H
#include "Graph.h"
#include "AST.h"
#include <cstring>
#include <stack>
#include <random>

/**
 * This convertor class will convert Regex to NFA, NFA to DFA, and DFA to Regex.
 */
namespace Converter {
    std::string shunting_yard(const std::string& regex);
    Graph convert_regex_to_nfa(const std::string& regex);
    void calculate_labels(Graph& graph, const std::string& regex);

};


#endif //REGEXTOOL_CONVERTER_H
