//
// Created by scot on 02/11/23.
//

#include "Converter.h"

#include <stack>
#include <utility>

/**
 *  Handle special characters, * and | with shunting yard algorithm
 * @param c the character to handle
 * @param operators the operator stack
 * @param postfix the postfix string to append to
 */
void handle_specials(char c, std::stack<char>* operators, std::string postfix)
{
    while (!operators->empty() && ASTPrecedence.find(AST::get_type(operators->top())) != ASTPrecedence.end()
        && ASTPrecedence.at(AST::get_type(operators->top())) >= ASTPrecedence.at(AST::get_type(c)))
    {
        postfix += operators->top();
        operators->pop();
    }
    operators->push(c);
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