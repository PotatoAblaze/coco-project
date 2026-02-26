#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    printf("Invalid input. Correct format: ./<executable>  <testcase .txt file>  <parseTree output .txt file>\n");
    exit(1);
  }

  int t;
  char* file_name = argv[1]; 

  while ((t = prompt())) {
    switch (t) {
    case 1:
      remove_comments(file_name);
      printf("\n");
      break;

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
    }
  }
  return 0;
}