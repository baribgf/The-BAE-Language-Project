#include "../headers/common.h"

FILE *open_file(const char *filename, char *mode)
{
    FILE *file = fopen(filename, mode);

    if (!file)
    {
        fprintf(stderr, "Couldn't open file: '%s'.\n", filename);
        _exit(1);
    }

    return file;
}

char *token_name(TOKEN t)
{
    switch (t)
    {
    case TOKEN_EOF:
        return "TOKEN_EOF";
    case TOKEN_BIT:
        return "TOKEN_BIT";
    case TOKEN_IDENT:
        return "TOKEN_IDENT";
    case TOKEN_ASSIGN_OP:
        return "TOKEN_ASSIGN_OP";
    case TOKEN_OR_OP:
        return "TOKEN_OR_OP";
    case TOKEN_AND_OP:
        return "TOKEN_AND_OP";
    case TOKEN_NOT_OP:
        return "TOKEN_NOT_OP";
    case TOKEN_OPEN_PAR:
        return "TOKEN_OPEN_PAR";
    case TOKEN_CLOSE_PAR:
        return "TOKEN_CLOSE_PAR";
    case TOKEN_OPEN_BRACE:
        return "TOKEN_OPEN_BRACE";
    case TOKEN_CLOSE_BRACE:
        return "TOKEN_CLOSE_BRACE";
    case TOKEN_IN_OP:
        return "TOKEN_IN_OP";
    case TOKEN_OUT_OP:
        return "TOKEN_OUT_OP";
    case TOKEN_COMMA:
        return "TOKEN_COMMA";
    case TOKEN_NEWLINE:
        return "TOKEN_NEWLINE";
    case TOKEN_ERROR:
        return "TOKEN_ERROR";

    default:
        return "UNKNOWN";
    }
}

// Strings stack definitions

Stack_S *stack_create_s()
{
    Stack_Node_S *top = (Stack_Node_S *)malloc(sizeof(*top));
    CHECK_MALLOC(top);

    top->prev = NULL;
    top->value = NULL;

    Stack_S *stack = (Stack_S *)malloc(sizeof(*stack));
    CHECK_MALLOC(stack);

    stack->top = top;
    stack->size = 0;

    return stack;
}

int stack_destroy_s(Stack_S *stack)
{
    if (!stack)
        return 1;
    Stack_Node_S *snode = stack->top;
    Stack_Node_S *prev_node;

    while (snode != NULL)
    {
        prev_node = snode->prev;
        free(snode);
        snode = prev_node;
    }

    return 0;
}

int stack_push_s(char *value, Stack_S *stack)
{
    if (!stack)
        return 1;
    if (stack->size == 0)
    {
        stack->top->value = value;
        stack->size = 1;
    }
    else
    {
        Stack_Node_S *new_top = (Stack_Node_S *)malloc(sizeof(new_top));
        CHECK_MALLOC(new_top);

        new_top->value = value;
        new_top->prev = stack->top;
        stack->top = new_top;
        stack->size++;
    }

    return 0;
}

char *stack_pop_s(Stack_S *stack)
{
    if (!stack || stack->size == 0)
        return (char *)NULL;

    Stack_Node_S *n = stack->top;
    char *rv = n->value;

    if (stack->size == 1)
    {
        stack->top->prev = NULL;
        stack->top->value = NULL;
    }
    else
    {
        stack->top = stack->top->prev;
        free(n);
    }
    stack->size--;

    return rv;
}

int print_stack_s(Stack_S *stack)
{
    if (!stack)
        return 1;
    Stack_Node_S *snode = stack->top;
    Stack_Node_S *prev_node;
    // int counter = stack->size;

    printf("[ ");
    for (int i = 0; i < stack->size; i++)
    {
        prev_node = snode->prev;
        // printf("(%d: %s) ", counter, snode->value);
        printf("%s ", snode->value);
        snode = prev_node;
        // counter--;
    }

    printf("]\n");

    return 0;
}

// Integers stack definitions

Stack_I *stack_create_i()
{
    Stack_Node_I *top = (Stack_Node_I *)malloc(sizeof(*top));
    CHECK_MALLOC(top);

    top->prev = NULL;
    top->value = 0;

    Stack_I *stack = (Stack_I *)malloc(sizeof(*stack));
    CHECK_MALLOC(stack);

    stack->top = top;
    stack->size = 0;

    return stack;
}

int stack_destroy_i(Stack_I *stack)
{
    if (!stack)
        return 1;
    Stack_Node_I *snode = stack->top;
    Stack_Node_I *prev_node;

    while (snode != NULL)
    {
        prev_node = snode->prev;
        free(snode);
        snode = prev_node;
    }

    return 0;
}

