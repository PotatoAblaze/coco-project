#ifndef PARSERDEF_H
#define PARSERDEF_H

#include <stdbool.h>
#include "lexer.h"
#include <stdlib.h>

typedef enum Variable
{
    VAR_PROGRAM,
    VAR_MAIN_FUNCTION,
    VAR_OTHER_FUNCTIONS,
    VAR_FUNCTION,
    VAR_INPUT_PAR,
    VAR_OUTPUT_PAR,
    VAR_PARAMETER_LIST,
    VAR_DATA_TYPE,
    VAR_PRIMITIVE_DATATYPE,
    VAR_CONSTRUCTED_DATATYPE,
    VAR_REMAINING_LIST,
    VAR_STMTS,
    VAR_TYPE_DEFINITIONS,
    VAR_ACTUAL_OR_REDEFINED,
    VAR_TYPE_DEFINITION,
    VAR_FIELD_DEFINITIONS,
    VAR_FIELD_DEFINITION,
    VAR_FIELD_TYPE,
    VAR_MORE_FIELDS,
    VAR_DECLARATIONS,
    VAR_DECLARATION,
    VAR_GLOBAL_OR_NOT,
    VAR_OTHER_STMTS,
    VAR_STMT,
    VAR_ASSIGNMENT_STMT,
    VAR_SINGLE_OR_REC_ID,
    VAR_OPTION_SINGLE_CONSTRUCTED,
    VAR_ONE_EXPANSION,
    VAR_MORE_EXPANSIONS,
    VAR_FUN_CALL_STMT,
    VAR_OUTPUT_PARAMETERS,
    VAR_INPUT_PARAMETERS,
    VAR_ITERATIVE_STMT,
    VAR_CONDITIONAL_STMT,
    VAR_ELSE_PART,
    VAR_IO_STMT,
    VAR_ARITHMETIC_EXPRESSION,
    VAR_EXP_PRIME,
    VAR_TERM,
    VAR_TERM_PRIME,
    VAR_FACTOR,
    VAR_HIGH_PRECEDENCE_OPERATORS,
    VAR_LOW_PRECEDENCE_OPERATORS,
    VAR_BOOLEAN_EXPRESSION,
    VAR_VAR,
    VAR_LOGICAL_OP,
    VAR_RELATIONAL_OP,
    VAR_RETURN_STMT,
    VAR_OPTIONAL_RETURN,
    VAR_ID_LIST,
    VAR_MORE_IDS,
    VAR_DEFINETYPESTMT,
    VAR_A,
    VAR_COUNT
} Variable;

