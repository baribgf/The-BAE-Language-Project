#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef TOKENS_H
#include "tokens.h"
#endif

#define CHECK_INPUT_FILE                                       \
    if (argc < 2)                                              \
    {                                                          \
        fprintf(stderr, "Error: Input source file required."); \
        return 1;                                              \
    }

#define CHECK_MALLOC(ptr)                                          \
    if (!ptr)                                                      \
    {                                                              \
        fprintf(stderr, "Fatal error: Couldn't allocate memory."); \
        exit(2);                                                   \
    }

#define INITIALIZE_INPUT_FILE yyin = open_file(filename, "r");
#define MAX_IDENT_SIZE 25

typedef struct token_pair_t
{
    TOKEN value;
    char text[MAX_IDENT_SIZE];
} Token_Pair_t;

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
    AST_NODE_TYPE_NOT,
    AST_NODE_TYPE_OR,
    AST_NODE_TYPE_AND,
    AST_NODE_TYPE_FUNCALL,
    AST_NODE_TYPE_ARG,
    AST_NODE_TYPE_FUNDEF,
    AST_NODE_TYPE_PARAM,
    AST_NODE_TYPE_INPUT,
    AST_NODE_TYPE_OUTPUT,
    AST_NODE_TYPE_OPERATOR,
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
    struct AST_Node_t *operator;        // for left & right side nodes
    struct AST_Node_t *operand;         // for left & right side nodes
    struct AST_Node_t *expand;
    char name[MAX_IDENT_SIZE];                      // for identifier node
    int value;                          // for bit node
} AST_Node;

typedef struct stack_node_t
{
    void *value;
    struct stack_node_t *prev;
} Stack_Node_t;

typedef struct
{
    Stack_Node_t *top;
    int size;
} Stack_t;

typedef struct list_node_t
{
    void *value;
    struct list_node_t *prev;
    struct list_node_t *next;
} List_Node_t;

typedef struct list_t
{
    List_Node_t *head;
    List_Node_t *tail;
    int size;
} List_t;

typedef struct map_pair_t
{
    char key[MAX_IDENT_SIZE];
    void *value;
} Map_Pair_t;

typedef struct map_t
{
    int size;
    List_t *pairs;
} Map_t;

typedef struct fundef_it_val {
    List_t *params;
    AST_Node *first_stmnt;
} Fundef_IT_Val;

typedef struct it_value_t
{
    int value;
    Fundef_IT_Val *fundef_val;
} IT_Value_t;

AST_Node *create_ast_node(AST_Node_Type type);
AST_Node *create_AST_Program_Node(AST_Node *first_stmnt);
AST_Node *create_AST_Stmnt_Node(AST_Node *expand, AST_Node *next_stmnt);
AST_Node *create_AST_Assign_Node(AST_Node *ident, AST_Node *expr);
AST_Node *create_AST_Funcall_Node(AST_Node *ident, AST_Node *first_arg);
AST_Node *create_AST_Input_Node(AST_Node *ident);
AST_Node *create_AST_Output_Node(AST_Node *expr);
AST_Node *create_AST_Arg_Node(AST_Node *expr, AST_Node *next_arg);
AST_Node *create_AST_Fundef_Node(AST_Node *first_param, AST_Node *first_stmnt);
AST_Node *create_AST_Param_Node(AST_Node *ident, AST_Node *next_param);
AST_Node *create_AST_Expr_Node(AST_Node *expand);
AST_Node *create_AST_LSide_Node(AST_Node *expand, AST_Node *operator);
AST_Node *create_AST_RSide_Node(AST_Node *operator, AST_Node *operand);
AST_Node *create_AST_Neg_Node(AST_Node *expr);
AST_Node *create_AST_Disj_Node(AST_Node *left_side, AST_Node *right_side);
AST_Node *create_AST_Conj_Node(AST_Node *left_side, AST_Node *right_side);
AST_Node *create_AST_Ident_Node(char *name);
AST_Node *create_AST_Bit_Node(int value);
AST_Node *create_AST_Operator_Node(AST_Node_Type type);
void print_ast(AST_Node *root);
char *ast_node_typename(AST_Node_Type type);
void destroy_ast_node(AST_Node *node);

extern FILE *yyin;
extern int yylex();
extern char *yytext;
extern int yylex_destroy(void);
extern Stack_S *RDStack;

char *token_name(TOKEN t);
FILE *open_file(const char *filename, char *mode);

Stack_S *stack_create_s();
int stack_destroy_s(Stack_S *stack);
int stack_push_s(char *value, Stack_S *stack);
char *stack_pop_s(Stack_S *stack);
int print_stack_s(Stack_S *stack);

Stack_I *stack_create_i();
int stack_destroy_i(Stack_I *stack);
int stack_push_i(int value, Stack_I *stack);
int stack_pop_i(Stack_I *stack);
int print_stack_i(Stack_I *stack);

Stack_t *stack_create();
int stack_destroy(Stack_t *stack);
int stack_push(void *value, Stack_t *stack);
void *stack_pop(Stack_t *stack);
int print_ast_stack(Stack_t *stack);

List_t *list_create();
int list_add(void *ptr, List_t *list);
int list_destroy(List_t *list);
void print_list_i(List_t *list);

unsigned long hash(const char *str);
Map_t *map_create();
int map_add(const char *key, void *value, Map_t *map);
int map_get(const char *key, void **buff, Map_t *map);
int map_update(const char *key, void *value, Map_t *map);
int map_destroy(Map_t *map);

#endif // COMMON_H
