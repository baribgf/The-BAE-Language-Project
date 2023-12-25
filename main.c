#include "headers/parser.h"
#include "headers/common.h"
#include "headers/tokens.h"

char *filename;
int token_pos, proc;
TOKEN token;
List_t *Tokens_List;
Stack_S *RDStack;               // Recursive Descent Stack for Debugging !
Stack_t *AST_Stack;
AST_Node *ast_root_node;

int main(int argc, char *argv[])
{
    CHECK_INPUT_FILE

    filename = argv[1];
    INITIALIZE_INPUT_FILE
    // Lexical Analysis Phase ///////////////////////////////////////
    Tokens_List = create_list();
    while ((token = yylex()) != TOKEN_EOF)
    {
        if (token == TOKEN_ERROR)
        {
            fprintf(stderr, "Undefined token: '%s'\n", yytext);
            exit(1);
        }

        int *token_ptr = (int *)malloc(sizeof(int));
        *token_ptr = token;
        add_list(token_ptr, Tokens_List);

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
    AST_Stack = create_stack();
    int state = Program();
    destroy_stack_s(RDStack);

    if (state)
    {
        puts("[>] Parsing completed with success.");
    }
    else
    {
        puts("[!] Parsing did not complete.");
        return 1;
    }

    ast_root_node = pop_stack(AST_Stack);
    destroy_stack(AST_Stack);

    // Program Interpretation Phase /////////////////////////////////
    // print_ast(ast_root_node);


    return 0;
}
