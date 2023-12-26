#include "../headers/interpreter.h"

// Helper functions

int get_it_val_from_curr_scope(const char *key, IT_Value_t **it_val_buff)
{
    return map_get(key, (void **)it_val_buff, ((Map_t *)IT_Stack->top->value));
}

int update_it_val_in_curr_scope(const char *key, void *value)
{
    return map_update(key, value, ((Map_t *)IT_Stack->top->value));
}

int create_it_val_in_curr_scope(const char *key, void *value)
{
    return map_add(key, value, ((Map_t *)IT_Stack->top->value));
}

// Interpretation implementation

IT_Value_t *handle_fundef(AST_Node *ast_node)
{
    IT_Value_t *it_val = (IT_Value_t *)malloc(sizeof(IT_Value_t));
    CHECK_MALLOC(it_val)
    Fundef_IT_Val *fdef_it_val = (Fundef_IT_Val *)malloc(sizeof(Fundef_IT_Val));
    CHECK_MALLOC(fdef_it_val)

    fdef_it_val->params = list_create();
    AST_Node *param = ast_node->first_param;
    while (param && param->ident)
    {
        list_add(param->ident, fdef_it_val->params);
        param = param->next_param;
    }
    fdef_it_val->first_stmnt = ast_node->first_stmnt->first_stmnt;

    it_val->value = -1;
    it_val->fundef_val = fdef_it_val;
    return it_val;
}

int handle_funcall(AST_Node *ast_node)
{
    IT_Value_t *it_val = (IT_Value_t *)malloc(sizeof(IT_Value_t)); CHECK_MALLOC(it_val)
    int lookup_state = get_it_val_from_curr_scope(ast_node->ident->name, &it_val); // TODO: needs checking
    if (lookup_state != 0)
    {
        fprintf(stderr, "Error: Undefined name: '%s'.\n", ast_node->ident->name);
        exit(1);
    }

    Map_t *local_ident_table = map_create();
    stack_push(local_ident_table, IT_Stack);

    List_Node_t *param_node = it_val->fundef_val->params->tail;
    AST_Node *arg = ast_node->first_arg;

    while (param_node && arg->next_arg)
    {
        IT_Value_t *arg_val = (IT_Value_t *)malloc(sizeof(IT_Value_t)); CHECK_MALLOC(arg_val)
        Map_t *p = stack_pop(IT_Stack);
        arg_val->value = handle_expr(arg->expr);
        stack_push(p, IT_Stack);
        create_it_val_in_curr_scope(((AST_Node *)param_node->value)->name, arg_val);
        param_node = param_node->next;
        arg = arg->next_arg;
    }
    if (arg->next_arg != NULL)
    {
        fprintf(stderr, "Error: Too many arguments for function call: '%s'.\n", ast_node->ident->name);
        exit(1);
    }
    if (param_node != NULL)
    {
        fprintf(stderr, "Error: Too few arguments for function call: '%s'.\n", ast_node->ident->name);
        exit(1);
    }
    int value = handle_inner_program(it_val->fundef_val->first_stmnt);
    stack_pop(IT_Stack);
    return value;
}

int handle_expr(AST_Node *ast_node)
{
    int value;
    switch (ast_node->type)
    {
    case AST_NODE_TYPE_BIT:
        value = ast_node->value; break;
    case AST_NODE_TYPE_IDENT:
        IT_Value_t *it_val = (IT_Value_t *)malloc(sizeof(IT_Value_t)); CHECK_MALLOC(it_val)
        if (get_it_val_from_curr_scope(ast_node->name, &it_val) != 0)
        {
            fprintf(stderr, "Error: Undefined name: '%s'.\n", ast_node->name);
            exit(1);
        }
        value = it_val->value; break;
    case AST_NODE_TYPE_DISJ:
        value = handle_expr(ast_node->left_side) || handle_expr(ast_node->right_side); break;
    case AST_NODE_TYPE_CONJ:
        value = handle_expr(ast_node->left_side) && handle_expr(ast_node->right_side); break;
    case AST_NODE_TYPE_NEG:
        value = !handle_expr(ast_node->expr); break;
    case AST_NODE_TYPE_FUNCALL:
        value = handle_funcall(ast_node); break;
    }

    return value;
}