static inline const char *get_variable_name(Variable var)
{
    switch (var)
    {
    case VAR_PROGRAM: return "PROGRAM";
    case VAR_MAIN_FUNCTION: return "MAIN_FUNCTION";
    case VAR_OTHER_FUNCTIONS: return "OTHER_FUNCTIONS";
    case VAR_FUNCTION: return "FUNCTION";
    case VAR_INPUT_PAR: return "INPUT_PAR";
    case VAR_OUTPUT_PAR: return "OUTPUT_PAR";
    case VAR_PARAMETER_LIST: return "PARAMETER_LIST";
    case VAR_DATA_TYPE: return "DATA_TYPE";
    case VAR_PRIMITIVE_DATATYPE: return "PRIMITIVE_DATATYPE";
    case VAR_CONSTRUCTED_DATATYPE: return "CONSTRUCTED_DATATYPE";
    case VAR_REMAINING_LIST: return "REMAINING_LIST";
    case VAR_STMTS: return "STMTS";
    case VAR_TYPE_DEFINITIONS: return "TYPE_DEFINITIONS";
    case VAR_ACTUAL_OR_REDEFINED: return "ACTUAL_OR_REDEFINED";
    case VAR_TYPE_DEFINITION: return "TYPE_DEFINITION";
    case VAR_FIELD_DEFINITIONS: return "FIELD_DEFINITIONS";
    case VAR_FIELD_DEFINITION: return "FIELD_DEFINITION";
    case VAR_FIELD_TYPE: return "FIELD_TYPE";
    case VAR_MORE_FIELDS: return "MORE_FIELDS";
    case VAR_DECLARATIONS: return "DECLARATIONS";
    case VAR_DECLARATION: return "DECLARATION";
    case VAR_GLOBAL_OR_NOT: return "GLOBAL_OR_NOT";
    case VAR_OTHER_STMTS: return "OTHER_STMTS";
    case VAR_STMT: return "STMT";
    case VAR_ASSIGNMENT_STMT: return "ASSIGNMENT_STMT";
    case VAR_SINGLE_OR_REC_ID: return "SINGLE_OR_REC_ID";
    case VAR_OPTION_SINGLE_CONSTRUCTED: return "OPTION_SINGLE_CONSTRUCTED";
    case VAR_ONE_EXPANSION: return "ONE_EXPANSION";
    case VAR_MORE_EXPANSIONS: return "MORE_EXPANSIONS";
    case VAR_FUN_CALL_STMT: return "FUN_CALL_STMT";
    case VAR_OUTPUT_PARAMETERS: return "OUTPUT_PARAMETERS";
    case VAR_INPUT_PARAMETERS: return "INPUT_PARAMETERS";
    case VAR_ITERATIVE_STMT: return "ITERATIVE_STMT";
    case VAR_CONDITIONAL_STMT: return "CONDITIONAL_STMT";
    case VAR_ELSE_PART: return "ELSE_PART";
    case VAR_IO_STMT: return "IO_STMT";
    case VAR_ARITHMETIC_EXPRESSION: return "ARITHMETIC_EXPRESSION";
    case VAR_EXP_PRIME: return "EXP_PRIME";
    case VAR_TERM: return "TERM";
    case VAR_TERM_PRIME: return "TERM_PRIME";
    case VAR_FACTOR: return "FACTOR";
    case VAR_HIGH_PRECEDENCE_OPERATORS: return "HIGH_PRECEDENCE_OPERATORS";
    case VAR_LOW_PRECEDENCE_OPERATORS: return "LOW_PRECEDENCE_OPERATORS";
    case VAR_BOOLEAN_EXPRESSION: return "BOOLEAN_EXPRESSION";
    case VAR_VAR: return "VAR";
    case VAR_LOGICAL_OP: return "LOGICAL_OP";
    case VAR_RELATIONAL_OP: return "RELATIONAL_OP";
    case VAR_RETURN_STMT: return "RETURN_STMT";
    case VAR_OPTIONAL_RETURN: return "OPTIONAL_RETURN";
    case VAR_ID_LIST: return "ID_LIST";
    case VAR_MORE_IDS: return "MORE_IDS";
    case VAR_DEFINETYPESTMT: return "DEFINETYPESTMT";
    case VAR_A: return "A";
    default: return "UNKNOWN";
    }
}

typedef struct Term
{
    bool is_terminal;
    union
    {
        Variable var;
        TokenType terminal_type;
    };
} Term;

typedef struct GrammarRule
{
    Variable root;
    const Term *expansion;
    int expansion_length;
} GrammarRule;

typedef struct TerminalArray
{
    int size;
    int capacity;
    TokenType *array;
} TerminalArray;

typedef struct TerminalSet
{
    unsigned long long bits;
} TerminalSet;

typedef struct FirstAndFollowEntry
{
    Term term;
    TerminalSet first;
    TerminalSet follow;
} FirstAndFollowEntry;

typedef int ParseTable[128][128];

typedef struct ParseTreeNode {
    Token* token;
    Term term;
    int child_count;
    struct ParseTreeNode* children[10];
} ParseTreeNode;

typedef struct TermStack {
    int size;
    int capacity;
    Term* array;
} TermStack;

#define T(t) ((Term){.is_terminal = true, .terminal_type = t})
#define V(v) ((Term){.is_terminal = false, .var = v})

#define RULE(lhs, ...) {                \
    .root = lhs,                        \
    .expansion = (Term[]){__VA_ARGS__}, \
    .expansion_length = sizeof((Term[]){__VA_ARGS__}) / sizeof(Term)}
#define EPSILON_RULE(lhs) { \
    .root = lhs,            \
    .expansion = NULL,      \
    .expansion_length = 0}

