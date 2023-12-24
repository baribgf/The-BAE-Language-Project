#include "headers/parser.h"
#include "headers/common.h"
#include "headers/tokens.h"

char *filename;
int token_pos, token_count, proc, blank_source;
TOKEN token;
Stack_S *RDStack;
Stack_I *State_Stack;
Stack_ASTN *AST_Stack;

int main(int argc, char *argv[])
{
    CHECK_INPUT_FILE

    filename = argv[1];
    INITIALIZE_INPUT_FILE

    token_count = 1;
    while ((token = yylex()) != TOKEN_EOF)
    {
        if (token == TOKEN_ERROR)
        {
            fprintf(stderr, "Undefined token: '%s'\n", yytext);
            _exit(1);
        }

        token_count++;

        // if (token == TOKEN_NEWLINE) yytext = "\\n";
        // printf("TOKEN: %d -> '%s'\n", token, yytext);
    }

    puts("> Lexical Analysis completed with success.");

    yylex_destroy();
    INITIALIZE_INPUT_FILE
    token = yylex();
    token_pos = 0;
    RDStack = create_stack_s();
    State_Stack = create_stack_i();
    AST_Stack = create_stack_astn();

    TOKEN lat = perform_lookahead();
    blank_source = (lat == TOKEN_EOF) ? 1 : 0; // Must first erase all leading whitespaces !

    int state = Program();

    AST_Node *root_node = pop_stack_astn(AST_Stack);
    print_ast(root_node);

    if (!blank_source)
    {
        Stack_Node_I *node = State_Stack->top;
        while (node)
        {
            if (node->value == 0)
            {
                state = 0;
                break;
            }
            node = node->prev;
        }
    }

    destroy_stack_s(RDStack);
    destroy_stack_i(State_Stack);

    if (token_pos >= token_count && state)
    {
        puts("> Parsing completed with success.");
    }
    else if (token_pos < token_count)
    {
        puts("* Parsing did not complete.");
        printf("token_pos = %d, token_count = %d\n", token_pos, token_count);
        return 1;
    }
    else
    {
        puts("* Parsing completed without success.");
        return 1;
    }

    return 0;
}