void handle_output(AST_Node *ast_node)
{
    output_count++;
    printf("[Out: %d] %d\n", output_count, handle_expr(ast_node->expr));
}

void handle_input(AST_Node *ast_node)
{
    input_count++;
    int val;
    printf("[In: %d] ", input_count);
    scanf("%d", &val);
    if (val > 1 || val < 0)
    {
        fprintf(stderr, "Error: Invalid bit value, got: %d\n", val);
        exit(1);
    }
    IT_Value_t *new_it_val_ptr;
    IT_Value_t *curr_it_val;
    new_it_val_ptr = (IT_Value_t *)malloc(sizeof(IT_Value_t)); CHECK_MALLOC(new_it_val_ptr)
    new_it_val_ptr->fundef_val = NULL;
    new_it_val_ptr->value = val;

    if (get_it_val_from_curr_scope(ast_node->ident->name, &curr_it_val) == 0)
        update_it_val_in_curr_scope(ast_node->ident->name, new_it_val_ptr);
    else
        create_it_val_in_curr_scope(ast_node->ident->name, new_it_val_ptr);
}

int handle_assign(AST_Node *ast_node)
{
    int value = 0;
    IT_Value_t *new_it_val_ptr;
    IT_Value_t *curr_it_val;
    
    curr_it_val = (void *)malloc(sizeof(IT_Value_t)); CHECK_MALLOC(curr_it_val)
    if (ast_node->expr->type == AST_NODE_TYPE_FUNDEF)
    {
        new_it_val_ptr = handle_fundef(ast_node->expr);
        if (get_it_val_from_curr_scope(ast_node->ident->name, &curr_it_val) == 0)
            update_it_val_in_curr_scope(ast_node->ident->name, new_it_val_ptr);
        else
            create_it_val_in_curr_scope(ast_node->ident->name, new_it_val_ptr);

        // free(curr_it_val); // not sure !!
    }
    else
    {
        new_it_val_ptr = (IT_Value_t *)malloc(sizeof(IT_Value_t)); CHECK_MALLOC(new_it_val_ptr)
        new_it_val_ptr->fundef_val = NULL;
        new_it_val_ptr->value = handle_expr(ast_node->expr);
        value = new_it_val_ptr->value;

        if (get_it_val_from_curr_scope(ast_node->ident->name, &curr_it_val) == 0)
            update_it_val_in_curr_scope(ast_node->ident->name, new_it_val_ptr);
        else
            create_it_val_in_curr_scope(ast_node->ident->name, new_it_val_ptr);

        // free(curr_it_val); // not sure !!
    }

    return value;
}

int handle_inner_program(AST_Node *ast_node)
{
    AST_Node *stmnt = ast_node;
    int return_val = 0;
    while (stmnt)
    {
        switch (stmnt->expand->type)
        {
        case AST_NODE_TYPE_ASSIGN:
            return_val = handle_assign(stmnt->expand); break;
        case AST_NODE_TYPE_FUNCALL:
            return_val = handle_funcall(stmnt->expand); break;
        case AST_NODE_TYPE_INPUT:
            handle_input(stmnt->expand); break;
        case AST_NODE_TYPE_OUTPUT:
            handle_output(stmnt->expand); break;
        }
        stmnt = stmnt->next_stmnt;
    }

    return return_val;
}

void handle_program(AST_Node *ast_node)
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

void interpret(AST_Node *ast_root)
{
    IT_Stack = stack_create();
    input_count = 0;
    output_count = 0;
    handle_program(ast_root);
    stack_destroy(IT_Stack);
}
