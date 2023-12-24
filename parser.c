#include "headers/parser.h"

int debug = 0;

void seekback_token()
{
    yylex_destroy();
    INITIALIZE_INPUT_FILE
    for (int i = 0; i < token_pos; i++)
        token = yylex();
    token_pos--;
}

void advance_token()
{
    token = yylex();
    token_pos++;
}

TOKEN perform_lookahead()
{
    TOKEN la_token;
    advance_token();
    la_token = token;
    seekback_token();
    return la_token;
}

int match_terminal(TOKEN curr_token)
{
    int result = (token == curr_token) || (curr_token == TOKEN_EPSILON);
    if (result)
    {
        if (debug)
        {
            switch (curr_token)
            {
            case TOKEN_EPSILON:
                printf("[DEBUG]: Matched terminal: EPSILON\n"); break;
            case TOKEN_EOF:
                printf("[DEBUG]: Matched terminal: EOF\n"); break;
            case TOKEN_NEWLINE:
                printf("[DEBUG]: Matched terminal: %s\n", "\\n");
            default:
                printf("[DEBUG]: Matched terminal: %s\n", yytext); break;
            }
        }

        AST_Node *node = NULL;
        switch (curr_token)
        {
            case TOKEN_BIT:
                node = create_AST_Bit_Node(atoi(yytext));
                break;
            case TOKEN_IDENT:
                node = create_AST_Ident_Node(yytext);
                strcpy(node->name, yytext);
                break;
            case TOKEN_OR_OP:
                node = create_AST_Operator_Node(AST_NODE_TYPE_OR);
                break;
            case TOKEN_AND_OP:
                node = create_AST_Operator_Node(AST_NODE_TYPE_AND);
                break;
            case TOKEN_NOT_OP:
                node = create_AST_Operator_Node(AST_NODE_TYPE_NOT);
                break;
            case TOKEN_OPEN_PAR:
                break;
            case TOKEN_CLOSE_PAR:
                break;
            case TOKEN_OPEN_TAG:
                break;
            case TOKEN_CLOSE_TAG:
                break;
            case TOKEN_OPEN_BRACE:
                break;
            case TOKEN_CLOSE_BRACE:
                break;
            case TOKEN_IN_OP:
                break;
            case TOKEN_OUT_OP:
                break;
            case TOKEN_COMMA:
                break;
            case TOKEN_NEWLINE:
                break;
            case TOKEN_EPSILON:
                break;
        }

        push_stack_astn(node, AST_Stack);

        if (curr_token != TOKEN_EPSILON)
            advance_token();
    }

    return result;
}

int IO()
{
    int n_match = 0;
    int input = 0, output = 0;
    DebugPreDerive("IO")
    proc = (input = (match_terminal(TOKEN_IN_OP) && ++n_match) && match_terminal(TOKEN_IDENT))
        || (output = (match_terminal(TOKEN_OUT_OP) && ++n_match) && Expr());
    DebugPostDerive("IO")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    if (input)
    {
        AST_Node *ident = pop_stack_astn(AST_Stack),
                 *in_op = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Input_Node(ident), AST_Stack);
    }
    else if (output)
    {
        AST_Node *expr = pop_stack_astn(AST_Stack),
                 *out_op = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Output_Node(expr), AST_Stack);
    }
    return proc;
}

int Params_Prime()
{
    int n_match = 0;
    int match_eps = 0;
    DebugPreDerive("Params'")
    proc = (match_terminal(TOKEN_COMMA) && ++n_match) && (match_terminal(TOKEN_IDENT) && ++n_match) && Params_Prime()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    DebugPostDerive("Params'")
    if (match_eps) for (int i = 0; i < n_match; i++) seekback_token();
    if (match_eps)
    {
        pop_stack_astn(AST_Stack);
        push_stack_astn(create_AST_Param_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *next_param = pop_stack_astn(AST_Stack),
                 *ident = pop_stack_astn(AST_Stack),
                 *comma = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Param_Node(ident, next_param), AST_Stack);
    }
    return proc;
}

int Params()
{
    int n_match = 0;
    int match_eps = 0;
    DebugPreDerive("Params")
    proc = (match_terminal(TOKEN_IDENT) && ++n_match) && Params_Prime()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    DebugPostDerive("Params")
    if (match_eps) for (int i = 0; i < n_match; i++) seekback_token();
    if (match_eps)
    {
        pop_stack_astn(AST_Stack);
        push_stack_astn(create_AST_Param_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *next_param = pop_stack_astn(AST_Stack),
                 *ident = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Param_Node(ident, next_param), AST_Stack);
    }
    return proc;
}