static const GrammarRule grammar[] = {
    // 1. <program> ===> <otherFunctions> <mainFunction>
    RULE(VAR_PROGRAM, V(VAR_OTHER_FUNCTIONS), V(VAR_MAIN_FUNCTION)),

    // 2. <mainFunction> ===> TK_MAIN <stmts> TK_END
    RULE(VAR_MAIN_FUNCTION, T(TK_MAIN), V(VAR_STMTS), T(TK_END)),

    // 3. <otherFunctions> ===> <function> <otherFunctions> | epsilon
    RULE(VAR_OTHER_FUNCTIONS, V(VAR_FUNCTION), V(VAR_OTHER_FUNCTIONS)),
    EPSILON_RULE(VAR_OTHER_FUNCTIONS),

    // 4. <function> ===> TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
    RULE(VAR_FUNCTION, T(TK_FUNID), V(VAR_INPUT_PAR), V(VAR_OUTPUT_PAR), T(TK_SEM), V(VAR_STMTS), T(TK_END)),

    // 5. <input_par> ===> TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
    RULE(VAR_INPUT_PAR, T(TK_INPUT), T(TK_PARAMETER), T(TK_LIST), T(TK_SQL), V(VAR_PARAMETER_LIST), T(TK_SQR)),

    // 6. <output_par> ===> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR | epsilon
    RULE(VAR_OUTPUT_PAR, T(TK_OUTPUT), T(TK_PARAMETER), T(TK_LIST), T(TK_SQL), V(VAR_PARAMETER_LIST), T(TK_SQR)),
    EPSILON_RULE(VAR_OUTPUT_PAR),

    // 7. <parameter_list> ===> <dataType> TK_ID <remaining_list>
    RULE(VAR_PARAMETER_LIST, V(VAR_DATA_TYPE), T(TK_ID), V(VAR_REMAINING_LIST)),

    // 8. <dataType> ===> <primitiveDatatype> | <constructedDatatype>
    RULE(VAR_DATA_TYPE, V(VAR_PRIMITIVE_DATATYPE)),
    RULE(VAR_DATA_TYPE, V(VAR_CONSTRUCTED_DATATYPE)),

    // 9. <primitiveDatatype> ===> TK_INT | TK_REAL
    RULE(VAR_PRIMITIVE_DATATYPE, T(TK_INT)),
    RULE(VAR_PRIMITIVE_DATATYPE, T(TK_REAL)),

    // 10. <constructedDatatype> ===> TK_RECORD TK_RUID | TK_UNION TK_RUID | TK_RUID
    RULE(VAR_CONSTRUCTED_DATATYPE, T(TK_RECORD), T(TK_RUID)),
    RULE(VAR_CONSTRUCTED_DATATYPE, T(TK_UNION), T(TK_RUID)),
    RULE(VAR_CONSTRUCTED_DATATYPE, T(TK_RUID)),

    // 11. <remaining_list> ===> TK_COMMA <parameter_list> | epsilon
    RULE(VAR_REMAINING_LIST, T(TK_COMMA), V(VAR_PARAMETER_LIST)),
    EPSILON_RULE(VAR_REMAINING_LIST),

    // 12. <stmts> ===> <typeDefinitions> <declarations> <otherStmts> <returnStmt>
    RULE(VAR_STMTS, V(VAR_TYPE_DEFINITIONS), V(VAR_DECLARATIONS), V(VAR_OTHER_STMTS), V(VAR_RETURN_STMT)),

    // 13. <typeDefinitions> ===> <actualOrRedefined> <typeDefinitions> | epsilon
    RULE(VAR_TYPE_DEFINITIONS, V(VAR_ACTUAL_OR_REDEFINED), V(VAR_TYPE_DEFINITIONS)),
    EPSILON_RULE(VAR_TYPE_DEFINITIONS),
    
    // 13b. <actualOrRedefined> ===> <typeDefinition> | <definetypestmt>
    RULE(VAR_ACTUAL_OR_REDEFINED, V(VAR_TYPE_DEFINITION)),
    RULE(VAR_ACTUAL_OR_REDEFINED, V(VAR_DEFINETYPESTMT)),

    // 14. <typeDefinition> ===> TK_RECORD TK_RUID <fieldDefinitions> TK_ENDRECORD
    RULE(VAR_TYPE_DEFINITION, T(TK_RECORD), T(TK_RUID), V(VAR_FIELD_DEFINITIONS), T(TK_ENDRECORD)),

    // 15. <typeDefinition> ===> TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION
    RULE(VAR_TYPE_DEFINITION, T(TK_UNION), T(TK_RUID), V(VAR_FIELD_DEFINITIONS), T(TK_ENDUNION)),

    // 16. <fieldDefinitions> ===> <fieldDefinition> <fieldDefinition> <moreFields>
    RULE(VAR_FIELD_DEFINITIONS, V(VAR_FIELD_DEFINITION), V(VAR_FIELD_DEFINITION), V(VAR_MORE_FIELDS)),

    // 17. <fieldDefinition> ===> TK_TYPE <fieldType> TK_COLON TK_FIELDID TK_SEM
    RULE(VAR_FIELD_DEFINITION, T(TK_TYPE), V(VAR_FIELD_TYPE), T(TK_COLON), T(TK_FIELDID), T(TK_SEM)),
    
    // 17a. <fieldType> ===> <primitiveDatatype> | <constructedDatatype>
    RULE(VAR_FIELD_TYPE, V(VAR_PRIMITIVE_DATATYPE)),
    RULE(VAR_FIELD_TYPE, V(VAR_CONSTRUCTED_DATATYPE)),

    // 18. <moreFields> ===> <fieldDefinition> <moreFields> | epsilon
    RULE(VAR_MORE_FIELDS, V(VAR_FIELD_DEFINITION), V(VAR_MORE_FIELDS)),
    EPSILON_RULE(VAR_MORE_FIELDS),

    // 19. <declarations> ===> <declaration> <declarations> | epsilon
    RULE(VAR_DECLARATIONS, V(VAR_DECLARATION), V(VAR_DECLARATIONS)),
    EPSILON_RULE(VAR_DECLARATIONS),

    // 20. <declaration> ===> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM
    RULE(VAR_DECLARATION, T(TK_TYPE), V(VAR_DATA_TYPE), T(TK_COLON), T(TK_ID), V(VAR_GLOBAL_OR_NOT), T(TK_SEM)),

    // 21. <global_or_not> ===> TK_COLON TK_GLOBAL | epsilon
    RULE(VAR_GLOBAL_OR_NOT, T(TK_COLON), T(TK_GLOBAL)),
    EPSILON_RULE(VAR_GLOBAL_OR_NOT),

    // 22. <otherStmts> ===> <stmt> <otherStmts> | epsilon
    RULE(VAR_OTHER_STMTS, V(VAR_STMT), V(VAR_OTHER_STMTS)),
    EPSILON_RULE(VAR_OTHER_STMTS),

    // 23. <stmt> ===> <assignmentStmt> | <iterativeStmt> | <conditionalStmt> | <ioStmt> | <funCallStmt>
    RULE(VAR_STMT, V(VAR_ASSIGNMENT_STMT)),
    RULE(VAR_STMT, V(VAR_ITERATIVE_STMT)),
    RULE(VAR_STMT, V(VAR_CONDITIONAL_STMT)),
    RULE(VAR_STMT, V(VAR_IO_STMT)),
    RULE(VAR_STMT, V(VAR_FUN_CALL_STMT)),

    // 24. <assignmentStmt> ===> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
    RULE(VAR_ASSIGNMENT_STMT, V(VAR_SINGLE_OR_REC_ID), T(TK_ASSIGNOP), V(VAR_ARITHMETIC_EXPRESSION), T(TK_SEM)),

    // 25. <singleOrRecId> ===> TK_ID <option_single_constructed>
    RULE(VAR_SINGLE_OR_REC_ID, T(TK_ID), V(VAR_OPTION_SINGLE_CONSTRUCTED)),
    
    // 25f. <option_single_constructed> ===> epsilon | <oneExpansion> <moreExpansions>
    EPSILON_RULE(VAR_OPTION_SINGLE_CONSTRUCTED),
    RULE(VAR_OPTION_SINGLE_CONSTRUCTED, V(VAR_ONE_EXPANSION), V(VAR_MORE_EXPANSIONS)),
    
    // 25c. <oneExpansion> ===> TK_DOT TK_FIELDID
    RULE(VAR_ONE_EXPANSION, T(TK_DOT), T(TK_FIELDID)),
    
    // 25d. <moreExpansions> ===> <oneExpansion> <moreExpansions> | epsilon
    RULE(VAR_MORE_EXPANSIONS, V(VAR_ONE_EXPANSION), V(VAR_MORE_EXPANSIONS)),
    EPSILON_RULE(VAR_MORE_EXPANSIONS),

    // 26. <funCallStmt> ===> <outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM
    RULE(VAR_FUN_CALL_STMT, V(VAR_OUTPUT_PARAMETERS), T(TK_CALL), T(TK_FUNID), T(TK_WITH), T(TK_PARAMETERS), V(VAR_INPUT_PARAMETERS), T(TK_SEM)),

    // 27. <outputParameters> ===> TK_SQL <idList> TK_SQR TK_ASSIGNOP | epsilon
    RULE(VAR_OUTPUT_PARAMETERS, T(TK_SQL), V(VAR_ID_LIST), T(TK_SQR), T(TK_ASSIGNOP)),
    EPSILON_RULE(VAR_OUTPUT_PARAMETERS),

    // 28. <inputParameters> ===> TK_SQL <idList> TK_SQR
    RULE(VAR_INPUT_PARAMETERS, T(TK_SQL), V(VAR_ID_LIST), T(TK_SQR)),

    // 29. <iterativeStmt> ===> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt> <otherStmts> TK_ENDWHILE
    RULE(VAR_ITERATIVE_STMT, T(TK_WHILE), T(TK_OP), V(VAR_BOOLEAN_EXPRESSION), T(TK_CL), V(VAR_STMT), V(VAR_OTHER_STMTS), T(TK_ENDWHILE)),

    // 30. <conditionalStmt> ===> TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt> <otherStmts> <elsePart>
    RULE(VAR_CONDITIONAL_STMT, T(TK_IF), T(TK_OP), V(VAR_BOOLEAN_EXPRESSION), T(TK_CL), T(TK_THEN), V(VAR_STMT), V(VAR_OTHER_STMTS), V(VAR_ELSE_PART)),
    
    // 31. <elsePart> ===> TK_ELSE <stmt> <otherStmts> TK_ENDIF | TK_ENDIF
    RULE(VAR_ELSE_PART, T(TK_ELSE), V(VAR_STMT), V(VAR_OTHER_STMTS), T(TK_ENDIF)),
    RULE(VAR_ELSE_PART, T(TK_ENDIF)),

    // 32. <ioStmt> ===> TK_READ TK_OP <var> TK_CL TK_SEM | TK_WRITE TK_OP <var> TK_CL TK_SEM
    RULE(VAR_IO_STMT, T(TK_READ), T(TK_OP), V(VAR_VAR), T(TK_CL), T(TK_SEM)),
    RULE(VAR_IO_STMT, T(TK_WRITE), T(TK_OP), V(VAR_VAR), T(TK_CL), T(TK_SEM)),

    // 34. <arithmeticExpression> ===> <term> <expPrime>
    RULE(VAR_ARITHMETIC_EXPRESSION, V(VAR_TERM), V(VAR_EXP_PRIME)),

    // 35. <expPrime> ===> <lowPrecedenceOperators> <term> <expPrime> | epsilon
    RULE(VAR_EXP_PRIME, V(VAR_LOW_PRECEDENCE_OPERATORS), V(VAR_TERM), V(VAR_EXP_PRIME)),
    EPSILON_RULE(VAR_EXP_PRIME),

    // 36. <term> ===> <factor> <termPrime>
    RULE(VAR_TERM, V(VAR_FACTOR), V(VAR_TERM_PRIME)),

    // 37. <termPrime> ===> <highPrecedenceOperators> <factor> <termPrime> | epsilon
    RULE(VAR_TERM_PRIME, V(VAR_HIGH_PRECEDENCE_OPERATORS), V(VAR_FACTOR), V(VAR_TERM_PRIME)),
    EPSILON_RULE(VAR_TERM_PRIME),

    // 38. <factor> ===> TK_OP <arithmeticExpression> TK_CL | <var>
    RULE(VAR_FACTOR, T(TK_OP), V(VAR_ARITHMETIC_EXPRESSION), T(TK_CL)),
    RULE(VAR_FACTOR, V(VAR_VAR)),

    // 39. <highPrecedenceOperators> ===> TK_MUL | TK_DIV
    RULE(VAR_HIGH_PRECEDENCE_OPERATORS, T(TK_MUL)),
    RULE(VAR_HIGH_PRECEDENCE_OPERATORS, T(TK_DIV)),

    // 40. <lowPrecedenceOperators> ===> TK_PLUS | TK_MINUS
    RULE(VAR_LOW_PRECEDENCE_OPERATORS, T(TK_PLUS)),
    RULE(VAR_LOW_PRECEDENCE_OPERATORS, T(TK_MINUS)),

    // 41. <booleanExpression> ===> TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL
    RULE(VAR_BOOLEAN_EXPRESSION, T(TK_OP), V(VAR_BOOLEAN_EXPRESSION), T(TK_CL), V(VAR_LOGICAL_OP), T(TK_OP), V(VAR_BOOLEAN_EXPRESSION), T(TK_CL)),

    // 42. <booleanExpression> ===> <var> <relationalOp> <var>
    RULE(VAR_BOOLEAN_EXPRESSION, V(VAR_VAR), V(VAR_RELATIONAL_OP), V(VAR_VAR)),

    // 43. <booleanExpression> ===> TK_NOT TK_OP <booleanExpression> TK_CL
    RULE(VAR_BOOLEAN_EXPRESSION, T(TK_NOT), T(TK_OP), V(VAR_BOOLEAN_EXPRESSION), T(TK_CL)),

    // 44. <var> ===> <singleOrRecId> | TK_NUM | TK_RNUM
    RULE(VAR_VAR, V(VAR_SINGLE_OR_REC_ID)),
    RULE(VAR_VAR, T(TK_NUM)),
    RULE(VAR_VAR, T(TK_RNUM)),

    // 45. <logicalOp> ===> TK_AND | TK_OR
    RULE(VAR_LOGICAL_OP, T(TK_AND)),
    RULE(VAR_LOGICAL_OP, T(TK_OR)),

    // 46. <relationalOp> ===> TK_LT | TK_LE | TK_EQ | TK_GT | TK_GE | TK_NE
    RULE(VAR_RELATIONAL_OP, T(TK_LT)),
    RULE(VAR_RELATIONAL_OP, T(TK_LE)),
    RULE(VAR_RELATIONAL_OP, T(TK_EQ)),
    RULE(VAR_RELATIONAL_OP, T(TK_GT)),
    RULE(VAR_RELATIONAL_OP, T(TK_GE)),
    RULE(VAR_RELATIONAL_OP, T(TK_NE)),

    // 47. <returnStmt> ===> TK_RETURN <optionalReturn> TK_SEM
    RULE(VAR_RETURN_STMT, T(TK_RETURN), V(VAR_OPTIONAL_RETURN), T(TK_SEM)),

    // 48. <optionalReturn> ===> TK_SQL <idList> TK_SQR | epsilon
    RULE(VAR_OPTIONAL_RETURN, T(TK_SQL), V(VAR_ID_LIST), T(TK_SQR)),
    EPSILON_RULE(VAR_OPTIONAL_RETURN),

    // 49. <idList> ===> TK_ID <more_ids>
    RULE(VAR_ID_LIST, T(TK_ID), V(VAR_MORE_IDS)),

    // 50. <more_ids> ===> TK_COMMA <idList> | epsilon
    RULE(VAR_MORE_IDS, T(TK_COMMA), V(VAR_ID_LIST)),
    EPSILON_RULE(VAR_MORE_IDS),

    // 51. <definetypestmt> ===> TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID
    RULE(VAR_DEFINETYPESTMT, T(TK_DEFINETYPE), V(VAR_A), T(TK_RUID), T(TK_AS), T(TK_RUID)),

    // 52. <A> ===> TK_RECORD | TK_UNION
    RULE(VAR_A, T(TK_RECORD)),
    RULE(VAR_A, T(TK_UNION))
};

static const int GRAMMAR_SIZE = sizeof(grammar) / sizeof(GrammarRule);

#endif