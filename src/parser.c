#include "../headers/parser.h"

int debug = 0;

void seekback_token()
{
    if (token_ptr->prev)
        token_ptr = token_ptr->prev;
    token = ((Token_Pair_t *)token_ptr->value)->value;
    yytext = ((Token_Pair_t *)token_ptr->value)->text;
    token_pos--;
}

void advance_token()
{
    if (token_ptr->next)
        token_ptr = token_ptr->next;
    token = ((Token_Pair_t *)token_ptr->value)->value;
    yytext = ((Token_Pair_t *)token_ptr->value)->text;
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
                printf("[DEBUG]: Matched terminal: %s\n", "\\n"); break;
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
            case TOKEN_NEWLINE:
                break;
            case TOKEN_EPSILON:
                break;
        }

        push_stack(node, AST_Stack);

        if (curr_token != TOKEN_EPSILON)
            advance_token();
    }

    return result;
}

int IO()
{
    DebugPreDerive("IO")
    int n_match = token_pos;
    int input = 0, output = 0;
    proc = (input = (match_terminal(TOKEN_IN_OP)) && match_terminal(TOKEN_IDENT))
        || (output = (match_terminal(TOKEN_OUT_OP)) && Expr());
    if (!proc) while (token_pos > n_match) seekback_token();

    if (proc)
    {
        if (input)
        {
            AST_Node *ident = pop_stack(AST_Stack),
                    *in_op = pop_stack(AST_Stack);

            push_stack(create_AST_Input_Node(ident), AST_Stack);
        }
        else if (output)
        {
            AST_Node *expr = pop_stack(AST_Stack),
                    *out_op = pop_stack(AST_Stack);

            push_stack(create_AST_Output_Node(expr), AST_Stack);
        }
    }

    DebugPostDerive("IO")
    return proc;
}

int InnerProgram()
{
    DebugPreDerive("InnerProgram")
    int n_match = token_pos;
    int Stmnt_State = 0;
    int next_is_stmnt = (token == TOKEN_IDENT || token == TOKEN_IN_OP || token == TOKEN_OUT_OP) ? 1 : 0;
    if (next_is_stmnt)
    {
        proc = (Stmnt_State = Stmnt()) && InnerProgram();
        if (proc)
        {
            if (Stmnt_State)
            {
                AST_Node *program = pop_stack(AST_Stack),
                        *first_stmnt = pop_stack(AST_Stack);
            
                if (program)
                    first_stmnt->next_stmnt = program->first_stmnt;

                push_stack(create_AST_Program_Node(first_stmnt), AST_Stack);
            }
        }
    }
    else
    {
        int newl_prog = 0, match_eps = 0;
        proc = ((newl_prog = (match_terminal(TOKEN_NEWLINE)) && InnerProgram()))
            || (match_eps = match_terminal(TOKEN_EPSILON));

        if (match_eps) while (token_pos > n_match) seekback_token();
        if (match_eps)
        {
            pop_stack(AST_Stack);
            push_stack(create_AST_Program_Node(NULL), AST_Stack);
        }
        else
        {
            if (newl_prog)
            {
                AST_Node *program = pop_stack(AST_Stack),
                         *newl = pop_stack(AST_Stack);
                if (program)
                    push_stack(create_AST_Program_Node(program->first_stmnt), AST_Stack);
            }
        }
    }
    
    DebugPostDerive("InnerProgram")
    return proc;
}

