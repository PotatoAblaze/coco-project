#ifndef LEXERDEF_H
#define LEXERDEF_H

static const int INPUT_BUFFER_SIZE = 2000;
static const int TOKEN_BUFFER_SIZE = 500;
static const int TOKEN_ARRAY_SIZE = 2000;

typedef enum TokenType
{
    TK_FIELDID,
    TK_ID,
    TK_NUM,
    TK_SQL,
    TK_SQR,
    TK_OP,
    TK_CL,
    TK_COMMA,
    TK_DOT,
    TK_SEM,
    TK_NOT,
    TK_COLON,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_FUNID,
    TK_RUID,
    TK_COMMENT,
    TK_AND,
    TK_OR,
    TK_RNUM,
    TK_LT,
    TK_LE,
    TK_ASSIGNOP,
    TK_GE,
    TK_GT,
    TK_EQ,
    TK_NE,

    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_ENDWHILE,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_EPSILON,
    TK_EOF,
    TK_COUNT,
} TokenType;

static inline const char *get_token_name(TokenType type)
{
    switch (type)
    {
    case TK_FIELDID:
        return "TK_FIELDID";
    case TK_ID:
        return "TK_ID";
    case TK_NUM:
        return "TK_NUM";
    case TK_SQL:
        return "TK_SQL";
    case TK_SQR:
        return "TK_SQR";
    case TK_OP:
        return "TK_OP";
    case TK_CL:
        return "TK_CL";
    case TK_COMMA:
        return "TK_COMMA";
    case TK_DOT:
        return "TK_DOT";
    case TK_SEM:
        return "TK_SEM";
    case TK_NOT:
        return "TK_NOT";
    case TK_COLON:
        return "TK_COLON";
    case TK_PLUS:
        return "TK_PLUS";
    case TK_MINUS:
        return "TK_MINUS";
    case TK_MUL:
        return "TK_MUL";
    case TK_DIV:
        return "TK_DIV";
    case TK_FUNID:
        return "TK_FUNID";
    case TK_RUID:
        return "TK_RUID";
    case TK_COMMENT:
        return "TK_COMMENT";
    case TK_AND:
        return "TK_AND";
    case TK_OR:
        return "TK_OR";
    case TK_RNUM:
        return "TK_RNUM";
    case TK_LT:
        return "TK_LT";
    case TK_LE:
        return "TK_LE";
    case TK_ASSIGNOP:
        return "TK_ASSIGNOP";
    case TK_GE:
        return "TK_GE";
    case TK_GT:
        return "TK_GT";
    case TK_EQ:
        return "TK_EQ";
    case TK_NE:
        return "TK_NE";
    case TK_WITH:
        return "TK_WITH";
    case TK_PARAMETERS:
        return "TK_PARAMETERS";
    case TK_END:
        return "TK_END";
    case TK_WHILE:
        return "TK_WHILE";
    case TK_UNION:
        return "TK_UNION";
    case TK_ENDUNION:
        return "TK_ENDUNION";
    case TK_DEFINETYPE:
        return "TK_DEFINETYPE";
    case TK_AS:
        return "TK_AS";
    case TK_TYPE:
        return "TK_TYPE";
    case TK_MAIN:
        return "TK_MAIN";
    case TK_GLOBAL:
        return "TK_GLOBAL";
    case TK_PARAMETER:
        return "TK_PARAMETER";
    case TK_LIST:
        return "TK_LIST";
    case TK_INPUT:
        return "TK_INPUT";
    case TK_OUTPUT:
        return "TK_OUTPUT";
    case TK_INT:
        return "TK_INT";
    case TK_REAL:
        return "TK_REAL";
    case TK_ENDWHILE:
        return "TK_ENDWHILE";
    case TK_IF:
        return "TK_IF";
    case TK_THEN:
        return "TK_THEN";
    case TK_ENDIF:
        return "TK_ENDIF";
    case TK_READ:
        return "TK_READ";
    case TK_WRITE:
        return "TK_WRITE";
    case TK_RETURN:
        return "TK_RETURN";
    case TK_CALL:
        return "TK_CALL";
    case TK_RECORD:
        return "TK_RECORD";
    case TK_ENDRECORD:
        return "TK_ENDRECORD";
    case TK_ELSE:
        return "TK_ELSE";
    case TK_EPSILON:
        return "TK_EPSILON";
    case TK_EOF:
        return "TK_EOF";
    default:
        return "UNKNOWN";
    }
}

typedef struct
{
    const char *keyword;
    TokenType type;
} KeywordMap;

static KeywordMap keyword_table[] = {
    {"with", TK_WITH},
    {"parameters", TK_PARAMETERS},
    {"end", TK_END},
    {"while", TK_WHILE},
    {"union", TK_UNION},
    {"endunion", TK_ENDUNION},
    {"definetype", TK_DEFINETYPE},
    {"as", TK_AS},
    {"type", TK_TYPE},
    {"_main", TK_MAIN},
    {"global", TK_GLOBAL},
    {"parameter", TK_PARAMETER},
    {"list", TK_LIST},
    {"input", TK_INPUT},
    {"output", TK_OUTPUT},
    {"int", TK_INT},
    {"real", TK_REAL},
    {"endwhile", TK_ENDWHILE},
    {"if", TK_IF},
    {"then", TK_THEN},
    {"endif", TK_ENDIF},
    {"read", TK_READ},
    {"write", TK_WRITE},
    {"return", TK_RETURN},
    {"call", TK_CALL},
    {"record", TK_RECORD},
    {"endrecord", TK_ENDRECORD},
    {"else", TK_ELSE}};

typedef struct Token
{
    TokenType type;
    int line_number;
    char *token;
} Token;

#endif