int stack_push_i(int value, Stack_I *stack)
{
    if (!stack)
        return 1;
    if (stack->size == 0)
    {
        stack->top->value = value;
        stack->size = 1;
    }
    else
    {
        Stack_Node_I *new_top = (Stack_Node_I *)malloc(sizeof(new_top));
        CHECK_MALLOC(new_top);

        new_top->value = value;
        new_top->prev = stack->top;
        stack->top = new_top;
        stack->size++;
    }

    return 0;
}

int stack_pop_i(Stack_I *stack)
{
    if (!stack || stack->size == 0)
        return 0;

    Stack_Node_I *n = stack->top;
    int rv = n->value;

    if (stack->size == 1)
    {
        stack->top->prev = NULL;
        stack->top->value = 0;
    }
    else
    {
        stack->top = stack->top->prev;
        free(n);
    }
    stack->size--;

    return rv;
}

int print_stack_i(Stack_I *stack)
{
    if (!stack)
        return 1;
    Stack_Node_I *snode = stack->top;
    Stack_Node_I *prev_node;

    printf("[ ");
    for (int i = 0; i < stack->size; i++)
    {
        prev_node = snode->prev;
        printf("%d ", snode->value);
        snode = prev_node;
    }

    printf("]\n");

    return 0;
}

// Standard stack definitions

Stack_t *stack_create()
{
    Stack_Node_t *top = (Stack_Node_t *)malloc(sizeof(*top));
    CHECK_MALLOC(top);

    top->prev = NULL;
    top->value = NULL;

    Stack_t *stack = (Stack_t *)malloc(sizeof(*stack));
    CHECK_MALLOC(stack);

    stack->top = top;
    stack->size = 0;

    return stack;
}

int stack_destroy(Stack_t *stack)
{
    if (!stack)
        return 1;

    Stack_Node_t *snode = stack->top;
    Stack_Node_t *prev_node;

    while (snode != NULL)
    {
        prev_node = snode->prev;
        free(snode);
        snode = prev_node;
    }

    return 0;
}

int stack_push(void *value, Stack_t *stack)
{
    if (!stack)
        return 1;
        
    if (stack->size == 0)
    {
        stack->top->value = value;
        stack->size = 1;
    }
    else
    {
        Stack_Node_t *new_top = (Stack_Node_t *)malloc(sizeof(*new_top));
        CHECK_MALLOC(new_top);

        new_top->value = value;
        new_top->prev = stack->top;
        stack->top = new_top;
        stack->size++;
    }

    return 0;
}

void *stack_pop(Stack_t *stack)
{
    if (!stack || stack->size == 0)
        return NULL;

    Stack_Node_t *n = stack->top;
    void *rv = n->value;

    if (stack->size == 1)
    {
        stack->top->prev = NULL;
        stack->top->value = 0;
    }
    else
    {
        stack->top = stack->top->prev;
        free(n);
    }
    stack->size--;

    return rv;
}

// Standard linked list definitions

List_t *list_create()
{
    List_Node_t *head = (List_Node_t *)malloc(sizeof(List_Node_t));
    List_Node_t *tail = (List_Node_t *)malloc(sizeof(List_Node_t));
    CHECK_MALLOC(head)
    CHECK_MALLOC(tail)
    head->next = NULL;
    head->prev = NULL;
    head->value = NULL;
    tail->next = NULL;
    tail->prev = NULL;
    tail->value = NULL;
    List_t *list = (List_t *)malloc(sizeof(List_t));
    CHECK_MALLOC(list)
    list->head = head;
    list->tail = tail;
    list->size = 0;
    return list;
}

int list_add(void *ptr, List_t *list)
{
    if (!list)
        return 1;

    if (list->size > 0)
    {
        List_Node_t *head = (List_Node_t *)malloc(sizeof(List_Node_t));
        CHECK_MALLOC(head)
        head->value = ptr;
        head->next = NULL;
        head->prev = list->head;
        list->head->next = head;
        list->head = head;
        if (list->size == 1)
            list->tail->next = list->head;
    }
    else
    {
        list->head->value = ptr;
        list->tail->value = ptr;
    }
    list->size++;
    return 0;
}

int list_destroy(List_t *list)
{
    if (!list)
        return 1;

    List_Node_t *node = list->head;
    List_Node_t *next_node;
    while (node)
    {
        next_node = node->prev;
        free(node);
        node = next_node;
    }

    free(list);
    return 0;
}

void print_list_i(List_t *list)
{
    List_Node_t *node = list->tail;
    printf("[ ");
    while (node)
    {
        printf("%d", *((int *)node->value));
        if (node->next)
            printf(",");
        printf(" ");
        node = node->next;
    }
    printf("]\n");
}

