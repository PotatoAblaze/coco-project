#ifndef LEXER_H
#define LEXER_H
#include <stdbool.h>
#include "lexerDef.h"


bool alpha(char c);
bool digit(char c);
bool lower(char c);
bool id_letter(char c);
bool id_num(char c);

void print_syntax_error(int line_number);
void remove_comments(char input[], char output[]);
TokenType check_keyword(char* lexeme);

TokenArray init_token_array();
void arr_push_token(TokenArray* ta, Token token);
void push_token(TokenArray* ta, char* input_buffer, int token_start,
                int token_end, TokenType type, int line_number);
TokenArray get_tokens(char* input_buffer);

#endif