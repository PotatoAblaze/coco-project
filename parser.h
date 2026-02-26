#ifndef PARSER_H
#define PARSER_H
#include "parserDef.h"
#include <stdio.h>

TerminalArray init_terminal_array();
void push_terminal_array(TerminalArray *array, TokenType term);
void concatenate_terminal_arrays(TerminalArray *arr1, TerminalArray *arr2);

void populate_first_and_follow(FirstAndFollowEntry entries[], int grammar_count, const GrammarRule rules[]);
void compute_parse_table(ParseTable pt, int var_count, FirstAndFollowEntry entries[], int grammar_count, const GrammarRule rules[]);
ParseTreeNode* generate_parse_tree(int token_count, Token** tokens, int* token_index, ParseTable pt, const GrammarRule rules[], Term curr_term);
void test_first_computation();
void print_parse_tree_inorder(ParseTreeNode* node, int depth);
void print_parse_tree_to_file(ParseTreeNode* node, int depth, FILE* file);

#endif