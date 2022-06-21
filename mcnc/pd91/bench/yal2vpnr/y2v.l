%{
/* Copyright 1990 by the Microelectronics Center of North Carolina       
 * All rights reserved.
 */
#include <stdio.h>
#include "y2v_yacc.h"

/* #define DEBUG */
#ifndef DEBUG
#define RETURN(a) return(a);
#define MYBEGIN(a) { state = a; BEGIN a; }
#else
#undef RETURN
#ifdef __GNUC__
#define RETURN(a) fprintf(stderr,"returning %s\n",#a); return(a);
#define MYBEGIN(a) { state = a; BEGIN a; fprintf(stderr,"state %s\n",#a); }
#else __GNUC__
#define RETURN(a) fprintf(stderr,"returning %s\n","a"); return(a);
#define MYBEGIN(a) { state = a; BEGIN a; fprintf(stderr,"state %s\n","a"); }
#endif __GNUC__
#endif

static int state = 0;       /* Stores the state while skipping a comment */
extern int yylval;
%}

ws	[ \t]
name	[A-Za-z][_A-Za-z0-9./#$<>]*
number	[-0-9][0-9]*
decimal [-0-9][0-9]*[.][0-9]+
semi	[;]

%{
/* The states are:
 *	INITIAL - waiting for a keyword.
 *	COMMENT	- scanning a comment.
 *	LINE	- reading a line past the keyword.
 */
%}
%start COMM LINE

%a 10000
%o 20000

%%

<COMM>.*"*/"	{ BEGIN state;	/* Return to the state before the comment */ }
<COMM>[^*/\n]*	{		/* Ignore comment contents */		     }
<COMM>[\n/*]	{		/* Ignore comment contents */		     }
"/*"		{ BEGIN COMM;	/* Start scanning a comment. */		     }

{semi}		{ MYBEGIN(0);	RETURN(SEMI); /* Semicolon ends a line. */   }

<LINE>CURRENT	{ MYBEGIN(LINE); RETURN(CURRENT); }
<LINE>VOLTAGE	{ MYBEGIN(LINE); RETURN(VOLTAGE); }
<LINE>{name}	{ yylval = strclone(yytext); RETURN(NAME); }
{number}	{ yylval = strclone(yytext); RETURN(NUMBER); }
{decimal}	{ yylval = strclone(yytext); RETURN(DECIMAL); }

MODULE		{ MYBEGIN(LINE); RETURN(MODULE); }
TYPE		{ MYBEGIN(LINE); RETURN(TYPE); }
WIDTH		{ MYBEGIN(LINE); RETURN(WIDTH); }
HEIGHT		{ MYBEGIN(LINE); RETURN(HEIGHT); }
IOLIST		{ MYBEGIN(LINE); RETURN(IOLIST); }
NETWORK		{ MYBEGIN(LINE); RETURN(NETWORK); }
DIMENSIONS	{ MYBEGIN(LINE); RETURN(DIMENSIONS); }
ENDNETWORK	{ MYBEGIN(LINE); RETURN(ENDNETWORK); }
ENDIOLIST	{ MYBEGIN(LINE); RETURN(ENDIOLIST); }
ENDMODULE	{ MYBEGIN(LINE); RETURN(ENDMODULE); }
{name}		{ MYBEGIN(LINE); yylval = strclone(yytext); RETURN(NAME); }

{ws}	;
\n	;
.	{fprintf(stderr,"Illegal character (%s) in line %d\n",yytext,yylineno);}

%%