// AST definitions

AST_Node *create_ast_node(AST_Node_Type type)
{
    AST_Node *node = (AST_Node *)malloc(sizeof(*node));
    CHECK_MALLOC(node)
    node->type = type;
    node->first_stmnt  = NULL;
    node->next_stmnt   = NULL;
    node->ident        = NULL;
    node->expr         = NULL;
    node->first_arg    = NULL;
    node->next_arg     = NULL;
    node->first_param  = NULL;
    node->next_param   = NULL;
    node->left_side    = NULL;
    node->right_side   = NULL;
    node->operator     = NULL;
    node->operand      = NULL;
    node->expand       = NULL;
    // node->name         = "";
    node->value        = -1;
    return node;
}

AST_Node *create_AST_Program_Node(AST_Node *first_stmnt)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_PROGRAM);
    node->first_stmnt = first_stmnt;
    return node;
}

AST_Node *create_AST_Stmnt_Node(AST_Node *expand, AST_Node *next_stmnt)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_STMNT);
    node->expand = expand;
    node->next_stmnt = next_stmnt;
    return node;
}

AST_Node *create_AST_Assign_Node(AST_Node *ident, AST_Node *expr)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_ASSIGN);
    node->ident = ident;
    node->expr = expr;
    return node;
}

AST_Node *create_AST_Funcall_Node(AST_Node *ident, AST_Node *first_arg)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_FUNCALL);
    node->ident = ident;
    node->first_arg = first_arg;
    return node;
}

AST_Node *create_AST_Input_Node(AST_Node *ident)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_INPUT);
    node->ident = ident;
    return node;
}

AST_Node *create_AST_Output_Node(AST_Node *expr)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_OUTPUT);
    node->expr = expr;
    return node;
}

AST_Node *create_AST_Arg_Node(AST_Node *expr, AST_Node *next_arg)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_ARG);
    node->expr = expr;
    node->next_arg = next_arg;
    return node;
}

AST_Node *create_AST_Fundef_Node(AST_Node *first_param, AST_Node *first_stmnt)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_FUNDEF);
    node->first_param = first_param;
    node->first_stmnt = first_stmnt;
    return node;
}

AST_Node *create_AST_Param_Node(AST_Node *ident, AST_Node *next_param)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_PARAM);
    node->ident = ident;
    node->next_param = next_param;
    return node;
}

AST_Node *create_AST_Expr_Node(AST_Node *expand)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_EXPR);
    node->expand = expand;
    return node;
}

AST_Node *create_AST_LSide_Node(AST_Node *expand, AST_Node *operator)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_LSIDE);
    node->expand = expand;
    node->operator = operator;
    return node;
}

AST_Node *create_AST_RSide_Node(AST_Node *operator, AST_Node *operand)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_RSIDE);
    node->operator = operator;
    node->operand = operand;
    return node;
}

AST_Node *create_AST_Neg_Node(AST_Node *expr)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_NEG);
    node->expr = expr;
    return node;
}

AST_Node *create_AST_Disj_Node(AST_Node *left_side, AST_Node *right_side)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_DISJ);
    node->left_side = left_side;
    node->right_side = right_side;
    return node;
}

AST_Node *create_AST_Conj_Node(AST_Node *left_side, AST_Node *right_side)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_CONJ);
    node->left_side = left_side;
    node->right_side = right_side;
    return node;
}

AST_Node *create_AST_Ident_Node(char *name)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_IDENT);
    strcpy(node->name, name);
    return node;
}

AST_Node *create_AST_Bit_Node(int value)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_BIT);
    node->value = value;
    return node;
}

AST_Node *create_AST_Operator_Node(AST_Node_Type type)
{
    AST_Node *node = create_ast_node(AST_NODE_TYPE_OPERATOR);
    node->type = type;
    return node;
}

char *ast_node_typename(AST_Node_Type type)
{
    switch (type)
    {
    case AST_NODE_TYPE_PROGRAM:
        return "PROGRAM";
    case AST_NODE_TYPE_STMNT:
        return "STMNT";
    case AST_NODE_TYPE_ASSIGN:
        return "ASSIGN";
    case AST_NODE_TYPE_IDENT:
        return "IDENT";
    case AST_NODE_TYPE_BIT:
        return "BIT";
    case AST_NODE_TYPE_EXPR:
        return "EXPR";
    case AST_NODE_TYPE_LSIDE:
        return "LSIDE";
    case AST_NODE_TYPE_RSIDE:
        return "RSIDE";
    case AST_NODE_TYPE_NEG:
        return "NEG";
    case AST_NODE_TYPE_DISJ:
        return "DISJ";
    case AST_NODE_TYPE_CONJ:
        return "CONJ";
    case AST_NODE_TYPE_NOT:
        return "NOT";
    case AST_NODE_TYPE_OR:
        return "OR";
    case AST_NODE_TYPE_AND:
        return "AND";
    case AST_NODE_TYPE_FUNCALL:
        return "FUNCALL";
    case AST_NODE_TYPE_ARG:
        return "ARG";
    case AST_NODE_TYPE_FUNDEF:
        return "FUNDEF";
    case AST_NODE_TYPE_PARAM:
        return "PARAM";
    case AST_NODE_TYPE_INPUT:
        return "INPUT";
    case AST_NODE_TYPE_OUTPUT:
        return "OUTPUT";
    case AST_NODE_TYPE_OPERATOR:
        return "OPERATOR";
    default:
        return "UNKNOWN";
    }
}

