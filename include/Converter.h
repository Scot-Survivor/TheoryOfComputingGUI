//
// Created by scot on 02/11/23.
//

#ifndef REGEXTOOL_CONVERTER_H
#define REGEXTOOL_CONVERTER_H
#include "Graph.h"
#include <cstring>
#include <stack>
#include <random>

/**
 * This convertor class will convert Regex to NFA, NFA to DFA, and DFA to Regex.
 */
class Converter {
public:
    static Graph convert_regex_to_nfa(std::string regex);
    static Graph convert_nfa_to_dfa(Graph nfa);
    static std::string convert_dfa_to_regex(Graph dfa);

};


#endif //REGEXTOOL_CONVERTER_H
