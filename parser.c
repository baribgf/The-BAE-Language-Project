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
        switch (curr_token)
        {
        case TOKEN_EPSILON:
            debug &&printf("[DEBUG]: Matched terminal: EPSILON\n"); break;
        case TOKEN_EOF:
            debug &&printf("[DEBUG]: Matched terminal: EOF\n"); break;
        case TOKEN_NEWLINE:
            debug &&printf("[DEBUG]: Matched terminal: %s\n", "\\n");
        default:
            debug &&printf("[DEBUG]: Matched terminal: %s\n", yytext); break;
        }
        if (curr_token != TOKEN_EPSILON)
            advance_token();
    }

    return result;
}

int IO()
{
    int n_match = 0;
    DebugPreDerive("IO")
    proc = (match_terminal(TOKEN_IN_OP) && ++n_match) && Expr()
        || (match_terminal(TOKEN_OUT_OP) && ++n_match) && Expr();
    DebugPostDerive("IO")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
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
    return proc;
}

int Params()
{
    int n_match = 0;
    int match_eps = 0;
    DebugPreDerive("Params")
    proc = (match_terminal(TOKEN_IDENT) && ++n_match) && Params_Prime() || (match_eps = match_terminal(TOKEN_EPSILON));
    DebugPostDerive("Params")
    if (match_eps) for (int i = 0; i < n_match; i++) seekback_token();
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
    return proc;
}

int Expr_Prime()
{
    int n_match = 0;
    int lat = perform_lookahead();
    int fncla = lat == TOKEN_OPEN_PAR;
    DebugPreDerive("Expr'")
    proc = Fundef()
        || fncla && Funcall()
        || (match_terminal(TOKEN_OPEN_PAR) && ++n_match) && Expr() && (match_terminal(TOKEN_CLOSE_PAR) && ++n_match)
        || (match_terminal(TOKEN_NOT_OP) && ++n_match) && Expr()
        || (match_terminal(TOKEN_IDENT) && ++n_match)
        || (match_terminal(TOKEN_BIT) && ++n_match);
    DebugPostDerive("Expr'")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    return proc;
}

int Expr()
{
    int n_match = 0;
    DebugPreDerive("Expr")
    proc = Expr_Prime() && Expr_Prime_2();
    DebugPostDerive("Expr")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
    return proc;
}

int Assign()
{
    int n_match = 0;
    DebugPreDerive("Assign")
    proc = (match_terminal(TOKEN_IDENT) && ++n_match) && (match_terminal(TOKEN_ASSIGN_OP) && ++n_match) && Expr();
    DebugPostDerive("Assign")
    if (!proc) for (int i = 0; i < n_match; i++) seekback_token();
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
        push_stack_i(Stmnt_State, State_Stack);
    }
    else
    {
        proc = (match_terminal(TOKEN_NEWLINE) && ++n_match) && Program()
            || (match_terminal(TOKEN_EOF) && ++n_match)
            || (match_eps = match_terminal(TOKEN_EPSILON));
    }

    DebugPostDerive("Program")
    if (match_eps) for (int i = 0; i < n_match; i++) seekback_token();
    return proc;
}
