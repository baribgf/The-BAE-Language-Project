#ifndef INTERPRETER_H
#define INTERPRETER_H

#ifndef COMMON_H
#include "common.h"
#endif

void handle_program(AST_Node *ast_node);
int handle_inner_program(AST_Node *ast_node);
int handle_expr(AST_Node *ast_node);
void interpret(AST_Node *ast_root);

static Map_t *global_ident_table;
static Stack_t *IT_Stack;
static unsigned int input_count;
static unsigned int output_count;

#endif // INTERPRETER_H
