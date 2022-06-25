%{
/*###########################################################################

                                                         ###
                                                         ###
     #########        #########         ######## #    ######       ######## #
   ###      ###      ###      ##       ##       ##       ###      ##       ##
  ###        ###     ###   ######      ######### #       ###      ######### #
  ###        ###     #######  ###         #######        ###         #######
  ###        ###    ###       ###      #        ##       ###      #        ##
   ###      ###     ###       ###      ##        #       ###      ##        #
     #########       ######### ####    # ########     #######     # ########

#############################################################################

     Copyright 1990 by the Microelectronics Center of North Carolina       
	           Designed and written by Krzysztof Kozminski.

     This copy, of MCNC proprietary software, is made available exclusively
     for the use as a part of the 'v2' translator.

#############################################################################

	This module contains a new lexical analyzer for the VPNR language.

###########################################################################*/

#ifdef __GNUC__
#ifndef ANSI_PROTO
#define ANSI_PROTO
#endif ANSI_PROTO
#endif __GNUC__

#include "unixproto.h"
#include "oasisutils.h"
#include "parser.h"

#ifdef ANSI_PROTO
int yywrap(void);
void yyerror(char*);
void SetInputFileName(char*);
int   strlen(char*);
void  stripq(char*);
int   fprintf(FILE*,char*,...);
void incl_file(void);
#else ANSI_PROTO
int yywrap();
void yyerror();
void SetInputFileName();
int   strlen();
int   fprintf();
void  stripq();
void incl_file();
#endif ANSI_PROTO

/* A NOTE ON THE LEXICAL ANALYZER.
 * After recognizing any token, the text string representing this token is put
 * into a temporary hash table.  The 'yylval' variable is set to point to the
 * copy of this string that is a part of the hash table structure; neither the
 * location pointed at by 'yylval' nor its contents will change any more (until
 * the table is deallocated).  This allows to get around the problems with the
 * contents of 'yytext' getting overwritten before they are passed to the
 * parser.  The definition below does what has just been described.
 */
#define MY_YYLVAL  UseHashTable(TmpNames);RegisterName(yytext,(char**)&yylval);

/* #define DEBUG */
#ifdef DEBUG
#undef DEBUG
#define DEBUG(a)   printf("\n%-14s (%2d) '%s'",a,(int)YYSTATE,yytext);
#ifdef __GNUC__
#define RETURN(a) {DEBUG(#a); MY_YYLVAL; return(a);}
#else __GNUC__
#define RETURN(a) {DEBUG("a");  MY_YYLVAL; return(a);}
#endif __GNUC__
#else DEBUG
#define RETURN(a) {MY_YYLVAL; return (a); }
#endif DEBUG

extern int yylval;

/* Data structure used to handle the '#include' directives */

#define MAXLINE 81

typedef struct ZZSTACK {
	char *fName;
	FILE *fp;
	struct ZZSTACK *next;
} *zzstack;

zzstack fileStack = NULL;
static char *inputFileName = "stdin";

extern int TmpNames;

%}
%e 600
%p 9000
%n 300
%k 100
%a 15000
%o 40000

/* White space definitions.  */

ws		[ \t\n]+
ws2         	[ \t]*
nl		[\n]+

/* Delimiters cannot be parts of names */
delim		[-=+{(,)};:]