void print_node_type(AST_Node_Type type)
{
    puts(ast_node_typename(type));
}

int n_indent = 0;

void print_ast(AST_Node *root)
{
    if (!root) return;
    for (int i=0; i<n_indent; i++) printf("      ");
    print_node_type(root->type);
    n_indent++;
    print_ast(root->ident);
    print_ast(root->expand);
    print_ast(root->expr);
    print_ast(root->first_arg);
    print_ast(root->next_arg);
    print_ast(root->first_param);
    print_ast(root->next_param);
    print_ast(root->first_stmnt);
    print_ast(root->next_stmnt);
    print_ast(root->left_side);
    print_ast(root->right_side);
    print_ast(root->operator);
    print_ast(root->operand);
    if (root->value != -1)
    {
        for (int i=0; i<n_indent; i++) printf("      ");
        printf("%d\n", root->value);
    }
    if (root->name != NULL)
    {
        for (int i=0; i<n_indent; i++) printf("      ");
        printf("%s\n", root->name);
    }
    n_indent--;
}

void destroy_ast_node(AST_Node *node)
{
    if (!node)
        return;
    
    destroy_ast_node(node->first_stmnt);
    destroy_ast_node(node->next_stmnt);
    destroy_ast_node(node->ident);
    destroy_ast_node(node->expr);
    destroy_ast_node(node->first_arg);
    destroy_ast_node(node->next_arg);
    destroy_ast_node(node->first_param);
    destroy_ast_node(node->next_param);
    destroy_ast_node(node->left_side);
    destroy_ast_node(node->right_side);
    destroy_ast_node(node->operator);
    destroy_ast_node(node->operand);
    destroy_ast_node(node->expand);

    free(node);
}

// AST Nodes stack definitions

int print_ast_stack(Stack_t *stack)
{
    if (!stack)
        return 1;
    Stack_Node_t *snode = stack->top;
    Stack_Node_t *prev_node;

    printf("[ ");
    for (int i = 0; i < stack->size; i++)
    {
        prev_node = snode->prev;
        if (snode->value)
            printf("%s ", ast_node_typename(((AST_Node *)snode->value)->type));
        else
            printf("NULL ");
        snode = prev_node;
    }

    printf("]\n");

    return 0;
}

// Map definitions

unsigned long hash(const char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}

Map_t *map_create()
{
    Map_t *map = (Map_t *)malloc(sizeof(Map_t));
    CHECK_MALLOC(map)

    map->pairs = list_create();
    map->size = 0;

    return map;
}

int map_add(const char *key, void *value, Map_t *map)
{
    if (!map)
        return 1;

    Map_Pair_t *pair = (Map_Pair_t *)malloc(sizeof(Map_Pair_t));
    CHECK_MALLOC(pair)
    
    strcpy(pair->key, key);
    pair->value = value;

    int state = list_add(pair, map->pairs);
    map->size++;

    return state;
}

int map_get(const char *key, void **buff, Map_t *map)
{
    if (!map)
        return 1;

    List_Node_t *node = map->pairs->head;
    while (node && node->value)
    {
        if (strcmp(((Map_Pair_t *)node->value)->key, key) == 0)
        {
            *buff = ((Map_Pair_t *)node->value)->value;
            return 0;
        }

        node = node->prev;
    }

    return 1;
}

int map_update(const char *key, void *value, Map_t *map)
{
    if (!map)
        return 1;

    List_Node_t *node = map->pairs->head;
    while (node)
    {
        if (strcmp(((Map_Pair_t *)node->value)->key, key) == 0)
        {
            ((Map_Pair_t *)node->value)->value = value;
            return 0;
        }

        node = node->prev;
    }

    return 1;
}

int map_destroy(Map_t *map)
{
    if (!map)
        return 1;

    int state = list_destroy(map->pairs);
    free(map);
    return state;
}
