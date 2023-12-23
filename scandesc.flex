%{
#include "headers/tokens.h"
%}

%%
[ \t]            // skip whitespaces excluding newline
#.*              // skip comments
[0-1]            { return TOKEN_BIT; }
[a-zA-Z_]+       { return TOKEN_IDENT; }
"="              { return TOKEN_ASSIGN_OP; }
[\+\|]           { return TOKEN_OR_OP; }
[\.&]            { return TOKEN_AND_OP; }
[!~]             { return TOKEN_NOT_OP; }
"("              { return TOKEN_OPEN_PAR; }
")"              { return TOKEN_CLOSE_PAR; }
"<"              { return TOKEN_OPEN_TAG; }
">"              { return TOKEN_CLOSE_TAG; }
"{"              { return TOKEN_OPEN_BRACE; }
"}"              { return TOKEN_CLOSE_BRACE; }
">>"             { return TOKEN_IN_OP; }
"<<"             { return TOKEN_OUT_OP; }
","              { return TOKEN_COMMA; }
\n               { return TOKEN_NEWLINE; }
.                { return TOKEN_ERROR; }
%%

int yywrap() { return 1; }
