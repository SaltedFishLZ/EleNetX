%{
/*
 *   This code was developed by individuals connected with MCNC
 *   and Duke University.
 *
 *   Copyright 1988 by the Microelectronics Center of North Carolina       
 *   All rights reserved.
 *
 *   $Source: /mcnc/pi/hill/modgen89/bench/src/load/RCS/load.y,v $
 *   $Date: 89/01/20 09:48:03 $
 *   $Author: kk $
 *   $Revision: 1.1 $
 *
 * The sole purpose of this file is to verify the syntax of the key_lines
 * in a data file for 'load'.  The yacc-generated parser does not output
 * anything, except if it finds an error.  All processing of the key_lines
 * and their conversion to /bin/ed commands is done by the lex-generated
 * lexical analyzer.
 */


%}

%token zzKEYLINE zzZERO zzONE zzTWO zzTHREE zzB zzL zzR zzT zzADDEND zzADDTOP zzATTRIBUTE zzBEGIN zzBOTTOM zzCELL zzCHANNEL zzCONTCONT zzCONTEDGE zzCONTSIZE zzDECIMAL zzDOMAIN zzEND zzEQUIVALENCES zzEQUIVNET zzGATE zzHEAD zzINTEGER zzIOLIST zzLEFT zzNAME zzNAMES zzNETS zzOFFSET zzPOINT zzPROFILE zzQUOTESTRING zzRIGHT zzROW zzSEP zzSIGNAL zzSTRING zzTECH zzTERMINAL zzTERMLIST zzTOP zzVIA zzVIACONT zzVIAEDGE zzVIALIST zzVIASIZE zzVIAVIA zzWIRE zzWIRELIST zzX1X2Y zzX1X2Y1Y2 zzXGRID zzXY zzXY1Y2 zzYGRID zzNEWLINE zzTRANSLIST zzSIGLIST zzTAIL zzDRAIN zzSOURCE

%start file

%%

file:		keyline
	|	file keyline
	;

keyline:	 zzKEYLINE keywords zzNEWLINE
	;

keywords:	hastopend addtopend
	|	zzATTRIBUTE attobject attvalue
	|	zzBEGIN entity
	|	zzCHANNEL
	|	zzEND entity
	|	zzEQUIVALENCES eqobject
	|	zzNETS netobject
	|	zzPROFILE profobject profwhere
	|	zzPROFILE profobject zzPOINT profwhere
	|	zzROW rowobject
	|	zzTECH techobject1
	|	zzTECH techobject2 num
	|	zzTERMLIST termobject1 xy termwhere
	|	zzTERMLIST termobject1 termobject2
	|	zzIOLIST xy iolwhere
	|	zzIOLIST iolobject
	|	zzVIALIST viaobject
	|	zzSIGLIST sigobject
	|	zzTRANSLIST trlobject
	|	zzWIRELIST wireobject
	;

hastopend:	/* empty */
	|	zzATTRIBUTE
	|	zzPROFILE
	|	zzROW
	|	zzTECH
	|	zzTERMLIST
	|	zzIOLIST
	|	zzVIALIST
	|	zzWIRELIST
	|	zzBEGIN
	|	zzEND
	|	zzEQUIVALENCES
	|	zzSIGLIST
	|	zzTRANSLIST
	|	zzNETS
	;

addtopend:	zzADDEND
	|	zzADDTOP
	;

attobject:	zzCELL
	|	zzCHANNEL
	|	zzDOMAIN
	|	zzGATE
	|	zzIOLIST
	|	zzNETS
	|	zzROW
	|	zzTECH
	|	zzTERMINAL
	|	zzVIA
	|	zzTRANSLIST
	|	zzSIGLIST
	|	zzWIRE
	;

attvalue:	zzDECIMAL
	|	zzINTEGER
	|	zzPOINT
	|	zzSTRING
	|	zzQUOTESTRING
	;

entity: 	zzCELL
	|	zzCHANNEL
	|	zzDOMAIN
	|	zzTECH
	;

eqobject:	zzEQUIVNET
	|	zzHEAD
	|	zzTAIL
	|	zzNAME
	;

netobject:	zzHEAD
	|	zzTAIL
	|	zzNAME
	;

profwhere:	zzBOTTOM
	|	zzLEFT
	|	zzRIGHT
	|	zzTOP
	;

profobject:	zzCELL
	|	zzCHANNEL
	|	zzDOMAIN
	;

rowobject:	zzGATE
	|	zzHEAD
	|	zzTAIL
	|	zzNAME
	|	zzSIGNAL
	;

num:		/* empty */
	|	zzZERO
	|	zzONE
	|	zzTWO
	|	zzTHREE
	;

techobject2:	zzCONTCONT
	|	zzCONTEDGE
	|	zzCONTSIZE
	|	zzSEP
	|	zzVIACONT
	|	zzVIAEDGE
	|	zzVIASIZE
	|	zzVIAVIA
	|	zzWIRE
	;

techobject1:	zzHEAD
	|	zzTAIL
	|	zzOFFSET
	|	zzXGRID
	|	zzYGRID
	;

termobject1:	zzCHANNEL
	|	zzCELL
	;

termwhere:	zzB
	|	zzL
	|	zzR
	|	zzT
	|	/* empty */
	;

xy:		zzX1X2Y
	|	zzX1X2Y1Y2
	|	zzXY
	|	zzXY1Y2
	;

termobject2:	zzBOTTOM
	|	zzHEAD
	|	zzTAIL
	|	zzLEFT
	|	zzNAME
	|	zzNAMES
	|	zzRIGHT
	|	zzTOP
	;

iolwhere:	zzB
	|	zzL
	|	zzR
	|	zzT
	;

iolobject:	zzBOTTOM
	|	zzHEAD
	|	zzTAIL
	|	zzLEFT
	|	zzRIGHT
	|	zzSIGNAL
	|	zzTOP
	;

trlobject:	zzHEAD
	|	zzTAIL
	|	zzNAME
	|	zzGATE
	|	zzDRAIN
	|	zzSOURCE
	;

sigobject:	zzHEAD
	|	zzTAIL
	|	zzNAME
	;

viaobject:	zzHEAD
	|	zzTAIL
	|	zzPOINT
	;

wireobject:	zzHEAD
	|	zzTAIL
	|	zzNAME
	|	zzNAMES
	|	zzPOINT
	;

%%

#include "lex.yy.c"

yyerror(s) char *s;
{
	fprintf(stderr,"%s on line %d. Probably bad sequence of keywords.\nCheck file load.tmpl in the VPNR library for the list of correct key lines.\n"
		,s,yylineno);
	exit (1);
}

main()
{
#ifdef YYDEBUG
	extern int yydebug;
	yydebug = 1;
#endif YYDEBUG
	yyparse();
}
