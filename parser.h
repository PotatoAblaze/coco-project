#ifndef PARSER_H
#define PARSER_H
#include "parserDef.h"

TerminalArray init_terminal_array();
void push_terminal_array(TerminalArray *array, TokenType term);
void concatenate_terminal_arrays(TerminalArray *arr1, TerminalArray *arr2);

void populate_first_and_follow(FirstAndFollowEntry entries[], int grammar_count, const GrammarRule rules[]);
void test_first_computation();
void print_parse_tree_inorder(ParseTreeNode* node, int depth);

#endif