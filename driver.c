#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define FILE_INPUT_BUFFER_SIZE 10000000

int prompt() {
	printf("Select task:\n");
	printf("0. Exit\n");
	printf("1. Print comment-free code\n");
	printf("2. Print token list\n");
	printf("3. Print parse tree onto file\n");
	printf("4. Compute time taken by lexer and parser\n");
	printf("Your choice - ");
	int t;
	scanf("%d", &t);
	return t;
}

int main(int argc, char** argv) {

	if (argc != 3) {
		printf("Invalid input. Correct format: ./<executable>  <testcase .txt "
			"file>  <parseTree output .txt file>");
		exit(1);
	}

	int t;
	char* file_name = argv[1];

	while ((t = prompt())) {
		switch (t) {
			case 0: {
					exit(0);
					break;
				}

			case 1: {
					remove_comments(file_name);
					printf("\n");
					break;
				}

			case 2: {
					TokenArray ta = get_tokens(file_name);
					int last_line_no = -1;
					for (int x = 0; x < ta.size; x++) {
						if (last_line_no != ta.arr[x].line_number) {
							last_line_no = ta.arr[x].line_number;
							printf("\n%d\t", last_line_no);
						}
						printf("%s ", get_token_name(ta.arr[x].type));
					}
					printf("\n");
					break;
				}

			case 3: {
					TokenArray ta = get_tokens(file_name);
					Token** tokens = (Token**)malloc(sizeof(Token*) * ta.size);
					for (int i = 0; i < ta.size; i++) {
						tokens[i] = &ta.arr[i];
					}

					FirstAndFollowEntry entries[VAR_COUNT];
					populate_first_and_follow(entries, GRAMMAR_SIZE, grammar);

					ParseTable pt;
					compute_parse_table(pt, VAR_COUNT, entries, GRAMMAR_SIZE, grammar);

					int token_index = 0;
					ParseTreeNode* root = generate_parse_tree(
						ta.size,
						tokens,
						&token_index,
						pt,
						grammar,
						(Term) {
						.is_terminal = false, .var = VAR_PROGRAM
					}
					);

					FILE* out_file = fopen(argv[2], "w");
					if (out_file == NULL) {
						perror("opening output file failed");
						break;
					}
					print_parse_tree_to_file(root, 0, out_file);
					fclose(out_file);
					printf("Parse tree printed to %s\n", argv[2]);
					free(tokens);
					break;
				}

			case 4: {
					clock_t start_time, end_time;
					double total_CPU_time, total_CPU_time_in_seconds;

					start_time = clock();

					// Lexer
					TokenArray ta = get_tokens(file_name);
					Token** tokens = (Token**)malloc(sizeof(Token*) * ta.size);
					for (int i = 0; i < ta.size; i++) {
						tokens[i] = &ta.arr[i];
					}

					// Parser
					FirstAndFollowEntry entries[VAR_COUNT];
					populate_first_and_follow(entries, GRAMMAR_SIZE, grammar);

					ParseTable pt;
					compute_parse_table(pt, VAR_COUNT, entries, GRAMMAR_SIZE, grammar);

					int token_index = 0;
					ParseTreeNode* root = generate_parse_tree(
						ta.size,
						tokens,
						&token_index,
						pt,
						grammar,
						(Term) {
						.is_terminal = false, .var = VAR_PROGRAM
					}
					);

					end_time = clock();

					total_CPU_time = (double)(end_time - start_time);
					total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

					printf("Total CPU time: %f ticks\n", total_CPU_time);
					printf("Total CPU time in seconds: %f seconds\n", total_CPU_time_in_seconds);

					free(tokens);
					break;
				}
		}
	}
}