/* Letters, digits, and a couple of other characters can be parts of names. */
letter		[$A-Za-z_#/<>]
period		[.]
digit		[0-9]

/* Letter T, B, R, and L are keywords */
tbrl		[TBRL]

/* Valid identifiers: letters and digits, with at least one letter */
id		({letter}|{digit}|{period})*{letter}({letter}|{digit}|{period})*

/* Numbers: integer and floating point */
integer		{digit}+
number		{digit}+(\.{digit}*)?

/* Quoted strings */
quotestr	\"[^\"\n]*\"

/* Comment delimiters */
combeg		\/\*
comend		\*\/

%start zzCOM zzNC

%%

{combeg}		{BEGIN zzCOM;}
<zzCOM>{ws}		{/* skip comments */}
<zzCOM>.		{/* skip comments */}
<zzCOM>{comend}		{BEGIN zzNC;}

<zzNC>#{ws2}include{ws2}\".*\"	{incl_file();}

<zzNC>{ws}		{/* skip white space */}

<zzNC>tech(nology)?	{RETURN(zzTECH);}
<zzNC>cell		{RETURN(zzCELL);}
<zzNC>domain		{RETURN(zzDOMAIN);}
<zzNC>channel		{RETURN(zzCHANNEL);}
<zzNC>sep		{RETURN(zzSEP);}
<zzNC>wire		{RETURN(zzWIRE);}
<zzNC>contsize		{RETURN(zzCONTSIZE);}
<zzNC>contcont		{RETURN(zzCONTCONT);}
<zzNC>contedge		{RETURN(zzCONTEDGE);}
<zzNC>viasize		{RETURN(zzVIASIZE);}
<zzNC>viavia		{RETURN(zzVIAVIA);}
<zzNC>viaedge		{RETURN(zzVIAEDGE);}
<zzNC>viacont		{RETURN(zzVIACONT);}
<zzNC>xgrid		{RETURN(zzXGRID);}
<zzNC>ygrid		{RETURN(zzYGRID);}
<zzNC>offset		{RETURN(zzOFFSET);}
<zzNC>begin		{RETURN(zzBEGIN);}
<zzNC>end		{RETURN(zzEND);}
<zzNC>profile		{RETURN(zzPROFILE);}
<zzNC>top		{RETURN(zzTOP);}
<zzNC>bot(tom)?		{RETURN(zzBOT);}
<zzNC>right		{RETURN(zzRIGHT);}
<zzNC>left		{RETURN(zzLEFT);}
<zzNC>RIGHT		{RETURN(zzRIGHT);}
<zzNC>LEFT		{RETURN(zzLEFT);}
<zzNC>TOP		{RETURN(zzTOP);}
<zzNC>BOT(TOM)?		{RETURN(zzBOT);}
<zzNC>iolist		{RETURN(zzIOLIST);}
<zzNC>termlist		{RETURN(zzTERMLIST);}
<zzNC>wirelist		{RETURN(zzWIRELIST);}
<zzNC>vialist		{RETURN(zzVIALIST);}
<zzNC>equivalences	{RETURN(zzEQUIVS);}
<zzNC>row		{RETURN(zzROW);}
<zzNC>nets		{RETURN(zzNETS);}
<zzNC>siglist		{RETURN(zzSIGLIST);}
<zzNC>translist		{RETURN(zzTRANSLIST);}

<zzNC>{tbrl}		{RETURN(zzTBRL);}
<zzNC>{quotestr}	{stripq(yytext); RETURN(zzQUOTESTR);}
<zzNC>{id}=		{yytext[strlen(yytext)-1]=0; RETURN(zzATT);}
<zzNC>{id}		{RETURN(zzID);}
<zzNC>{integer}		{RETURN(zzINTEGER);}
<zzNC>{number}		{RETURN(zzNUMBER);}
<zzNC>{delim}		{RETURN(yytext[0]);}
<zzNC>.			{Message("FATAL ERROR - Unexpected token: '%s'\n",yytext);exit(1);}

\n			{ BEGIN zzNC; }
.			{unput(yytext[0]); BEGIN zzNC; }

%%

/* Process an included file */

void incl_file()
{
	char *fName;
	register zzstack a;

/* Store the current file.
 */
	a = (zzstack) MessPtr(sizeof(*a));
	a->fName = inputFileName;
	a->fp = yyin;
	a->next = fileStack;
	fileStack = a;

/* Strip the last quote, and register the file name.  Note that at this point
 * we have exactly two quotes in yytext (the second is also the last character).
 */
	yytext[yyleng-1] = '\0';
	UseHashTable(TmpNames);
	RegisterName(index(yytext,'"'),&fName);

	if ((yyin = fopen(fName,"r")) == NULL) {
		Message("FATAL ERROR - Cannot open input file '%s'\n",fName);
		exit(1);
	}
}


/* Return from included file or end-of-data */

int yywrap()
{
	register zzstack a;

	if (fileStack == NULL)	/* End of data */
		return(1);
	yyin = fileStack->fp;
	inputFileName = fileStack->fName;

	a = fileStack;
	fileStack = fileStack->next;
	cfree ((char *) a);
	return(0);
}

void SetInputFileName(fName)
char *fName;
{
	UseHashTable(TmpNames);
	RegisterName(fName,&inputFileName);
}

void yyerror(s)
char *s;
{
	Message("FATAL ERROR - Syntax error in file '%s', line %d:  %s\n"
		,inputFileName,yylineno,s);
	exit(1);
}

void ErrMessage(format,a,b,c,d,e,f,g,h,i,j,k)
char *format;
long a,b,c,d,e,f,g,h,i,j,k;
{
	Message(format,a,b,c,d,e,f,g,h,i,j,k);
	Message("The above occurred in the vicinity of line %d in file '%s'"
		,yylineno,inputFileName);
}

void stripq(s)
register char *s;
{
	s[strlen(s)-1] = 0;
	do {
		*s = *(s+1);
		s++;
	} while (*s);
}
