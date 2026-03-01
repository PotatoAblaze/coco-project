#include "parser.h"

#include <stdio.h>

#include "lexer.h"

TerminalArray init_terminal_array() { return (TerminalArray){0, 0, NULL}; }

bool set_contains(TerminalSet set, TokenType terminal) { return (set.bits >> terminal) & 1ull; }

bool set_insert(TerminalSet* set, TokenType terminal) {
    if(set_contains(*set, terminal)) return false;
    (set->bits) = (set->bits | (1ull << terminal));
    return true;
}

bool set_union(TerminalSet* set1, TerminalSet* set2) {
    if((set1->bits | set2->bits) == set1->bits) return false;
    set1->bits = set1->bits | set2->bits;
    return true;
}

void set_delete(TerminalSet* set, TokenType terminal) {
    unsigned long long full = ~(1ull << terminal);
    (set->bits) = (set->bits & full);
}

void term_stack_push(TermStack* st, ParseTreeNode* term) {
    if(st->capacity == 0) {
        st->capacity = 4;
        st->array = (ParseTreeNode**)malloc(sizeof(ParseTreeNode*) * 4);
    } else if(st->size == st->capacity) {
        st->capacity *= 2;
        ParseTreeNode** new_arr = (ParseTreeNode**)malloc(sizeof(ParseTreeNode*) * st->capacity);
        for(int x = 0; x < st->size; x++) {
            new_arr[x] = st->array[x];
        }
        free(st->array);
        st->array = new_arr;
    }

    st->array[st->size++] = term;
}

ParseTreeNode* term_stack_top(TermStack st) {
    if(st.size == 0) {
        printf("error: term_stack_top() called with empty stack\n");
        exit(1);
    }
    return st.array[st.size - 1];
}

void term_stack_pop(TermStack* st) {
    if(st->size == 0) {
        printf("error: term_stack_top() called with empty stack\n");
        exit(1);
    }
    st->size--;
}

void populate_first_and_follow(FirstAndFollowEntry entries[], int grammar_count, const GrammarRule rules[]) {
    for(int x = 0; x < VAR_COUNT; x++) {
        entries[x] = (FirstAndFollowEntry){
            .term = (Term){.is_terminal = false, .var = x},
            .first = (TerminalSet){.bits = 0LL},
            .follow = (TerminalSet){.bits = 0LL},
        };
    }

    // First computation
    bool changed = true;
    while(changed) {
        changed = false;

        for(int x = 0; x < grammar_count; x++) {
            Variable curr_var = rules[x].root;
            if(rules[x].expansion == NULL) {
                changed |= set_insert(&entries[curr_var].first, TK_EPSILON);
                continue;
            }
            bool is_finished = false;
            for(int y = 0; y < rules[x].expansion_length; y++) {
                if(rules[x].expansion[y].is_terminal) {
                    changed |= set_insert(&entries[curr_var].first, rules[x].expansion[y].terminal_type);
                    // if(changed) printf("changed 1\n");
                    is_finished = true;
                    break;
                } else {
                    TerminalSet targetFirstSet = entries[rules[x].expansion[y].var].first;
                    set_delete(&targetFirstSet, TK_EPSILON);
                    changed |= set_union(&entries[curr_var].first, &targetFirstSet);
                    if(!set_contains(entries[rules[x].expansion[y].var].first, TK_EPSILON)) {
                        is_finished = true;
                        break;
                    }
                }
            }
            if(!is_finished) {
                changed |= set_insert(&entries[curr_var].first, TK_EPSILON);
            }
        }
    }

    // Follow computation
    set_insert(&entries[VAR_PROGRAM].follow, TK_EOF);
    changed = true;
    while(changed) {
        changed = false;

        for(int x = 0; x < grammar_count; x++) {
            if(rules[x].expansion == NULL) continue;
            for(int y = 0; y < rules[x].expansion_length; y++) {
                if(rules[x].expansion[y].is_terminal) continue;
                Variable curr_var = rules[x].expansion[y].var;
                bool is_finished = false;
                for(int z = y + 1; z < rules[x].expansion_length; z++) {
                    if(rules[x].expansion[z].is_terminal) {
                        is_finished = true;
                        changed |= set_insert(&entries[curr_var].follow, rules[x].expansion[z].terminal_type);
                        break;
                    } else {
                        TerminalSet targetFirstSet = entries[rules[x].expansion[z].var].first;
                        set_delete(&targetFirstSet, TK_EPSILON);
                        changed |= set_union(&entries[curr_var].follow, &targetFirstSet);
                        if(!set_contains(entries[rules[x].expansion[z].var].first, TK_EPSILON)) {
                            is_finished = true;
                            break;
                        }
                    }
                }
                if(!is_finished) {
                    changed |= set_union(&entries[curr_var].follow, &entries[rules[x].root].follow);
                }
            }
        }
    }
}

