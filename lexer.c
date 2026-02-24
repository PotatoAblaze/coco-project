#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"

bool alpha(char c)
{
    return isalpha(c);
}

bool digit(char c)
{
    return isdigit(c);
}

bool lower(char c)
{
    return islower(c);
}

bool id_letter(char c)
{
    return (c >= 'b' && c <= 'd');
}

bool id_num(char c)
{
    return (c >= '2' && c <= '7');
}

void print_syntax_error(int line_number)
{
    printf("error: syntax error at line %d", line_number);
    exit(0);
}

TokenType check_keyword(char *lexeme)
{
    int len = sizeof(keyword_table) / sizeof(KeywordMap);
    for (int i = 0; i < len; i++)
    {
        if (strcmp(lexeme, keyword_table[i].keyword) == 0)
        {
            return keyword_table[i].type;
        }
    }
    return TK_FIELDID;
}

void push_token(int *token_count, Token **tokens, char *input_buffer, int token_start, int token_end, TokenType type, int line_number)
{
    int len = token_end - token_start + 1;
    char *token_string = (char *)malloc(sizeof(char) * (len + 1));
    token_string[len] = '\0';
    for (int x = token_start; x <= token_end; x++)
    {
        token_string[x - token_start] = input_buffer[x];
    }
    Token *t = (Token *)malloc(sizeof(Token));
    t->type = type;
    t->line_number = line_number;
    t->token = token_string;
    tokens[*token_count] = t;
    (*token_count)++;
}

