%{
#define LEX_DEBUG
#undef LEX_DEBUG
#ifdef MCNC
#include "oasisutils.h"
#include "unixproto.h"
#include "includefile.h"
#include "lexflex.h"
#else
#include "non_mcnc.h"
#endif

#include "x_yacc.h"

extern YYSTYPE yylval;

%}

digit           [0-9]
nl		\n
white		[ \t]
dot		\.
equal		[=]

number		{digit}+|[-]{digit}+
float           {digit}+\.{digit}*


%state LIBBEGIN, CHNBEGIN, DPCBEGIN
%%

<LIBBEGIN>{number}	{ (void)CurrentLineTxt(yytext); yylval.integer=atoi(yytext); BEGIN INITIAL; LEX_RETURN(CELL_COUNT); }
<CHNBEGIN>{number}	{ (void)CurrentLineTxt(yytext); yylval.integer=atoi(yytext); BEGIN INITIAL; LEX_RETURN(CHAN_COUNT); }
<DPCBEGIN>{number}	{ (void)CurrentLineTxt(yytext); yylval.integer=atoi(yytext); BEGIN INITIAL; LEX_RETURN(ROW_COUNT); }

{number}		{ (void)CurrentLineTxt(yytext); yylval.integer=atoi(yytext); LEX_RETURN(INTEGER); }
{float}			{ (void)CurrentLineTxt(yytext); yylval.flpt=atof(yytext); LEX_RETURN(FLOAT); }
{nl}+			{ (void)CurrentLineTxt(yytext); LEX_RETURN(NEWLINE); }
{equal}			{ (void)CurrentLineTxt(yytext); LEX_RETURN(EQUAL); }
"nrIOs"			{ (void)CurrentLineTxt(yytext); LEX_RETURN(NRIOS); }
"nrMacros"		{ (void)CurrentLineTxt(yytext); LEX_RETURN(NRMACROS); }
"nrMods"		{ (void)CurrentLineTxt(yytext); LEX_RETURN(NRMODS); }
"nrNets"		{ (void)CurrentLineTxt(yytext); LEX_RETURN(NRNETS); }
"nrPins"		{ (void)CurrentLineTxt(yytext); LEX_RETURN(NRPINS); }
"0 4 15"		{ (void)CurrentLineTxt(yytext); LEX_RETURN(SHAPE_HEAD); }
{white}			{ (void)CurrentLineTxt(yytext); /* ignore white space */ }
.			{ CurrentLineTxt(yytext); yyerror("Illegal character");}

%%

void initLexState(state)
int  state;
{
    switch (state) {
    case 0: BEGIN LIBBEGIN ; break;
    case 1: BEGIN CHNBEGIN ; break;
    case 2: BEGIN DPCBEGIN ; break;
    }
}

/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set sw=4:
 */