void compute_parse_table(ParseTable pt, int var_count, FirstAndFollowEntry entries[], int grammar_count, const GrammarRule rules[]) {
    for(int x = 0; x < 128; x++) {
        for(int y = 0; y < 128; y++) {
            pt[x][y] = -1;
        }
    }
    for(int x = 0; x < grammar_count; x++) {
        TerminalSet alpha_first = {.bits = 0ull};
        bool is_finished = false;
        if(rules[x].expansion == NULL) set_insert(&alpha_first, TK_EPSILON);
        for(int y = 0; y < rules[x].expansion_length; y++) {
            if(rules[x].expansion[y].is_terminal) {
                is_finished = true;
                set_insert(&alpha_first, rules[x].expansion[y].terminal_type);
                break;
            } else {
                TerminalSet targetFirstSet = entries[rules[x].expansion[y].var].first;
                set_delete(&targetFirstSet, TK_EPSILON);
                set_union(&alpha_first, &targetFirstSet);
                if(!set_contains(entries[rules[x].expansion[y].var].first, TK_EPSILON)) {
                    is_finished = true;
                    break;
                }
            }
        }
        if(!is_finished) {
            set_insert(&alpha_first, TK_EPSILON);
        }

        if(set_contains(alpha_first, TK_EPSILON)) {
            TerminalSet root_follow = entries[rules[x].root].follow;
            for(int y = 0; y < TK_COUNT; y++) {
                if(set_contains(root_follow, y)) {
                    if(pt[rules[x].root][y] != -1) {
                        printf("error: not LL(1) grammar. reason: var - %s , terminal - %s\n", get_variable_name(rules[x].root), get_token_name(y));
                        exit(1);
                    }
                    pt[rules[x].root][y] = x;
                }
            }
        }

        for(int y = 0; y < TK_COUNT; y++) {
            if(set_contains(alpha_first, y)) {
                if(y == TK_EPSILON) continue;
                if(pt[rules[x].root][y] != -1) {
                    printf("error: not LL(1) grammar. reason: var - %s , terminal - %s\n", get_variable_name(rules[x].root), get_token_name(y));
                    exit(1);
                }
                pt[rules[x].root][y] = x;
            }
        }
    }
}

ParseTreeNode* create_new_tree_node(Token* token, Term term) {
    ParseTreeNode* new_node = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    new_node->token = token;
    new_node->term = term;
    new_node->child_count = 0;
    return new_node;
}

const char* get_term_name(Term term) {
    if(term.is_terminal) {
        return get_token_name(term.terminal_type);
    } else {
        return get_variable_name(term.var);
    }
}