Token **get_tokens(char *file_name, int *size_var)
{
    char *input_buffer = (char *)malloc(sizeof(char) * (INPUT_BUFFER_SIZE));
    input_buffer[0] = '\0';

    FILE *source_file = fopen(file_name, "r");
    char read_buffer[64];
    while (fgets(read_buffer, 64, source_file))
    {
        strcat(input_buffer, read_buffer);
    }

    int current_state = 0;

    int len = strlen(input_buffer);
    input_buffer[len] = ' ';
    len++;
    input_buffer[len] = '\0';

    int token_start = -1;
    int ind = 0;
    int tokens_count = 0;
    Token **tokens = (Token **)malloc(sizeof(Token *) * TOKEN_ARRAY_SIZE);
    int line_number = 1;

    while (ind < len)
    {
        char c = input_buffer[ind];

        switch (current_state)
        {
        case 0:
            token_start = ind;
            if (lower(c))
            {
                if (c == 'b' || c == 'c' || c == 'd')
                    current_state = 2;
                else
                    current_state = 1;
            }
            else if (digit(c))
                current_state = 8;
            else
            {
                switch (c)
                {
                case '=':
                    current_state = 30;
                    break;
                case '!':
                    current_state = 28;
                    break;
                case '_':
                    current_state = 38;
                    break;
                case '#':
                    current_state = 42;
                    break;
                case '%':
                    current_state = 45;
                    break;
                case '[':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_SQL, line_number);
                    break;
                case ']':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_SQR, line_number);
                    break;
                case ',':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_COMMA, line_number);
                    break;
                case ';':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_SEM, line_number);
                    break;
                case '.':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_DOT, line_number);
                    break;
                case ':':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_COLON, line_number);
                    break;
                case '/':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_DIV, line_number);
                    break;
                case '+':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_PLUS, line_number);
                    break;
                case '-':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_MINUS, line_number);
                    break;
                case '*':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_MUL, line_number);
                    break;
                case '~':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_NOT, line_number);
                    break;
                case '(':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_OP, line_number);
                    break;
                case ')':
                    push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_CL, line_number);
                    break;
                case '@':
                    current_state = 35;
                    break;
                case '&':
                    current_state = 32;
                    break;
                case '<':
                    current_state = 19;
                    break;
                case '>':
                    current_state = 25;
                    break;
                }
            }
            break;
        case 1:
            if (lower(c))
                break;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_FIELDID, line_number);
                TokenType possible = check_keyword(tokens[tokens_count - 1]->token);
                tokens[tokens_count - 1]->type = possible;
            }
            break;
        case 2:
            if (lower(c))
                current_state = 1;
            else if (id_num(c))
                current_state = 4;
            else
                print_syntax_error(line_number);

            break;

        case 4:
            if (id_num(c))
                current_state = 6;
            else if (id_letter(c))
                current_state = 5;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_ID, line_number);
            }
            break;

        case 5:
            if (id_num(c))
                current_state = 6;
            else if (id_letter(c))
                break;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_ID, line_number);
            }
            break;

        case 6:
            if (id_num(c))
                break;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_ID, line_number);
            }
            break;

        case 8:
            if (digit(c))
                break;
            else if (c == '.')
                current_state = 10;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_NUM, line_number);
            }
            break;

        case 10:
            if (digit(c))
                current_state = 11;
            else
                print_syntax_error(line_number);
            break;

        case 11:
            if (digit(c))
                current_state = 12;
            else
                print_syntax_error(line_number);
            break;

        case 12:
            if (c == 'E')
                current_state = 13;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_RNUM, line_number);
            }
            break;

        case 13:
            if (c == '+' || c == '-')
                current_state = 14;
            else if (digit(c))
                current_state = 15;
            else
                print_syntax_error(line_number);
            break;

        case 14:
            if (digit(c))
                current_state = 15;
            else
                print_syntax_error(line_number);
            break;

        case 15:
            if (digit(c))
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_RNUM, line_number);
            }
            else
                print_syntax_error(line_number);
            break;

        case 19:
            if (c == '=')
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_LE, line_number);
            }
            else if (c == '-')
                current_state = 21;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_LT, line_number);
            }
            break;

        case 21:
            if (c == '-')
                current_state = 23;
            else
                print_syntax_error(line_number);
            break;

        case 23:
            if (c == '-')
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_ASSIGNOP, line_number);
            }
            else
                print_syntax_error(line_number);

            break;

        case 25:
            if (c == '=')
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_GE, line_number);
            }
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_GT, line_number);
            }

            break;

        case 28:
            if (c == '=')
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_NE, line_number);
            }
            else
                print_syntax_error(line_number);
            break;

        case 30:
            if (c == '=')
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_EQ, line_number);
            }
            else
                print_syntax_error(line_number);
            break;

        case 32:
            if (c == '&')
                current_state = 33;
            else
                print_syntax_error(line_number);
            break;

        case 33:
            if (c == '&')
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_AND, line_number);
            }
            else
                print_syntax_error(line_number);
            break;

        case 35:
            if (c == '@')
                current_state = 36;
            else
                print_syntax_error(line_number);
            break;

        case 36:
            if (c == '@')
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_OR, line_number);
            }
            else
                print_syntax_error(line_number);
            break;

        case 38:
            if (alpha(c))
                current_state = 39;
            else
                print_syntax_error(line_number);
            break;

        case 39:
            if (alpha(c))
                break;
            else if (digit(c))
                current_state = 40;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_FUNID, line_number);
                TokenType possible = check_keyword(tokens[tokens_count - 1]->token);
                tokens[tokens_count - 1]->type = possible;
            }
            break;

        case 40:
            if (digit(c))
                break;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_FUNID, line_number);
            }
            break;

        case 42:
            if (lower(c))
                current_state = 43;
            else
                print_syntax_error(line_number);
            break;

        case 43:
            if (lower(c))
                break;
            else
            {
                ind--;
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_RUID, line_number);
            }
            break;

        case 45:
            if (c == '\n')
            {
                current_state = 0;
                push_token(&tokens_count, tokens, input_buffer, token_start, ind, TK_COMMENT, line_number);
            }
            else
                break;
            break;
        }
        if (c == '\n')
            line_number++;
        ind++;
    }

    Token *t = (Token *)malloc(sizeof(Token));
    t->type = TK_EOF;
    t->line_number = line_number;
    t->token = "";
    tokens[tokens_count] = t;
    tokens_count++;

    (*size_var) = tokens_count;

    for (int x = 0; x < tokens_count; x++)
    {
        printf("%s | ", get_token_name(tokens[x]->type));
    }
    printf("\n\n Lexing Complete!\n\n");

    return tokens;
}