int Fundef()
{
    int n_match = 0;
    DebugPreDerive("Fundef")
    proc = (match_terminal(TOKEN_OPEN_TAG) && ++n_match)
        && Params()
        && (match_terminal(TOKEN_CLOSE_TAG) && ++n_match)
        && (match_terminal(TOKEN_OPEN_BRACE) && ++n_match)
        && Program()
        && (match_terminal(TOKEN_CLOSE_BRACE) && ++n_match);
    DebugPostDerive("Fundef")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    else
    {
        AST_Node *close_brace = pop_stack_astn(AST_Stack),
                 *first_stmnt = pop_stack_astn(AST_Stack),
                 *open_brace = pop_stack_astn(AST_Stack),
                 *close_tag = pop_stack_astn(AST_Stack),
                 *first_param = pop_stack_astn(AST_Stack),
                 *open_tag = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Fundef_Node(first_param, first_stmnt), AST_Stack);
    }
    return proc;
}

int Args_Prime()
{
    int n_match = 0;
    int match_eps = 0;
    DebugPreDerive("Args'")
    proc = (match_terminal(TOKEN_COMMA) && ++n_match)
            && Expr()
            && Args_Prime()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    DebugPostDerive("Args'")
    if (match_eps) for (int i = 0; i < n_match; i++) seekback_token();
    if (match_eps)
    {
        pop_stack_astn(AST_Stack);
        push_stack_astn(create_AST_Arg_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *next_arg = pop_stack_astn(AST_Stack),
                 *expr = pop_stack_astn(AST_Stack),
                 *comma = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Arg_Node(expr, next_arg), AST_Stack);
    }
    return proc;
}

int Args()
{
    int n_match = 0;
    int match_eps = 0;
    DebugPreDerive("Args")
    proc = Expr() && Args_Prime()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    DebugPostDerive("Args")
    if (match_eps) for (int i = 0; i < n_match; i++) seekback_token();
    if (match_eps)
    {
        pop_stack_astn(AST_Stack);
        push_stack_astn(create_AST_Arg_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *next_arg = pop_stack_astn(AST_Stack),
                 *expr = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Arg_Node(expr, next_arg), AST_Stack);
    }
    return proc;
}

int Funcall()
{
    int n_match = 0;
    DebugPreDerive("Funcall")
    proc = (match_terminal(TOKEN_IDENT) && ++n_match)
        && (match_terminal(TOKEN_OPEN_PAR) && ++n_match)
        && Args()
        && (match_terminal(TOKEN_CLOSE_PAR) && ++n_match);
    DebugPostDerive("Funcall")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    else
    {
        AST_Node *close_par = pop_stack_astn(AST_Stack),
                 *first_arg = pop_stack_astn(AST_Stack),
                 *open_par = pop_stack_astn(AST_Stack),
                 *ident = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Funcall_Node(ident, first_arg), AST_Stack);
    }
    return proc;
}