ParseTreeNode* generate_parse_tree(TokenArray tokens, ParseTable pt, int rules_count, const GrammarRule rules[]) {
    int token_count = tokens.size;
    int token_index = 0;
    TermStack st = {0, 0, NULL};

    // term_stack_push(&st, create_new_tree_node(NULL, (Term){.is_terminal = true, .terminal_type = TK_EOF}));
    if(token_count == 0) return term_stack_top(st);
    ParseTreeNode* root = create_new_tree_node(&tokens.arr[token_index], (Term){.is_terminal = false, .var = VAR_PROGRAM});
    term_stack_push(&st, root);

    while(st.size > 0 && token_index < token_count) {
        ParseTreeNode* top = term_stack_top(st);
        term_stack_pop(&st);
        Term top_term = top->term;

        if(top_term.is_terminal) {
            if(tokens.arr[token_index].type == top_term.terminal_type) {
                top->token = &tokens.arr[token_index];
                token_index++;
                continue;
            } else {
                printf("Line %d Error: The token %s for lexeme %s does not match with the expected token %s\n", tokens.arr[token_index].line_number,
                       get_token_name(tokens.arr[token_index].type), tokens.arr[token_index].token, get_token_name(top_term.terminal_type));
                continue;
            }
        } else {
            // Search all rules
            int rule_index = pt[top_term.var][tokens.arr[token_index].type];
            bool error_found = false;
            while(token_index < token_count && rule_index == -1) {
                if(!error_found) {
                    printf("Line %d Error: Invalid token %s encountered with value %s ; stack top %s\n", tokens.arr[token_index].line_number,
                           get_token_name(tokens.arr[token_index].type), tokens.arr[token_index].token, get_variable_name(top_term.var));
                    error_found = true;
                }
                token_index++;
                if(token_index < token_count) {
                    rule_index = pt[top_term.var][tokens.arr[token_index].type];
                }
            }
            if(rule_index == -2) {
                continue;
            }
            if(token_index >= token_count) continue;

            if(rules[rule_index].expansion == NULL) {
                top->child_count = 0;
                continue;
            }
            top->child_count = rules[rule_index].expansion_length;
            top->token = &tokens.arr[token_index];
            for(int x = rules[rule_index].expansion_length - 1; x >= 0; x--) {
                ParseTreeNode* child = create_new_tree_node(NULL, rules[rule_index].expansion[x]);
                term_stack_push(&st, child);
                top->children[x] = child;
            }
        }
    }
    if(st.size > 0 && token_index >= token_count) {
        printf("Error: Unexpected end of file. Parsing incomplete.\n");
    }
    return root;
}

void print_parse_tree_to_file(ParseTreeNode* node, ParseTreeNode* parent, int depth, FILE* file) {
    // lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
    if(node == NULL) {
        return;
    }
    if(node->token == NULL) {
        return;
    }

    const char* parent_name;
    if(parent == NULL) {
        parent_name = "ROOT";
    } else {
        parent_name = get_variable_name(parent->term.var);
    }

    if(parent == NULL && depth == 0) {
        fprintf(file, "%-24s %-8s %-24s %-16s %-24s %-8s %-24s\n", "lexemeCurrentNode", "lineno", "tokenName", "valueIfNumber", "parentNodeSymbol",
                "isLeaf", "NodeSymbol");
        fprintf(file, "%-24s %-8s %-24s %-16s %-24s %-8s %-24s\n", "------------------------", "--------", "------------------------",
                "----------------", "------------------------", "--------", "------------------------");
    }

    // Print current node
    if(node->term.is_terminal) {
        const char* number_str = (node->token->type == TK_RNUM || node->token->type == TK_NUM) ? node->token->token : "~~~~";
        fprintf(file, "%-24s %-8d %-24s %-16s %-24s %-8s %-24s\n", node->token->token, node->token->line_number,
                get_token_name(node->term.terminal_type), number_str, parent_name, "yes", get_token_name(node->term.terminal_type));
    } else {
        if(node->child_count > 0) print_parse_tree_to_file(node->children[0], node, depth + 1, file);
        fprintf(file, "%-24s %-8d %-24s %-16s %-24s %-8s %-24s\n", "~~~~", node->token->line_number, "~~~~", "~~~~", parent_name, "no",
                get_variable_name(node->term.var));
        for(int i = 1; i < node->child_count; i++) {
            print_parse_tree_to_file(node->children[i], node, depth + 1, file);
        }
    }
}

