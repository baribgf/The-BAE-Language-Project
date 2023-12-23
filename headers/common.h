
#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef TOKENS_H
#include "tokens.h"
#endif

#define CHECK_INPUT_FILE                         \
    if (argc < 2)                                \
    {                                            \
        fprintf(stderr, "Input file required."); \
        return 1;                                \
    }

#define CHECK_MALLOC(ptr)                             \
    if (!ptr)                                         \
    {                                                 \
        fprintf(stderr, "Couldn't allocate memory."); \
        return NULL;                                  \
    }

#define CHECK_MALLOC_INT(ptr)                         \
    if (!ptr)                                         \
    {                                                 \
        fprintf(stderr, "Couldn't allocate memory."); \
        return 1;                                     \
    }

#define INITIALIZE_INPUT_FILE yyin = open_file(filename, "r");

typedef struct stack_node_s_t
{
    char *value;
    struct stack_node_s_t *prev;
} Stack_Node_S;

typedef struct
{
    Stack_Node_S *top;
    int size;
} Stack_S;

typedef struct stack_node_i_t
{
    int value;
    struct stack_node_i_t *prev;
} Stack_Node_I;

typedef struct
{
    Stack_Node_I *top;
    int size;
} Stack_I;

typedef enum {
    AST_NODE_TYPE_PROGRAM,
    AST_NODE_TYPE_STMNT,
    AST_NODE_TYPE_ASSIGN,
    AST_NODE_TYPE_IDENT,
    AST_NODE_TYPE_BIT,
    AST_NODE_TYPE_EXPR,
    AST_NODE_TYPE_LSIDE,
    AST_NODE_TYPE_RSIDE,
    AST_NODE_TYPE_NEG,
    AST_NODE_TYPE_DISJ,
    AST_NODE_TYPE_CONJ,
    AST_NODE_TYPE_FUNCALL,
    AST_NODE_TYPE_ARG,
    AST_NODE_TYPE_FUNDEF,
    AST_NODE_TYPE_PARAM,
    AST_NODE_TYPE_INPUT,
    AST_NODE_TYPE_OUTPUT
} AST_Node_Type;

typedef struct AST_Node_t
{
    AST_Node_Type type;
    struct AST_Node_t *first_stmnt;     // for program node
    struct AST_Node_t *next_stmnt;      // for statement node
    struct AST_Node_t *ident;           // for assignment & function call nodes
    struct AST_Node_t *expr;            // for assignment & IO nodes
    struct AST_Node_t *first_arg;       // for function call node
    struct AST_Node_t *next_arg;        // for arg node
    struct AST_Node_t *first_param;     // for function definition node
    struct AST_Node_t *next_param;      // for param node
    struct AST_Node_t *left_side;       // for expression node
    struct AST_Node_t *right_side;      // for expression node
    struct AST_Node_t *name;            // for identifier node
    struct AST_Node_t *value;           // for bit node
    struct AST_Node_t *expand;
} AST_Node;

AST_Node *create_ast_node(AST_Node_Type type);
void destroy_ast_node(AST_Node *node);

extern FILE *yyin;
extern int yylex();
extern char *yytext;
extern int yylex_destroy(void);
extern Stack_S *RDStack;
extern Stack_I *State_Stack;

char *token_name(TOKEN t);
FILE *open_file(const char *filename, char *mode);

Stack_S *create_stack_s();
int destroy_stack_s(Stack_S *stack);
int push_stack_s(char *value, Stack_S *stack);
char *pop_stack_s(Stack_S *stack);
int print_stack_s(Stack_S *stack);

Stack_I *create_stack_i();
int destroy_stack_i(Stack_I *stack);
int push_stack_i(int value, Stack_I *stack);
int pop_stack_i(Stack_I *stack);
int print_stack_i(Stack_I *stack);

#endif // HELPER_H
