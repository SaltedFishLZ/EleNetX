%{
/****************************************************************************

                                                         ***
                                                         ***
     *********        *********         ******** *    ******       ******** *
   ***      ***      ***      **       **       **       ***      **       **
  ***        ***     ***   ******      ********* *       ***      ********* *
  ***        ***     *******  ***         *******        ***         *******
  ***        ***    ***       ***      *        **       ***      *        **
   ***      ***     ***       ***      **        *       ***      **        *
     *********       ********* ****    * ********     *******     * ********

*****************************************************************************

     Copyright 1990 by the Microelectronics Center of North Carolina       
	           Designed and written by Krzysztof Kozminski.

     This copy, of MCNC proprietary software, is made available exclusively
     for the use as a part of the 'v2' translator.

	This module contains a new parser for the VPNR language.

****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "unixproto.h"
#include "oasisutils.h"
#include "custom.h"
#include "parser_proto.h"
#include "lint_fix.h"

/* Code of an entity that can posess an attribute.
 */
enum atttype {
	 ATT_NONE, ATT_OF_CELL, ATT_OF_CHANNEL, ATT_OF_DOMAIN
	, ATT_OF_ROW, ATT_OF_NET ,ATT_OF_TERMINAL, ATT_OF_XTOR
} attEntity = ATT_NONE;


#ifdef ANSI_PROTO
void yyerror(char*);
#else ANSI_PROTO
void yyerror();
#endif ANSI_PROTO
%}

%start file
%token zzATT,zzBEGIN,zzBOT,zzCELL,zzCHANNEL,zzCONTCONT,zzCONTEDGE,zzCONTSIZE
%token zzDOMAIN,zzEND,zzEQUIVS,zzID,zzINTEGER,zzIOLIST,zzLEFT,zzNETS,zzNUMBER
%token zzOFFSET,zzPROFILE,zzQUOTESTR,zzRIGHT,zzROW,zzSEP,zzSIGLIST,zzTBRL
%token zzTECH,zzTERMLIST,zzTOP,zzTRANSLIST,zzVIACONT,zzVIAEDGE,zzVIALIST
%token zzVIASIZE,zzVIAVIA,zzWIRE,zzWIRELIST,zzXGRID,zzXTOR,zzYGRID,zzPOINT

%%

file:	  section | file section ;

section: cell | technology | domain ;

technology:
	techbegin techbody zzTECH zzEND name ;

techbegin:
	  zzTECH zzBEGIN name
	| techbegin attribute
	;

techbody:
	| techbody techline ;

techline:
	  zzSEP int_or_empty ':' zzINTEGER ';'
	| zzWIRE int_or_empty ':' zzINTEGER ';'
	| zzVIASIZE int_or_empty ':' zzINTEGER ';'
	| zzVIAVIA int_or_empty ':' zzINTEGER ';'
	| zzVIACONT int_or_empty ':' zzINTEGER ';'
	| zzCONTSIZE int_or_empty ':' zzINTEGER ';'
	| zzCONTCONT int_or_empty ':' zzINTEGER ';'
	| zzVIAEDGE int_or_empty ':' zzINTEGER ';'
	| zzCONTEDGE int_or_empty ':' zzINTEGER ';'
	| zzXGRID ':' zzINTEGER ';'
	| zzYGRID ':' zzINTEGER ';'
	| zzOFFSET ':' zzINTEGER ';'
	| techline attribute
	;

int_or_empty:	{ $$=(-1); } | integer ;

cell: cell_with_stuff zzCELL zzEND name		{ $$=Y_WrapUpCell($1,$4); } ;

name:	  zzID | zzINTEGER ;

cell_with_stuff:
	  cell_with_io | cell_with_equivs | cell_with_sigs | cell_with_xtors ;

cell_with_io:
	  begcell zzTERMLIST cellport		{ $$=Y_AddCellPort($1,$3); }
	| cell_with_profile zzTERMLIST cellport	{ $$=Y_AddCellPort($1,$3); }
	| cell_with_io cellport			{ $$=Y_AddCellPort($1,$2); }
	;

begcell:  zzCELL zzBEGIN name			{ $$=Y_InitCell($3); }
	| begcell attribute			{ $$=Y_AddCellAttrib($1,$2); }
	;

cellport: cellportpins ';'		{ $$=Y_WrapUpCellPort($1); }

cellportpins: cellport_some_pins '}'	{ $$=Y_WrapUpCellPortPins($1); }
	| cellportpins attribute	{ $$=Y_AddCellPortAttrib($1,$2); }

cellport_some_pins:
	  name '{'			{ $$=Y_InitCellPort($1); }
	| cellport_some_pins pin	{ $$=Y_AddCellPortPin($1,$2); }
	;

pin:	  '(' integer ',' integer ')'
					{ $$=Y_InitCellPin($2,$2,$4,$4); }
	| '(' integer '-' integer ',' integer ')'
					{ $$=Y_InitCellPin($2,$4,$6,$6); }
	| '(' integer ',' integer '-' integer ')'
					{ $$=Y_InitCellPin($2,$2,$4,$6); }
	| '(' integer '-' integer ',' integer '-' integer ')'
					{ $$=Y_InitCellPin($2,$4,$6,$8); }
	| pin attribute			{ $$=Y_AddCellPinAttrib($1,$2);}
	;