void print_parse_tree_inorder(ParseTreeNode* node, int depth) {
    if(node == NULL) {
        return;
    }

    // Print indentation
    for(int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Print current node
    if(node->term.is_terminal) {
        printf("Terminal: %s", get_token_name(node->term.terminal_type));
        if(node->token != NULL) {
            printf(" (lexeme: \"%s\", line: %d)", node->token->token, node->token->line_number);
        }
        printf("\n");
    } else {
        printf("Non-Terminal: %s\n", get_variable_name(node->term.var));

        // Recursively print children
        for(int i = 0; i < node->child_count; i++) {
            print_parse_tree_inorder(node->children[i], depth + 1);
        }
    }
}

// compute synchronization tokens here
void compute_synchronization_tokens(ParseTable pt, FirstAndFollowEntry entries[]) {
    for(int x = 0; x < VAR_COUNT; x++) {
        TerminalSet follow_set = entries[x].follow;
        for(int y = 0; y < TK_COUNT; y++) {
            if(set_contains(follow_set, y)) {
                // In panic mode recovery, if the table entry is empty,
                // we place a synchronization token (represented by -2)
                // based on the FOLLOW set of the variable.
                if(pt[x][y] == -1) {
                    pt[x][y] = -2;
                }
            }
        }
    }
}

// panic mode error recovery procedure
void panic_mode_recovery(int token_count, TokenArray tokens, int* token_index, ParseTable pt, Term curr_term) {
    if(*token_index >= token_count) {
        return;
    }
    // If the expected symbol is a terminal, just report the error and move forward
    if(curr_term.is_terminal) {
        printf("Line %d Error: The token %s for lexeme %s does not match with the expected token %s\n", tokens.arr[*token_index].line_number,
               get_token_name(tokens.arr[*token_index].type), tokens.arr[*token_index].token, get_token_name(curr_term.terminal_type));
        // Do NOT consume the current token - assume the expected terminal was missing
        return;
    }

    // Non-terminal case: scan forward until we find a token in FIRST or FOLLOW (sync token)
    printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n", tokens.arr[*token_index].line_number,
           get_token_name(tokens.arr[*token_index].type), tokens.arr[*token_index].token, get_variable_name(curr_term.var));

    while(*token_index < token_count) {
        TokenType curr_token_type = tokens.arr[*token_index].type;

        if(curr_token_type == TK_EOF) {
            break;
        }

        // Skip comments during recovery
        if(curr_token_type == TK_COMMENT) {
            (*token_index)++;
            continue;
        }

        int table_entry = pt[curr_term.var][curr_token_type];

        // If we find a valid rule (>= 0) or a sync token (-2), stop discarding
        if(table_entry != -1) {
            break;
        }

        (*token_index)++;
    }
}

