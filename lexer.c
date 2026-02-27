#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#define B_SIZE 512
#define TOTAL_B_SIZE 1024

static const char* source_string = NULL;
static int source_length = 0;
static int source_offset = 0;

static char input_buffer[TOTAL_B_SIZE];
static int ind = 0;
static int line_number = 1;
static bool is_eof = false;

bool alpha(char c) { return isalpha(c); }
bool digit(char c) { return isdigit(c); }
bool lower(char c) { return islower(c); }
bool id_letter(char c) { return (c >= 'b' && c <= 'd'); }
bool id_num(char c) { return (c >= '2' && c <= '7'); }

void print_unknown_pattern(int token_start, int current_ind, int line, char* buffer) {
	int end_idx = current_ind - 1;
	if (end_idx < 0) end_idx = TOTAL_B_SIZE - 1;

	int len;
	if (end_idx >= token_start) len = end_idx - token_start + 1;
	else len = TOTAL_B_SIZE - token_start + end_idx + 1;

	char* err_str = (char*)malloc(len + 1);
	int curr = token_start;
	for (int x = 0; x < len; x++) {
		err_str[x] = buffer[curr];
		curr++;
		if (curr == TOTAL_B_SIZE) curr = 0;
	}
	err_str[len] = '\0';
	printf("Line %d Error: Unknown pattern <%s>\n", line, err_str);
	free(err_str);
}

void remove_comments(char input[], char output[]) {
	bool comment_flag = false;
	char c;
	int i = 0; 
	for(int x = 0; input[x] != '\0'; x++) {
		char c = input[x]; 
		if (c == '%') comment_flag = true;
		else if (c == '\n') comment_flag = false;
		if (!comment_flag) { 
			output[i++] = c; 
		}
	}
    output[i] = '\0'; 
}

TokenType check_keyword(char* lexeme) {
	int len = sizeof(keyword_table) / sizeof(KeywordMap);
	for (int i = 0; i < len; i++) {
		if (strcmp(lexeme, keyword_table[i].keyword) == 0) {
			return keyword_table[i].type;
		}
	}
	return TK_FIELDID;
}

TokenArray init_token_array() {
	TokenArray ta = { 0, 0, NULL };
	return ta;
}

void arr_push_token(TokenArray* ta, Token token) {
	if (ta->size == ta->capacity) {
		if (ta->capacity == 0) ta->capacity = 2;
		else ta->capacity = ta->capacity * 2;
		Token* new_arr = (Token*)malloc(sizeof(Token) * ta->capacity);
		for (int x = 0; x < ta->size; x++) new_arr[x] = ta->arr[x];
		if (ta->arr != NULL) free(ta->arr);
		ta->arr = new_arr;
	}
	ta->arr[ta->size] = token;
	ta->size++;
}

Token create_token(int token_start, int token_end, TokenType type, int current_line) {
	int len;
	if (token_end >= token_start) len = token_end - token_start + 1;
	else len = TOTAL_B_SIZE - token_start + token_end + 1;

	char* token_string = (char*)malloc(sizeof(char) * (len + 1));
	token_string[len] = '\0';

	int curr = token_start;
	for (int x = 0; x < len; x++) {
		token_string[x] = input_buffer[curr];
		curr++;
		if (curr == TOTAL_B_SIZE) curr = 0;
	}

	if (type == TK_ID && len > 20) {
		printf("Line %d \tError: Variable Identifier is longer than the prescribed length of 20 characters.\n", current_line);
	}

	Token token = { .type = type, .line_number = current_line, .token = token_string };
	return token;
}

void init_lexer(const char* input) {
	source_string = input;
    source_length = strlen(input);
    source_offset = 0;

    int bytes_to_read = (source_length - source_offset < B_SIZE) ? (source_length - source_offset) : B_SIZE;
    memcpy(input_buffer, source_string + source_offset, bytes_to_read);
    source_offset += bytes_to_read;

	if (bytes_to_read < B_SIZE) {
        input_buffer[bytes_to_read] = '\0';
    }

	ind = 0;
	line_number = 1;
	is_eof = false;
}

