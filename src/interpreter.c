#include "../headers/interpreter.h"

void *get_it_val_from_curr_scope(const char *key)
{
    if (map_get(key, &it_value, ((Map_t *)IT_Stack->top)) == 0)
        return it_value;
    else if (map_get(key, &it_value, global_ident_table) == 0)
        return it_value;
    else
        return NULL;
}

int update_it_val_in_curr_scope(const char *key, void *value)
{
    return map_update(key, value, ((Map_t *)IT_Stack->top));
}

int create_it_val_in_curr_scope(const char *key, void *value)
{
    return map_add(key, value, ((Map_t *)IT_Stack->top));
}

int handle_inner_program(AST_Node *ast_node)
{
}

int handle_fundef(AST_Node *ast_node)
{
}

int handle_expr(AST_Node *ast_node)
{
}

int handle_output(AST_Node *ast_node)
{
}

int handle_input(AST_Node *ast_node)
{
}

int handle_funcall(AST_Node *ast_node)
{
}

int handle_assign(AST_Node *ast_node)
{
    int state = handle_expr(ast_node->expr);
    void *past_val, *new_val;
    void **new_ptr;
    if (ast_node->expr->type == AST_NODE_TYPE_FUNDEF)
    {
        new_ptr = malloc(sizeof(AST_Node));
        CHECK_MALLOC_INT(new_ptr)
    }
    else
    {
        new_ptr = malloc(sizeof(int));
        CHECK_MALLOC_INT(new_ptr)
    }
    *new_ptr = expr_return;
    
    if ((past_val = get_it_val_from_curr_scope(ast_node->ident->name)))
    {
        if (ast_node->expr->type == AST_NODE_TYPE_FUNDEF)
            destroy_ast_node(past_val);
        else
            free(past_val);

        update_it_val_in_curr_scope(ast_node->ident->name, new_ptr);
    }
    else
    {
        create_it_val_in_curr_scope(ast_node->ident->name, new_ptr);
    }
}

int handle_program(AST_Node *ast_node)
{
    global_ident_table = map_create();
    stack_push(global_ident_table, IT_Stack);
    AST_Node *stmnt = ast_node->first_stmnt;
    while (stmnt)
    {
        switch (stmnt->expand->type)
        {
        case AST_NODE_TYPE_ASSIGN:
            handle_assign(stmnt->expand); break;
        case AST_NODE_TYPE_FUNCALL:
            handle_funcall(stmnt->expand); break;
        case AST_NODE_TYPE_INPUT:
            handle_input(stmnt->expand); break;
        case AST_NODE_TYPE_OUTPUT:
            handle_output(stmnt->expand); break;
        }
        stmnt = stmnt->next_stmnt;
    }
    
    map_destroy(global_ident_table);
}

int interpret(AST_Node *ast_root)
{
    IT_Stack = stack_create();
    it_value = (void *)malloc(sizeof(AST_Node));
    int state = handle_program(ast_root);
    stack_destroy(IT_Stack);
    free(it_value);
    return state;
}
