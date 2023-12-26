#include "../headers/tokens.h"
#include "../headers/common.h"
#include "../headers/parser.h"
#include "../headers/interpreter.h"

char *filename;
int token_pos, proc;
List_Node_t *token_ptr;
TOKEN token;
List_t *Tokens_List;
Stack_S *RDStack;               // Recursive Descent Stack for Debugging !
Stack_t *AST_Stack;
AST_Node *ast_root_node;
void *it_value;
void *expr_return;

int main(int argc, char *argv[])
{
    CHECK_INPUT_FILE

    filename = argv[1];
    INITIALIZE_INPUT_FILE

    // Lexical Analysis Phase ///////////////////////////////////////
    Tokens_List = list_create();
    int last_is_newl = 0;
    do
    {
        token = yylex();
        if (token == TOKEN_ERROR)
        {
            fprintf(stderr, "Undefined token: '%s'\n", yytext);
            exit(1);
        }
        else if (token == TOKEN_NEWLINE)
        {
            if (!last_is_newl)
                last_is_newl = 1;
            else
                continue;
        }
        else
        {
            last_is_newl = 0;
        }

        Token_Pair_t *token_ptr = (Token_Pair_t *)malloc(sizeof(Token_Pair_t));
        token_ptr->value = token;
        strcpy(token_ptr->text, yytext);
        list_add(token_ptr, Tokens_List);

        // if (token == TOKEN_NEWLINE) yytext = "\\n";
        // else if (token == TOKEN_EOF) yytext = "EOF";
        // printf("TOKEN: %d -> '%s'\n", token, yytext);
    }
    while (token != TOKEN_EOF);

    puts("[>] Lexical Analysis completed with success.");

    // Parsing Phase ////////////////////////////////////////////////
    int state = parse(Tokens_List);
    stack_destroy_s(RDStack);
    list_destroy(Tokens_List);

    if (state)
    {
        puts("[>] Parsing completed with success.");
    }
    else
    {
        puts("[!] Parsing did not complete.");
        return 1;
    }

    ast_root_node = stack_pop(AST_Stack);
    stack_destroy(AST_Stack);

    // Program Interpretation Phase /////////////////////////////////
    // print_ast(ast_root_node);
    interpret(ast_root_node);

    return 0;
}
