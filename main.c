#include "headers/parser.h"
#include "headers/common.h"
#include "headers/tokens.h"

char *filename;
int token_pos, token_count, proc = 0, blank_source;
TOKEN token;
Stack_S *RDStack;               // Recursive Descent Stack for Debugging !
Stack_I *State_Stack;
Stack_t *AST_Stack;
AST_Node *ast_root_node;

int main(int argc, char *argv[])
{
    CHECK_INPUT_FILE

    filename = argv[1];
    INITIALIZE_INPUT_FILE

    // Lexical Analysis Phase ///////////////////////////////////////
    token_count = 1;
    while ((token = yylex()) != TOKEN_EOF)
    {
        if (token == TOKEN_ERROR)
        {
            fprintf(stderr, "Undefined token: '%s'\n", yytext);
            exit(1);
        }

        token_count++;
        // if (token == TOKEN_NEWLINE) yytext = "\\n";
        // else if (token == TOKEN_EOF) yytext = "EOF";
        // printf("TOKEN: %d -> '%s'\n", token, yytext);
    }

    puts("[>] Lexical Analysis completed with success.");

    yylex_destroy();
    INITIALIZE_INPUT_FILE

    // Parsing Phase ////////////////////////////////////////////////
    token = yylex();
    token_pos = 0;
    RDStack = create_stack_s();
    State_Stack = create_stack_i();
    AST_Stack = create_stack();
    int state = Program();

    destroy_stack_s(RDStack);
    destroy_stack_i(State_Stack);

    if (token_pos >= token_count && state)
    {
        puts("[>] Parsing completed with success.");
    }
    else if (token_pos < token_count)
    {
        puts("[!] Parsing did not complete.");
        return 1;
    }
    else
    {
        puts("[!] Parsing completed without success (Something went wrong).");
        return 1;
    }

    ast_root_node = pop_stack(AST_Stack);
    destroy_stack(AST_Stack);

    // print_ast(ast_root_node);

    return 0;
}
