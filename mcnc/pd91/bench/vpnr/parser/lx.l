%{
/*
 *   This code was developed by individuals connected with MCNC
 *   and Duke University.
 *
 *   Copyright 1988 by the Microelectronics Center of North Carolina       
 *   All rights reserved.
 *
 *   $Source: /mcnc/pi/hill/modgen89/bench/src/parser/RCS/lx.l,v $
 *   $Date: 89/01/20 09:47:56 $
 *   $Author: kk $
 *   $Revision: 1.1 $
 *
 */

#include "vparser.h"

extern int yylval;

#define MAXLINE 81
struct zzstack
{
	FILE *fp;
	char str[MAXLINE];
	struct zzstack * next;
} *zztost = NULL;
FILE *popntop();

%}

%e 531
%p 2923
%n 244
%k 82
%a 12623
%o 26093

/* regular definitions */

ws		[ \t\n]+
ws2         	[ \t]*
nl		[\n]+
delim		[-+{(,)};:]
letter		[$A-Za-z_.#/<>]
digit		[0-9]
tbrl		[TBRL]
id		{letter}({letter}|{digit})*(\[{digit}+\])*
integer		{digit}+
number		{digit}+(\.{digit}*)?
combeg		\/\*
comend		\*\/
quotestr	\"[^\"\n]*\"

%start zzCOM zzNC

%%

{combeg}		{BEGIN zzCOM;}

<zzCOM>{ws}		{/* skip comments */}
<zzCOM>.		{/* skip comments */}
<zzCOM>{comend}		{BEGIN zzNC;}

<zzNC>#{ws2}include{ws2}\".*\"	{incl_file();}

<zzNC>{ws}		{/* skip white space */}

<zzNC>tech(nology)?	{yylval=zzTECH;		return(zzTECH);}
<zzNC>cell		{yylval=zzCELL;		return(zzCELL);}
<zzNC>domain		{yylval=zzDOMAIN;	return(zzDOMAIN);}
<zzNC>channel		{yylval=zzCHANNEL;	return(zzCHANNEL);}
<zzNC>sep		{yylval=zzSEP;		return(zzSEP);}
<zzNC>wire		{yylval=zzWIRE;		return(zzWIRE);}
<zzNC>contsize		{yylval=zzCONTSIZE;	return(zzCONTSIZE);}
<zzNC>contcont		{yylval=zzCONTCONT;	return(zzCONTCONT);}
<zzNC>contedge		{yylval=zzCONTEDGE;	return(zzCONTEDGE);}
<zzNC>viasize		{yylval=zzVIASIZE;	return(zzVIASIZE);}
<zzNC>viavia		{yylval=zzVIAVIA;	return(zzVIAVIA);}
<zzNC>viaedge		{yylval=zzVIAEDGE;	return(zzVIAEDGE);}
<zzNC>viacont		{yylval=zzVIACONT;	return(zzVIACONT);}
<zzNC>xgrid		{yylval=zzXGRID;	return(zzXGRID);}
<zzNC>ygrid		{yylval=zzYGRID;	return(zzYGRID);}
<zzNC>offset		{yylval=zzOFFSET;	return(zzOFFSET);}
<zzNC>begin		{yylval=zzBEGIN;	return(zzBEGIN);}
<zzNC>end		{yylval=zzEND;		return(zzEND);}
<zzNC>profile		{yylval=zzPROFILE;	return(zzPROFILE);}
<zzNC>top		{yylval=zzTOP;		return(zzTOP);}
<zzNC>bot(tom)?		{yylval=zzBOT;		return(zzBOT);}
<zzNC>right		{yylval=zzRIGHT;	return(zzRIGHT);}
<zzNC>left		{yylval=zzLEFT;		return(zzLEFT);}
<zzNC>RIGHT		{yylval=zzRIGHT;	return(zzRIGHT);}
<zzNC>LEFT		{yylval=zzLEFT;		return(zzLEFT);}
<zzNC>TOP		{yylval=zzTOP;		return(zzTOP);}
<zzNC>BOT(TOM)?		{yylval=zzBOT;		return(zzBOT);}
<zzNC>iolist		{yylval=zzIOLIST;	return(zzIOLIST);}
<zzNC>termlist		{yylval=zzTERMLIST;	return(zzTERMLIST);}
<zzNC>wirelist		{yylval=zzWIRELIST;	return(zzWIRELIST);}
<zzNC>vialist		{yylval=zzVIALIST;	return(zzVIALIST);}
<zzNC>equivalences	{yylval=zzEQUIVS;	return(zzEQUIVS);}
<zzNC>row		{yylval=zzROW;		return(zzROW);}
<zzNC>nets		{yylval=zzNETS;		return(zzNETS);}
<zzNC>siglist		{yylval=zzSIGLIST;	return(zzSIGLIST);}
<zzNC>translist		{yylval=zzTRANSLIST;	return(zzTRANSLIST);}

<zzNC>{tbrl}		{yylval=yytext[0];	return(yylval);}
<zzNC>{id}=		{yylval=(int)yytext;	return(zzATT);}
<zzNC>{quotestr}	{yylval=(int)yytext;	return(zzQUOTESTR);}
<zzNC>{id}		{yylval=(int)yytext;	return(zzID);}
<zzNC>{integer}		{yylval=atoi(yytext);	return(zzINTEGER);}
<zzNC>{number}		{yylval=(int)yytext;	return(zzNUMBER);}
<zzNC>{delim}		{yylval=yytext[0];	return(yylval);}
<zzNC>.			{yyerror("Bad identifier:  "); yyerror(yytext);}

{ws}			{ BEGIN zzNC; }
.			{unput(yytext[0]); BEGIN zzNC; }

%%

incl_file()
{
	char * p;

	push("first",yyin);
	for (p=yytext ; p[0]!='"'; p++)
		;
	p++;
	yytext[yyleng-1] = '\0';
	if ((yyin = fopen(p,"r")) == NULL)
		yyerror("Cannot open input file");
}


yywrap()
{
	char p[MAXLINE];
	FILE *ptr;

	if (zztost == NULL)
		return(1);
	else {
		ptr = popntop(p);
		if (ptr != NULL)
			yyin = ptr;
		else
			yyin = stdin;
		return(0);
	}
}


push(p,ptr)
char * p;
FILE * ptr;
{
	struct zzstack *a;

	a = (struct zzstack *) calloc(1,sizeof(struct zzstack));
	strcpy(a->str,p);
	a->fp = ptr;
	a->next = zztost;
	zztost = a;
}


FILE *
popntop(p)
char * p;
{
	struct zzstack *a;
	FILE *fp;

	if (zztost != NULL) {
		strcpy(p,zztost->str);
		fp = zztost->fp;
		a = zztost;
		zztost = a->next;
		cfree ((char *) a);
		return(fp);
	}
	strcpy(p,"\0");
	return(NULL);
}

yyerror(s)
char *s;
{
	fprintf(stderr,"Line %d:  %s\n",yylineno,s);
	exit(1);
}
