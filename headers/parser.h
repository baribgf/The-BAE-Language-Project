#ifndef PARSER_H
#define PARSER_H

#ifndef HELPER_H
#include "common.h"
#endif

#ifndef TOKENS_H
#include "tokens.h"
#endif

#define DebugPreDerive(x) if (debug) { printf("[DEBUG]: Deriving %s, pointing to: %s    ", x, (token != TOKEN_EOF && token != TOKEN_NEWLINE) ? yytext : (token == TOKEN_EOF) ? "EOF" : "\\n"); push_stack_s(x, RDStack); print_stack_s(RDStack); }
#define DebugPostDerive(x) if (debug) { pop_stack_s(RDStack); printf("[DEBUG]: Returning (%d) from %s, pointing to: %s    ", proc, x, (token != TOKEN_EOF && token != TOKEN_NEWLINE) ? yytext : (token == TOKEN_EOF) ? "EOF" : "\\n"); print_stack_s(RDStack); }

int Program();
int Expr();
int Stmnt();

TOKEN perform_lookahead();

extern TOKEN token;
extern int token_pos, blank_source;
extern char *filename;
extern Stack_t *AST_Stack;

extern int proc;

#endif // PARSER_H
