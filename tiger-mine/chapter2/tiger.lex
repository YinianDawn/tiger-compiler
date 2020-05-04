%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos = 1;

int yywrap(void) {
    charPos = 1;
    return 1;
}


void adjust(void) {
    EM_tokPos = charPos;
    charPos += yyleng;
}


%}

digits [0-9]
ID     [a-zA-Z][a-zA-Z_0-9]*

%%
" "	 { adjust(); continue; }
\n	 { adjust(); EM_newline(); continue; }
","	 { adjust(); return COMMA; }
for  { adjust(); return FOR; }
{digits}+ { adjust(); yylval.ival = atoi(yytext); return INT; }


let  { adjust(); return LET; }
type { adjust(); return TYPE; }


\t	 { adjust(); continue; }
=    { adjust(); return ASSIGN; }
:    { adjust(); return COLON; }
"["    { adjust(); return LBRACK; }
"]"    { adjust(); return RBRACK; }

{ID} { adjust(); yylval.sval = yytext; return ID; }

"/*" {
        int c;
        for ( ; ; ) {
            while ( (c = input()) != '*' && c != EOF ) ;    /* eat up text of comment */
            if ( c == '*' ) {
                while ( (c = input()) == '*' ) ;
                if ( c == '/' ) break;    /* found the end */
            }
            if ( c == EOF ) {
                EM_error(EM_tokPos, "EOF in comment"); break;
            }
        }
        continue; 
     }

.	 { adjust(); printf("'%s'->%d", yytext, yytext); EM_error(EM_tokPos, "illegal token"); }