void test_first_computation() {
    printf("\n=== TESTING FIRST TABLE COMPUTATION ===\n\n");

    // Initialize first and follow table
    FirstAndFollowEntry entries[VAR_COUNT];

    // Populate the first and follow sets
    populate_first_and_follow(entries, GRAMMAR_SIZE, grammar);

    // Print FIRST sets for all variables
    printf("FIRST SETS:\n");
    printf("%-30s | FIRST Set\n", "Non-Terminal");
    printf("------------------------------------------------------------\n");

    for(int i = 0; i < VAR_COUNT; i++) {
        printf("%-30s | { ", get_variable_name(i));

        if(entries[i].first.bits == 0) {
            printf("}");
        } else {
            int last_index;
            for(int j = TK_COUNT - 1; j >= 0; j--) {
                if((entries[i].first.bits >> j) & 1) {
                    last_index = j;
                    break;
                }
            }
            for(int j = 0; j < TK_COUNT; j++) {
                if(!((entries[i].first.bits >> j) & 1)) continue;
                printf("%s", get_token_name(j));
                if(j != last_index) printf(", ");
            }
            printf(" }");
        }
        printf("\n");
    }

    printf("FOLLOW SETS:\n");
    printf("%-30s | FOLLOW Set\n", "Non-Terminal");
    printf("------------------------------------------------------------\n");

    for(int i = 0; i < VAR_COUNT; i++) {
        printf("%-30s | { ", get_variable_name(i));

        if(entries[i].follow.bits == 0) {
            printf("}");
        } else {
            int last_index;
            for(int j = TK_COUNT - 1; j >= 0; j--) {
                if((entries[i].follow.bits >> j) & 1) {
                    last_index = j;
                    break;
                }
            }
            for(int j = 0; j < TK_COUNT; j++) {
                if(!((entries[i].follow.bits >> j) & 1)) continue;
                printf("%s", get_token_name(j));
                if(j != last_index) printf(", ");
            }
            printf(" }");
        }
        printf("\n");
    }

    printf("\n=== TEST COMPLETE ===\n");
}

// int main()
// {
//     test_first_computation();

//     printf("\n=== TESTING PARSE TABLE GENERATION ===\n\n");

//     // Initialize first and follow table
//     FirstAndFollowEntry entries[VAR_COUNT];

//     // Populate the first and follow sets
//     populate_first_and_follow(entries, GRAMMAR_SIZE, grammar);

//     // Compute parse table
//     ParseTable pt;
//     compute_parse_table(pt, VAR_COUNT, entries, GRAMMAR_SIZE, grammar);

//     // printf("PARSE TABLE:\n");
//     // printf("%-30s |", "Non-Terminal \\ Terminal");

//     // // Print header with terminal names
//     // for (int j = 0; j < TK_COUNT; j++)
//     // {
//     //     if (j == TK_EPSILON)
//     //         continue; // Skip epsilon in table header
//     //     printf(" %-12s", get_token_name(j));
//     // }
//     // printf("\n");

//     // // Print separator line
//     // for (int i = 0; i < 31; i++)
//     //     printf("-");
//     // printf("+");
//     // for (int j = 0; j < TK_COUNT; j++)
//     // {
//     //     if (j == TK_EPSILON)
//     //         continue;
//     //     printf("---------");
//     // }
//     // printf("\n");

//     // // Print table rows
//     // for (int i = 0; i < VAR_COUNT; i++)
//     // {
//     //     printf("%-30s |", get_variable_name(i));
//     //     for (int j = 0; j < TK_COUNT; j++)
//     //     {
//     //         if (j == TK_EPSILON)
//     //             continue;
//     //         if (pt[i][j] == -1)
//     //         {
//     //             printf(" %-12s", "-");
//     //         }
//     //         else
//     //         {
//     //             printf(" %-12d", pt[i][j]);
//     //         }
//     //     }
//     //     printf("\n");
//     // }

//     // printf("\n=== PARSE TABLE TEST COMPLETE ===\n");

//     printf("\n=== TESTING PARSE TREE GENERATION ===\n\n");

//     // Simple test: Create a minimal token stream to test parse tree
//     // For a complete test, you would need to read from a file
//     // Here's a skeleton showing how to use the parse tree functions:
//     int token_count;
//     Token** test_tokens = get_tokens("sample.moo", &token_count) ; // Array of token pointers
//     int token_index = 0;

//     ParseTreeNode* root = generate_parse_tree(
//         token_count,
//         test_tokens,
//         &token_index,
//         pt,
//         grammar,
//         (Term){.is_terminal = false, .var = VAR_PROGRAM}
//     );

//     printf("Parse Tree (Inorder Traversal):\n");
//     print_parse_tree_inorder(root, 0);

//     printf("\n=== PARSE TREE TEST COMPLETE ===\n");
// }