int Params_Prime()
{
    DebugPreDerive("Params'")
    int n_match = token_pos;
    int match_eps = 0;    
    proc = (match_terminal(TOKEN_COMMA)) && (match_terminal(TOKEN_IDENT)) && Params_Prime()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    if (match_eps) while (token_pos > n_match) seekback_token();
    if (match_eps)
    {
        pop_stack(AST_Stack);
        push_stack(create_AST_Param_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *next_param = pop_stack(AST_Stack),
                 *ident = pop_stack(AST_Stack),
                 *comma = pop_stack(AST_Stack);

        push_stack(create_AST_Param_Node(ident, next_param), AST_Stack);
    }
    DebugPostDerive("Params'")
    return proc;
}

int Params()
{
    DebugPreDerive("Params")
    int n_match = token_pos;
    int match_eps = 0;
    proc = (match_terminal(TOKEN_IDENT)) && Params_Prime()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    if (match_eps) while (token_pos > n_match) seekback_token();
    if (match_eps)
    {
        pop_stack(AST_Stack);
        push_stack(create_AST_Param_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *next_param = pop_stack(AST_Stack),
                 *ident = pop_stack(AST_Stack);

        push_stack(create_AST_Param_Node(ident, next_param), AST_Stack);
    }
    DebugPostDerive("Params")
    return proc;
}

int Fundef()
{
    DebugPreDerive("Fundef")
    int n_match = token_pos;
    proc = match_terminal(TOKEN_OPEN_TAG)
        && Params()
        && match_terminal(TOKEN_CLOSE_TAG)
        && match_terminal(TOKEN_OPEN_BRACE)
        && InnerProgram()
        && match_terminal(TOKEN_CLOSE_BRACE);
    if (!proc) while (token_pos > n_match) seekback_token();
    else
    {
        AST_Node *close_brace = pop_stack(AST_Stack),
                 *first_stmnt = pop_stack(AST_Stack),
                 *open_brace = pop_stack(AST_Stack),
                 *close_tag = pop_stack(AST_Stack),
                 *first_param = pop_stack(AST_Stack),
                 *open_tag = pop_stack(AST_Stack);

        push_stack(create_AST_Fundef_Node(first_param, first_stmnt), AST_Stack);
    }
    DebugPostDerive("Fundef")
    return proc;
}

int Args_Prime()
{
    DebugPreDerive("Args'")
    int n_match = token_pos;
    int match_eps = 0;
    proc = (match_terminal(TOKEN_COMMA))
            && Expr()
            && Args_Prime()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    if (match_eps) while (token_pos > n_match) seekback_token();
    if (match_eps)
    {
        pop_stack(AST_Stack);
        push_stack(create_AST_Arg_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *next_arg = pop_stack(AST_Stack),
                 *expr = pop_stack(AST_Stack),
                 *comma = pop_stack(AST_Stack);

        push_stack(create_AST_Arg_Node(expr, next_arg), AST_Stack);
    }
    DebugPostDerive("Args'")
    return proc;
}

int Args()
{
    DebugPreDerive("Args")
    int n_match = token_pos;
    int match_eps = 0;
    proc = Expr() && Args_Prime()
        || (match_eps = match_terminal(TOKEN_EPSILON));
    if (match_eps) while (token_pos > n_match) seekback_token();
    if (match_eps)
    {
        pop_stack(AST_Stack);
        push_stack(create_AST_Arg_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *next_arg = pop_stack(AST_Stack),
                 *expr = pop_stack(AST_Stack);

        push_stack(create_AST_Arg_Node(expr, next_arg), AST_Stack);
    }
    DebugPostDerive("Args")
    return proc;
}

int Funcall()
{
    DebugPreDerive("Funcall")
    int n_match = token_pos;    
    proc = match_terminal(TOKEN_IDENT)
        && match_terminal(TOKEN_OPEN_PAR)
        && Args()
        && match_terminal(TOKEN_CLOSE_PAR);
    if (!proc) while (token_pos > n_match) seekback_token();
    else
    {
        AST_Node *close_par = pop_stack(AST_Stack),
                 *first_arg = pop_stack(AST_Stack),
                 *open_par = pop_stack(AST_Stack),
                 *ident = pop_stack(AST_Stack);

        push_stack(create_AST_Funcall_Node(ident, first_arg), AST_Stack);
    }
    DebugPostDerive("Funcall")
    return proc;
}

int Expr_Prime_2()
{
    DebugPreDerive("Expr''")
    int n_match = token_pos;
    int match_eps = 0;
    int match_or = 0, match_and = 0, match_expr = 0;
    proc = (match_or = match_terminal(TOKEN_OR_OP)) && (match_expr = Expr())
        || (match_and = match_terminal(TOKEN_AND_OP)) && (match_expr = Expr())
        || (match_eps = match_terminal(TOKEN_EPSILON));
    if ((match_or || match_and) && !match_expr)
        proc = 0;
    if (match_eps) while (token_pos > n_match) seekback_token();
    if (match_eps)
    {
        pop_stack(AST_Stack);
        push_stack(create_AST_RSide_Node(NULL, NULL), AST_Stack);
    }
    else
    {
        AST_Node *operand = pop_stack(AST_Stack),
                 *operator = pop_stack(AST_Stack);
        push_stack(create_AST_RSide_Node(operator, operand), AST_Stack);
    }
    DebugPostDerive("Expr''")
    return proc;
}

int Expr_Prime()
{
    DebugPreDerive("Expr'")
    int n_match = token_pos;
    int lat = perform_lookahead();
    int fncla = lat == TOKEN_OPEN_PAR;
    int funcall = 0; int bit = 0; int ident = 0; int neg = 0; int expr_par = 0;
    proc = (funcall = (fncla && Funcall()))
        || (expr_par = ((match_terminal(TOKEN_OPEN_PAR)) && Expr() && (match_terminal(TOKEN_CLOSE_PAR))))
        || (neg = ((match_terminal(TOKEN_NOT_OP)) && Expr_Prime()))
        || (ident = (match_terminal(TOKEN_IDENT)))
        || (bit = (match_terminal(TOKEN_BIT)));
    if (!proc) while (token_pos > n_match) seekback_token();
    else
    {
        if (expr_par)
        {
            AST_Node *close_par = pop_stack(AST_Stack),
                     *expr = pop_stack(AST_Stack),
                     *open_par = pop_stack(AST_Stack);

            push_stack(expr, AST_Stack);
        }
        else if (neg)
        {
            AST_Node *expr = pop_stack(AST_Stack),
                     *neg_op = pop_stack(AST_Stack);

            push_stack(create_AST_Neg_Node(expr), AST_Stack);
        }
        // else if (funcall)
        // {
        //     AST_Node *funcll = pop_stack(AST_Stack);
        // }
    }
    DebugPostDerive("Expr'")
    return proc;
}

int Expr()
{
    DebugPreDerive("Expr")
    int n_match = token_pos;
    proc = Expr_Prime() && Expr_Prime_2();
    if (!proc) while (token_pos > n_match) seekback_token();
    else
    {
        AST_Node *rside = pop_stack(AST_Stack),
                 *lside = pop_stack(AST_Stack);

        if (rside->operator == NULL)
            push_stack(lside, AST_Stack);
        else
        {
            if (rside->operator->type == AST_NODE_TYPE_OR)
            {
                push_stack(create_AST_Disj_Node(lside, rside->operand), AST_Stack);
            }
            else if (rside->operator->type == AST_NODE_TYPE_AND)
            {
                push_stack(create_AST_Conj_Node(lside, rside->operand), AST_Stack);
            }
        }
    }
    DebugPostDerive("Expr")
    return proc;
}

int Assign_Prime()
{
    DebugPreDerive("Assign'")
    int n_match = token_pos;
    proc = Expr() || Fundef();
    if (!proc) while (token_pos > n_match) seekback_token();
    DebugPostDerive("Assign'")
    return proc;
}

int Assign()
{
    DebugPreDerive("Assign")
    int n_match = token_pos;
    proc = (match_terminal(TOKEN_IDENT)) && (match_terminal(TOKEN_ASSIGN_OP)) && Assign_Prime();
    if (!proc) while (token_pos > n_match) seekback_token();
    else
    {
        AST_Node *assign_prime = pop_stack(AST_Stack),
                 *assign_op = pop_stack(AST_Stack),
                 *ident = pop_stack(AST_Stack);

        push_stack(create_AST_Assign_Node(ident, assign_prime), AST_Stack);
    }
    DebugPostDerive("Assign")
    return proc;
}

int Stmnt()
{
    DebugPreDerive("Stmnt")
    int n_match = token_pos;
    int lookahead = 0;
    if (perform_lookahead() == TOKEN_ASSIGN_OP)
        lookahead = 1;

    
    proc = lookahead && Assign() || Funcall() || IO();
    if (!proc) while (token_pos > n_match) seekback_token();
    else
    {
        AST_Node *expand = pop_stack(AST_Stack);
        push_stack(create_AST_Stmnt_Node(expand, NULL), AST_Stack);
    }
    DebugPostDerive("Stmnt")
    return proc;
}

int Program()
{
    DebugPreDerive("Program")
    int n_match = token_pos;
    int next_is_stmnt = (token == TOKEN_IDENT || token == TOKEN_IN_OP || token == TOKEN_OUT_OP) ? 1 : 0;
    if (next_is_stmnt)
    {
        int Stmnt_State;
        proc = (Stmnt_State = Stmnt()) && Program();
        if (!proc) while (token_pos > n_match) seekback_token();
        if (proc)
        {
            AST_Node *program = pop_stack(AST_Stack),
                     *first_stmnt = pop_stack(AST_Stack);
            
            if (program && first_stmnt)
                first_stmnt->next_stmnt = program->first_stmnt;

            push_stack(create_AST_Program_Node(first_stmnt), AST_Stack);
        }
    }
    else
    {
        int eof = 0, newl_prog = 0;
        proc = (newl_prog = ((match_terminal(TOKEN_NEWLINE)) && Program()))
            || (eof = (match_terminal(TOKEN_EOF)));

        if (eof)
        {
            pop_stack(AST_Stack);  // pop EOF node
            push_stack(create_AST_Program_Node(NULL), AST_Stack);
        }
        else if (newl_prog)
        {
            AST_Node *program = pop_stack(AST_Stack),
                     *newl = pop_stack(AST_Stack);

            push_stack(create_AST_Program_Node(program->first_stmnt), AST_Stack);
        }
    }

    DebugPostDerive("Program")
    return proc;
}
