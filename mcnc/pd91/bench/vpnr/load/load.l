%{
/*
 *   This code was developed by individuals connected with MCNC
 *   and Duke University.
 *
 *   Copyright 1988 by the Microelectronics Center of North Carolina       
 *   All rights reserved.
 *
 *   $Source: /mcnc/pi/hill/modgen89/bench/src/load/RCS/load.l,v $
 *   $Date: 89/01/20 09:48:02 $
 *   $Author: kk $
 *   $Revision: 1.1 $
 *
 * This file contains a lexical analyzer fore processing 'load' data.  Each
 * key_line is converted to an 'ed' command that searches for the appropriate
 * comment in the parser.tmpl template and inserts the code after this comment.
 */

static int zzSwitch = 0;
static int zzIgnored = 1;
#ifndef RETURN
#define RETURN(a) return(a);
#else
#undef RETURN
#define RETURN(a) fprintf(stderr,"returning %d\n",a); return(a);
#endif
%}

/* regular definitions */
ws			[ \t]*
cmd			##
key			[0-9a-z]*

%START	CMD TXT

%%

<TXT>^{cmd}			{if (zzSwitch != 0) printf(".\n"); else zzSwitch = 1; printf("/\\/\\*"); BEGIN CMD; RETURN(zzKEYLINE);}
<TXT>{ws}		{ECHO;}
<TXT>.			{ECHO;}
<TXT>\n			{ECHO;}
^{cmd}			{if (zzSwitch != 0) printf(".\n"); else zzSwitch = 1; printf("/\\/\\*"); BEGIN CMD; RETURN(zzKEYLINE);}
<CMD>0			{ECHO;RETURN(zzZERO);}
<CMD>1			{ECHO;RETURN(zzONE);}
<CMD>2			{ECHO;RETURN(zzTWO);}
<CMD>3			{ECHO;RETURN(zzTHREE);}
<CMD>B			{ECHO;RETURN(zzB);}
<CMD>L			{ECHO;RETURN(zzL);}
<CMD>R			{ECHO;RETURN(zzR);}
<CMD>T			{ECHO;RETURN(zzT);}
<CMD>addend		{ECHO;RETURN(zzADDEND);}
<CMD>addtop		{ECHO;RETURN(zzADDTOP);}
<CMD>attribute		{ECHO;RETURN(zzATTRIBUTE);}
<CMD>begin		{ECHO;RETURN(zzBEGIN);}
<CMD>bottom		{ECHO;RETURN(zzBOTTOM);}
<CMD>cell		{ECHO;RETURN(zzCELL);}
<CMD>channel		{ECHO;RETURN(zzCHANNEL);}
<CMD>contcont		{ECHO;RETURN(zzCONTCONT);}
<CMD>contedge		{ECHO;RETURN(zzCONTEDGE);}
<CMD>contsize		{ECHO;RETURN(zzCONTSIZE);}
<CMD>decimal		{ECHO;RETURN(zzDECIMAL);}
<CMD>domain		{ECHO;RETURN(zzDOMAIN);}
<CMD>end		{ECHO;RETURN(zzEND);}
<CMD>equivalences	{ECHO;RETURN(zzEQUIVALENCES);}
<CMD>equivnet		{ECHO;RETURN(zzEQUIVNET);}
<CMD>gate		{ECHO;RETURN(zzGATE);}
<CMD>head		{ECHO;RETURN(zzHEAD);}
<CMD>tail		{ECHO;RETURN(zzTAIL);}
<CMD>integer		{ECHO;RETURN(zzINTEGER);}
<CMD>iolist		{ECHO;RETURN(zzIOLIST);}
<CMD>left		{ECHO;RETURN(zzLEFT);}
<CMD>name		{ECHO;RETURN(zzNAME);}
<CMD>names		{ECHO;RETURN(zzNAMES);}
<CMD>nets		{ECHO;RETURN(zzNETS);}
<CMD>offset		{ECHO;RETURN(zzOFFSET);}
<CMD>point		{ECHO;RETURN(zzPOINT);}
<CMD>profile		{ECHO;RETURN(zzPROFILE);}
<CMD>quotestring	{ECHO;RETURN(zzQUOTESTRING);}
<CMD>right		{ECHO;RETURN(zzRIGHT);}
<CMD>row		{ECHO;RETURN(zzROW);}
<CMD>sep		{ECHO;RETURN(zzSEP);}
<CMD>signal		{ECHO;RETURN(zzSIGNAL);}
<CMD>string		{ECHO;RETURN(zzSTRING);}
<CMD>tech		{ECHO;RETURN(zzTECH);}
<CMD>terminal		{ECHO;RETURN(zzTERMINAL);}
<CMD>termlist		{ECHO;RETURN(zzTERMLIST);}
<CMD>siglist		{ECHO;RETURN(zzSIGLIST);}
<CMD>translist		{ECHO;RETURN(zzTRANSLIST);}
<CMD>drain		{ECHO;RETURN(zzDRAIN);}
<CMD>source		{ECHO;RETURN(zzSOURCE);}
<CMD>top		{ECHO;RETURN(zzTOP);}
<CMD>via		{ECHO;RETURN(zzVIA);}
<CMD>viacont		{ECHO;RETURN(zzVIACONT);}
<CMD>viaedge		{ECHO;RETURN(zzVIAEDGE);}
<CMD>vialist		{ECHO;RETURN(zzVIALIST);}
<CMD>viasize		{ECHO;RETURN(zzVIASIZE);}
<CMD>viavia		{ECHO;RETURN(zzVIAVIA);}
<CMD>wire		{ECHO;RETURN(zzWIRE);}
<CMD>wirelist		{ECHO;RETURN(zzWIRELIST);}
<CMD>x1x2y		{ECHO;RETURN(zzX1X2Y);}
<CMD>x1x2y1y2		{ECHO;RETURN(zzX1X2Y1Y2);}
<CMD>xgrid		{ECHO;RETURN(zzXGRID);}
<CMD>xy			{ECHO;RETURN(zzXY);}
<CMD>xy1y2		{ECHO;RETURN(zzXY1Y2);}
<CMD>ygrid		{ECHO;RETURN(zzYGRID);}
<CMD>{ws}\n		{printf(" \\*\\//a\n"); BEGIN TXT; RETURN(zzNEWLINE);}
<CMD>{ws}		{printf(" "); }
<CMD>{key}		{fprintf(stderr,"Bad keyword: '%s' in line %d\n",yytext,yylineno);exit(1);}
<CMD>.			{fprintf(stderr,"Bad keyword: '%s' in line %d\n",yytext,yylineno);exit(1);}
.			{if (zzIgnored) fprintf(stderr,"The following extra characters before the first key_line ignored:\n"); zzIgnored = 0; fprintf(stderr,"%s",yytext);}
{ws}			{ if (zzIgnored == 0) fprintf(stderr,"%s",yytext);}
\n			{ if (zzIgnored == 0) fprintf(stderr,"%s",yytext);}

%%

yywrap()
{printf(".\nw\n\q\n"); RETURN(1); }
