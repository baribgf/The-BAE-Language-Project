#ifndef INTERPRETER_H
#define INTERPRETER_H

#ifndef COMMON_H
#include "common.h"
#endif

int handle_program(AST_Node *ast_node);
int interpret(AST_Node *ast_root);

static Map_t *global_ident_table;
static Stack_t *IT_Stack;
extern void *it_value;
extern void *expr_return;

#endif // INTERPRETER_H