cell_with_profile:
	  begcell zzPROFILE zzTOP profile ';' zzPROFILE zzBOT profile ';'
		{ int aux=Y_AddCellProfile($1,$4,zzTOP,$3);
		  $$=Y_AddCellProfile(aux,$8,zzBOT,$7); }
	| begcell zzPROFILE zzBOT profile ';' zzPROFILE zzTOP profile ';'
		{ int aux=Y_AddCellProfile($1,$8,zzTOP,$7);
		  $$=Y_AddCellProfile(aux,$4,zzBOT,$3); }

profile:
	  '(' integer ',' integer ')'
				{ $$=Y_InitProfile($2,$4); }
	| profile '(' integer ',' integer ')'
				{ $$=Y_AppendProfilePoint($1,$3,$5); }
	;


cell_with_equivs:
	  cell_with_io zzEQUIVS equiv	{ $$=Y_AddCellEquiv($1,$3); }
	| cell_with_equivs equiv	{ $$=Y_AddCellEquiv($1,$2); }
	;

equiv:	  equivbeg ')' ';'		{ $$=Y_WrapUpEquiv($1); } ; 

equivbeg:
	  name '(' name			{ $$=Y_InitEquiv($1,$3); }
	| equivbeg name			{ $$=Y_AddEquiv($1,$2); }
	;

cell_with_sigs:
	  cell_with_some_sigs ';'	{ $$=Y_WrapUpCellSiglist($1); } ;

cell_with_some_sigs:
	  cell_with_io zzSIGLIST	{ $$=Y_InitCellSigList($1); }
	| cell_with_equivs zzSIGLIST	{ $$=Y_InitCellSigList($1); }
	| cell_with_some_sigs name	{ $$=Y_AddCellSignal($1,$2); }
	;

cell_with_xtors:
	  cell_with_some_xtors ';'	{ $$=Y_WrapUpXtors($1); } ;
	
cell_with_some_xtors:
	  cell_with_io zzTRANSLIST	{ $$=Y_InitXtorList($1); }
	| cell_with_equivs zzTRANSLIST	{ $$=Y_InitXtorList($1); }
	| cell_with_sigs zzTRANSLIST	{ $$=Y_InitXtorList($1); }
	| cell_with_some_xtors xtor	{ $$=Y_AddXtor($1,$2); }
	;

xtor:
	  name name name name	{ $$=Y_InitCellXtor($1,$2,$3,$4); }
	| xtor attribute	{ $$=Y_AddXtorAttrib($1,$2); }
	;

integer:  '-' zzINTEGER	{ $$=(-atoi((char*)$2)); }
	| '+' zzINTEGER { $$=atoi((char*)$2); }
	| zzINTEGER	{ $$=atoi((char*)$1); }
	;

attribute:
	  zzATT zzID		{ $$=Y_InitAttrib(zzID,$1,$2); }
	| zzATT zzINTEGER	{ $$=Y_InitAttrib(zzINTEGER,$1,$2); }
	| zzATT zzQUOTESTR	{ $$=Y_InitAttrib(zzQUOTESTR,$1,$2); }
	| zzATT zzNUMBER	{ $$=Y_InitAttrib(zzNUMBER,$1,$2); }
	| zzATT '(' integer ',' integer ')'
				{ $$=Y_InitPointAttrib($1,$3,$5); }
	;


domain: domain_with_cells ';' zzDOMAIN zzEND name
						{ $$=Y_WrapUpDomain($1,$5); } ;

domain_with_cells:
	  domain_with_io ';' zzROW zzINTEGER cell_instance
						{ $$=Y_AddCellInstance($1,$5); }
	| domain_with_cells cell_instance	{ $$=Y_AddCellInstance($1,$2); }

domain_with_io:
	  begdomain zzIOLIST domainport		{ $$=Y_AddDomainPort($1,$3); }
	| domain_with_profile zzIOLIST domainport
						{ $$=Y_AddDomainPort($1,$3); }
	| domain_with_io domainport		{ $$=Y_AddDomainPort($1,$2); }
	;

begdomain:  zzDOMAIN zzBEGIN name		{ $$=Y_InitDomain($3); }
	| begdomain attribute			{ $$=Y_AddDomainAttrib($1,$2); }
	;

domainport:
	  name zzTBRL ':' '(' integer ',' integer ')'
					{ $$=Y_InitDomainPort($1,$2,$5,$7); }
	| domainport attribute		{ $$=Y_AddDomainPortAttrib($1,$2); }
	;

domain_with_profile:
	  begdomain zzPROFILE zzTOP profile ';' zzPROFILE zzBOT profile ';'
		{ int aux=Y_AddDomainProfile($1,$4,zzTOP,$3);
		  $$=Y_AddDomainProfile(aux,$8,zzBOT,$7); }
	| begdomain zzPROFILE zzBOT profile ';' zzPROFILE zzTOP profile ';'
		{ int aux=Y_AddDomainProfile($1,$8,zzTOP,$7);
		  $$=Y_AddDomainProfile(aux,$4,zzBOT,$3); }

cell_instance:
	  instance_with_signals	')'	{ $$=Y_WrapUpInstance($1); }
	| cell_instance attribute	{ $$=Y_AddInstanceAttrib($1,$2); }
	;

instance_with_signals:
	  name name '(' name			{ $$=Y_InitCellInst($1,$2,$4); }
	| instance_with_signals ',' name	{ $$=Y_AddInstanceNet($1,$3); }
	;