Token getNextToken() {
	int current_state = 0;
	int token_start = ind;
	Token generated_token;
	bool token_ready = false;

	while (!is_eof && !token_ready) {
		char c = input_buffer[ind];
		if (c == '\0') {
			c = ' ';
			is_eof = true;
		}

		switch (current_state) {
			case 0:
				token_start = ind;
				if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
					if (c == '\n') line_number++;
					current_state = 0;
				}
				else if (lower(c)) {
					if (c == 'b' || c == 'c' || c == 'd') current_state = 2;
					else current_state = 1;
				}
				else if (digit(c)) current_state = 8;
				else {
					switch (c) {
						case '=': current_state = 30; break;
						case '!': current_state = 28; break;
						case '_': current_state = 38; break;
						case '#': current_state = 42; break;
						case '%': current_state = 45; break;
						case '[': generated_token = create_token(token_start, ind, TK_SQL, line_number); token_ready = true; break;
						case ']': generated_token = create_token(token_start, ind, TK_SQR, line_number); token_ready = true; break;
						case ',': generated_token = create_token(token_start, ind, TK_COMMA, line_number); token_ready = true; break;
						case ';': generated_token = create_token(token_start, ind, TK_SEM, line_number); token_ready = true; break;
						case '.': generated_token = create_token(token_start, ind, TK_DOT, line_number); token_ready = true; break;
						case ':': generated_token = create_token(token_start, ind, TK_COLON, line_number); token_ready = true; break;
						case '/': generated_token = create_token(token_start, ind, TK_DIV, line_number); token_ready = true; break;
						case '+': generated_token = create_token(token_start, ind, TK_PLUS, line_number); token_ready = true; break;
						case '-': generated_token = create_token(token_start, ind, TK_MINUS, line_number); token_ready = true; break;
						case '*': generated_token = create_token(token_start, ind, TK_MUL, line_number); token_ready = true; break;
						case '~': generated_token = create_token(token_start, ind, TK_NOT, line_number); token_ready = true; break;
						case '(': generated_token = create_token(token_start, ind, TK_OP, line_number); token_ready = true; break;
						case ')': generated_token = create_token(token_start, ind, TK_CL, line_number); token_ready = true; break;
						case '@': current_state = 35; break;
						case '&': current_state = 32; break;
						case '<': current_state = 19; break;
						case '>': current_state = 25; break;
						default:
							printf("Line %d Error: Unknown Symbol <%c>\n", line_number, c);
							current_state = 0;
							break;
					}
				}
				break;

			case 1:
				if (lower(c)) break;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_FIELDID, line_number);
					generated_token.type = check_keyword(generated_token.token);
					token_ready = true;
				}
				break;

			case 2:
				if (lower(c)) current_state = 1;
				else if (id_num(c)) current_state = 4;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 4:
				if (id_num(c)) current_state = 6;
				else if (id_letter(c)) current_state = 5;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_ID, line_number);
					token_ready = true;
				}
				break;

			case 5:
				if (id_num(c)) current_state = 6;
				else if (id_letter(c)) break;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_ID, line_number);
					token_ready = true;
				}
				break;

			case 6:
				if (id_num(c)) break;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_ID, line_number);
					token_ready = true;
				}
				break;

			case 8:
				if (digit(c)) break;
				else if (c == '.') current_state = 10;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_NUM, line_number);
					token_ready = true;
				}
				break;

			case 10:
				if (digit(c)) current_state = 11;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 11:
				if (digit(c)) current_state = 12;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 12:
				if (c == 'E') current_state = 13;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_RNUM, line_number);
					token_ready = true;
				}
				break;

			case 13:
				if (c == '+' || c == '-') current_state = 14;
				else if (digit(c)) current_state = 15;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 14:
				if (digit(c)) current_state = 15;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 15:
				if (digit(c)) break;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_RNUM, line_number);
					token_ready = true;
				}
				break;

			case 19:
				if (c == '=') {
					generated_token = create_token(token_start, ind, TK_LE, line_number);
					token_ready = true;
				}
				else if (c == '-') current_state = 21;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_LT, line_number);
					token_ready = true;
				}
				break;

			case 21:
				if (c == '-') current_state = 23;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 23:
				if (c == '-') {
					generated_token = create_token(token_start, ind, TK_ASSIGNOP, line_number);
					token_ready = true;
				}
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 25:
				if (c == '=') {
					generated_token = create_token(token_start, ind, TK_GE, line_number);
					token_ready = true;
				}
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_GT, line_number);
					token_ready = true;
				}
				break;

			case 28:
				if (c == '=') {
					generated_token = create_token(token_start, ind, TK_NE, line_number);
					token_ready = true;
				}
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 30:
				if (c == '=') {
					generated_token = create_token(token_start, ind, TK_EQ, line_number);
					token_ready = true;
				}
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 32:
				if (c == '&') current_state = 33;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 33:
				if (c == '&') {
					generated_token = create_token(token_start, ind, TK_AND, line_number);
					token_ready = true;
				}
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 35:
				if (c == '@') current_state = 36;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 36:
				if (c == '@') {
					generated_token = create_token(token_start, ind, TK_OR, line_number);
					token_ready = true;
				}
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 38:
				if (alpha(c)) current_state = 39;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 39:
				if (alpha(c)) break;
				else if (digit(c)) current_state = 40;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_FUNID, line_number);
					TokenType possible = check_keyword(generated_token.token);
					if (possible != TK_FIELDID) {
						generated_token.type = possible;
					}
					token_ready = true;
				}
				break;

			case 40:
				if (digit(c)) break;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_FUNID, line_number);
					token_ready = true;
				}
				break;

			case 42:
				if (lower(c)) current_state = 43;
				else {
					print_unknown_pattern(token_start, ind, line_number, input_buffer);
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					current_state = 0;
				}
				break;

			case 43:
				if (lower(c)) break;
				else {
					ind--; if (ind < 0) ind = TOTAL_B_SIZE - 1;
					generated_token = create_token(token_start, ind, TK_RUID, line_number);
					token_ready = true;
				}
				break;

			case 45:
				if (c == '\n') {
					generated_token = create_token(token_start, ind, TK_COMMENT, line_number);
					line_number++;
					token_ready = true;
				}
				else break;
				break;
		}

		if (!is_eof) {
			ind++;
			// MODIFIED: Replaced fread with string chunk copying
			if (ind == B_SIZE) {
                int r = (source_length - source_offset < B_SIZE) ? (source_length - source_offset) : B_SIZE;
                memcpy(input_buffer + B_SIZE, source_string + source_offset, r);
                source_offset += r;
				if (r < B_SIZE) input_buffer[B_SIZE + r] = '\0';
			}
			else if (ind == TOTAL_B_SIZE) {
                int r = (source_length - source_offset < B_SIZE) ? (source_length - source_offset) : B_SIZE;
                memcpy(input_buffer, source_string + source_offset, r);
                source_offset += r;
				if (r < B_SIZE) input_buffer[r] = '\0';
				ind = 0;
			}
		}
	}

	if (token_ready) return generated_token;

	Token t = { .type = TK_EOF, .line_number = line_number, .token = "" };
	return t;
}

TokenArray get_tokens(const char* input_text) {
	init_lexer(input_text);
	TokenArray tokens = init_token_array();

	while (true) {
		Token t = getNextToken();
		arr_push_token(&tokens, t);
		if (t.type == TK_EOF) break;
	}

	return tokens;
}