int Expr_Prime_2()
{
    int n_match = 0;
    int match_eps = 0;
    DebugPreDerive("Expr''")
    proc = (match_terminal(TOKEN_OR_OP) && ++n_match) && Expr()
        || (match_terminal(TOKEN_AND_OP) && ++n_match) && Expr()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    DebugPostDerive("Expr''")
    if (match_eps) for (int i = 0; i < n_match; i++) seekback_token();
    if (match_eps)
    {
        pop_stack_astn(AST_Stack);
        push_stack_astn(create_AST_RSide_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *operand = pop_stack_astn(AST_Stack);
        AST_Node *operator = pop_stack_astn(AST_Stack);
        push_stack_astn(create_AST_RSide_Node(operator, operand), AST_Stack);
    }
    return proc;
}

int Expr_Prime()
{
    int n_match = 0;
    int lat = perform_lookahead();
    int fncla = lat == TOKEN_OPEN_PAR;
    int funcall = 0; int bit = 0; int ident = 0; int neg = 0; int expr_par = 0;
    DebugPreDerive("Expr'")
    proc = Fundef()
        || (funcall = (fncla && Funcall()))
        || (expr_par = ((match_terminal(TOKEN_OPEN_PAR) && ++n_match) && Expr() && (match_terminal(TOKEN_CLOSE_PAR) && ++n_match)))
        || (neg = ((match_terminal(TOKEN_NOT_OP) && ++n_match) && Expr_Prime()))
        || (ident = (match_terminal(TOKEN_IDENT) && ++n_match))
        || (bit = (match_terminal(TOKEN_BIT) && ++n_match));
    DebugPostDerive("Expr'")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    else
    {
        if (expr_par)
        {
            AST_Node *close_par = pop_stack_astn(AST_Stack),
                     *expr = pop_stack_astn(AST_Stack),
                     *open_par = pop_stack_astn(AST_Stack);

            push_stack_astn(expr, AST_Stack);
        }
        else if (neg)
        {
            AST_Node *expr = pop_stack_astn(AST_Stack),
                     *neg_op = pop_stack_astn(AST_Stack);

            push_stack_astn(create_AST_Neg_Node(expr), AST_Stack);
        }
        // else if (funcall)
        // {
        //     AST_Node *funcll = pop_stack_astn(AST_Stack);
        // }
    }
    return proc;
}

int Expr()
{
    int n_match = 0;
    DebugPreDerive("Expr")
    proc = Expr_Prime() && Expr_Prime_2();
    DebugPostDerive("Expr")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    else
    {
        AST_Node *rside = pop_stack_astn(AST_Stack),
                 *lside = pop_stack_astn(AST_Stack);

        if (rside->operator == NULL)
            push_stack_astn(lside, AST_Stack);
        else
        {
            if (rside->operator->type == AST_NODE_TYPE_OR)
            {
                push_stack_astn(create_AST_Disj_Node(lside, rside->operand), AST_Stack);
            }
            else if (rside->operator->type == AST_NODE_TYPE_AND)
            {
                push_stack_astn(create_AST_Conj_Node(lside, rside->operand), AST_Stack);
            }
        }
    }
    return proc;
}

int Assign()
{
    int n_match = 0;
    DebugPreDerive("Assign")
    proc = (match_terminal(TOKEN_IDENT) && ++n_match) && (match_terminal(TOKEN_ASSIGN_OP) && ++n_match) && Expr();
    DebugPostDerive("Assign")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    else
    {
        AST_Node *expr = pop_stack_astn(AST_Stack),
                 *assign_op = pop_stack_astn(AST_Stack),
                 *ident = pop_stack_astn(AST_Stack);

        push_stack_astn(create_AST_Assign_Node(ident, expr), AST_Stack);
    }
    return proc;
}

int Stmnt()
{
    int n_match = 0;
    int lookahead = 0;
    if (perform_lookahead() == TOKEN_ASSIGN_OP)
        lookahead = 1;

    DebugPreDerive("Stmnt")
    proc = lookahead && Assign() || Funcall() || IO();
    DebugPostDerive("Stmnt")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    else
    {
        AST_Node *expand = pop_stack_astn(AST_Stack);
        push_stack_astn(create_AST_Stmnt_Node(expand, NULL), AST_Stack);
    }
    return proc;
}

int Program()
{
    int n_match = 0;
    int match_eps = 0;
    DebugPreDerive("Program")
    int next_stmnt = (token == TOKEN_IDENT || token == TOKEN_IN_OP || token == TOKEN_OUT_OP) ? 1 : 0;
    if (next_stmnt)
    {
        int Stmnt_State;
        proc = (Stmnt_State = Stmnt()) && Program();
        if (proc)
        {
            AST_Node *program = pop_stack_astn(AST_Stack),
                     *first_stmnt = pop_stack_astn(AST_Stack);
            
            if (program)
                first_stmnt->next_stmnt = program->first_stmnt;

            push_stack_astn(create_AST_Program_Node(first_stmnt), AST_Stack);
        }

        push_stack_i(Stmnt_State, State_Stack);
    }
    else
    {
        int eof = 0, newl_prog = 0;
        proc = (newl_prog = ((match_terminal(TOKEN_NEWLINE) && ++n_match) && Program()))
            || (eof = (match_terminal(TOKEN_EOF) && ++n_match))
            || (match_eps = match_terminal(TOKEN_EPSILON));

        if (eof)
        {
            pop_stack_astn(AST_Stack);  // pop EOF node
            push_stack_astn(create_AST_Program_Node(NULL), AST_Stack);
        }
        else if (newl_prog)
        {
            AST_Node *program = pop_stack_astn(AST_Stack),
                     *newl = pop_stack_astn(AST_Stack);

            push_stack_astn(create_AST_Program_Node(program->first_stmnt), AST_Stack);
        }
    }

    DebugPostDerive("Program")
    if (match_eps) for (int i = 0; i < n_match; i++) seekback_token();
    if (match_eps)
    {
        pop_stack_astn(AST_Stack);
        push_stack_astn(create_AST_Program_Node(NULL), AST_Stack);
    }
    return proc;
}
