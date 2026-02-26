#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char* read_file(char* file_name) {
  char* input_buffer = (char*)malloc(sizeof(char) * FILE_INPUT_BUFFER_SIZE);

  FILE* source_file = fopen(file_name, "r");
  if (source_file == NULL) {
    perror("opening testcase file failed");
    exit(1);
  }
  char read_buffer[128];
  while (fgets(read_buffer, 128, source_file)) {
    strcat(input_buffer, read_buffer);
  }

  return input_buffer;
}

int main(int argc, char** argv) {

  if (argc != 3) {
    printf("Invalid input. Correct format: ./<executable>  <testcase .txt "
           "file>  <parseTree output .txt file>");
    exit(1);
  }

  int t;
  char* input = read_file(argv[1]);

  while ((t = prompt())) {
    switch (t) {
      char* output;
    case 1:
      output = (char*)malloc(sizeof(char) * FILE_INPUT_BUFFER_SIZE);
      remove_comments(input, output);
      printf("%s\n", output);
      break;

    case 2:
      output = (char*)malloc(sizeof(char) * FILE_INPUT_BUFFER_SIZE);
      remove_comments(input, output);
      TokenArray ta = get_tokens(output);
      int last_line_no = -1;
      for (int x = 0; x < ta.size; x++) {
        if (last_line_no != ta.arr[x].line_number) {
          last_line_no = ta.arr[x].line_number;
          printf("\n%d\t", last_line_no);
        }
        printf("%s ", get_token_name(ta.arr[x].type));
      }
      break;
    }
  }
}
