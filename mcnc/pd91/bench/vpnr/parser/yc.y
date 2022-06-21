%{
/*
 *   This code was developed by individuals connected with MCNC
 *   and Duke University.
 *
 *   Copyright 1988 by the Microelectronics Center of North Carolina       
 *   All rights reserved.
 *
 *   $Source: /mcnc/pi/hill/modgen89/bench/src/parser/RCS/yc.y,v $
 *   $Date: 89/01/20 09:47:57 $
 *   $Author: kk $
 *   $Revision: 1.1 $
 *
 */

#include <stdio.h>
#include <ctype.h>

#define zzMAXNAME 32
#define zzMAXLINE 81
%}

%start file

%{
/* tokens for variable "zztype" */
%}
%token zzCELL zzDOMAIN zzCHANNEL zzTECH

%{
/* tokens for variable "zzkeyword" */
%}
%token zzBEGIN zzEND zzPROFILE zzTERMLIST zzWIRELIST zzVIALIST zzEQUIVS zzROW 
%token zzGATE zzNETS zzSIG zzIOLIST zzSIGLIST zzTRANSLIST

%{
/* tokens for the variable "zzwhere" */
%}
%token zzTOP zzBOT zzRIGHT zzLEFT

%{
/* tokens for values */
%}
%token zzID zzINTEGER zzNUMBER zzATT zzPOINT zzSTRING zzQUOTESTR

%token zzTERMINAL zzWIRE zzVIA zzEQLST zzDRAIN zzSOURCE

%{
/* tokens for technology stuff */
%}
%token zzXGRID zzYGRID zzSEP zzVIAVIA zzVIAEDGE zzCONTEDGE
%token zzCONTCONT zzVIACONT zzOFFSET zzCONTSIZE zzVIASIZE zzNONE

%{
/* Stuff for "load" */
%}
%token zzEQUIV zzHEAD zzTAIL zzNAME zzNAMES zzSIGNAL 
%token zzX1X2Y zzX1X2Y1Y2 zzXY zzXY1Y2 zzFIXED zzNETATT
%token zzLBS zzATTRIBUTE zzEQLIST zzADDTOP zzADDEND zzERROR

%{
int zztype;
int zzwhere;
int zzkeyword;
int zzi;
int pair[2];
int three[3];
int four[4];
char word[zzMAXLINE];
char wordpair[2][zzMAXNAME];
char attword[zzMAXLINE];
%}

%%

file:	  section
	| file section
	;

section:  begtech techbod endtech
	| begcell celltrm equivs siglist translist endcell
	| begcell profile celltrm equivs siglist translist endcell
	| begdmn profile dmninf netstuf enddmn
	| begchnl profile trmdesc wirdesc endchnl
	;

begtech:  zzTECH zzBEGIN
		{zztype=zzTECH; zzkeyword=zzBEGIN;}
	  name 
		{zzbegintype((char *)$4,zztype);}
	  | begtech att
	;

techbod:  techinf
	| techbod techinf
	;

techinf:  zzSEP ztothree ':' zzINTEGER ';'
		{zztechlist(zzSEP,$2,$4);}
	| zzWIRE ztothree ':' zzINTEGER ';'
		{zztechlist(zzWIRE,$2,$4);}
	| zzVIASIZE ztothree ':' zzINTEGER ';'
		{zztechlist(zzVIASIZE,$2,$4);}
	| zzVIAVIA ztothree ':' zzINTEGER ';'
		{zztechlist(zzVIAVIA,$2,$4);}
	| zzVIACONT ztothree ':' zzINTEGER ';'
		{zztechlist(zzVIACONT,$2,$4);}
	| zzCONTSIZE ztothree ':' zzINTEGER ';'
		{zztechlist(zzCONTSIZE,$2,$4);}
	| zzCONTCONT ztothree ':' zzINTEGER ';'
		{zztechlist(zzCONTCONT,$2,$4);}
	| zzVIAEDGE ztothree ':' zzINTEGER ';'
		{zztechlist(zzVIAEDGE,$2,$4);}
	| zzCONTEDGE ztothree ':' zzINTEGER ';'
		{zztechlist(zzCONTEDGE,$2,$4);}
	| zzXGRID ':' zzINTEGER ';'
		{zztechlist(zzXGRID,-1,$3);}
	| zzYGRID ':' zzINTEGER ';'
		{zztechlist(zzYGRID,-1,$3);}
	| zzOFFSET ':' zzINTEGER ';'
		{zztechlist(zzOFFSET,-1,$3);}
	| techinf att
	;

ztothree:  zzINTEGER
		{
			if ((0 > $1) || ($1 > 3)) yyerror("bad tech info");
			else switch($1) {
				case 0: $$='0'; break;
				case 1: $$='1'; break;
				case 2: $$='2'; break;
				case 3: $$='3';
			} 
		}
	|
		{$$ = zzNONE;}
	;

endtech:  zzTECH zzEND
		{zzkeyword=zzEND;}
	  name
		{zzendtype((char *)$4,zztype);}
	;

begcell:  zzCELL zzBEGIN
		{zztype=zzCELL; zzkeyword=zzBEGIN;}
	  name
		{zzbegintype((char *)$4,zztype);}
	  | begcell att
	;

celltrm:  zzTERMLIST
		{zzkeyword=zzTERMLIST; zzmaketermlist(zzHEAD,-1,zztype,-1);}
	  siglst3
	|
	;

siglst3:  sigtrm
	| siglst3 sigtrm
	;

sigtrm:	  name
		{zzmaketermlist(zzNAME,$1,zztype,zzwhere);}
	  sot ';'
		{zzmaketermlist(zzTAIL,-1,zztype,-1);}
	| name
		{zzmaketermlist(zzNAME,$1,zztype,zzwhere);}
	  ';'
		{zzmaketermlist(zzTAIL,-1,zztype,-1);}
	| name
		{zzmaketermlist(zzNAME,$1,zztype,zzwhere);}
	  soa ';'
		{zzmaketermlist(zzTAIL,-1,zztype,-1);}
	;

siglist:  zzSIGLIST
		{zzkeyword=zzSIGLIST; zzmksiglist(zzHEAD,-1); }
	  intlsiglst
	  ';'
		{zzmksiglist(zzTAIL,-1);}
	|
	;

intlsiglst:
	  intsig | intlsiglst intsig ;

intsig:	  name {zzmksiglist(zzNAME,$1);} | intsig att;

translist:
	  zzTRANSLIST
		{zzkeyword=zzTRANSLIST; zzmktranslist(zzHEAD,-1);}
	  intltranslst ';'
		{zzmktranslist(zzTAIL,-1);}
	|
	;

intltranslst:
	  trans | intltranslst trans ;

trans:	  name	  { zzmktranslist(zzNAME,$1); }
	  name	  { zzmktranslist(zzGATE,$3); }
	  name	  { zzmktranslist(zzDRAIN,$5); }
	  name	  { zzmktranslist(zzSOURCE,$7); }
	| trans att
	;

equivs:	  zzEQUIVS
		{zzkeyword=zzEQUIVS; zzmakeeqlist(zzHEAD,-1);}
	  eqlst
	|
	;

eqlst:	  eqlst1
	| eqlst eqlst1
	;

eqlst1:	  name 
		{zzmakeeqlist(zzNAME,$1);}
	  loen ';'
		{zzmakeeqlist(zzTAIL,$1);}
	;

loen:	  '(' eqnets ')'
	;

eqnets:	  name
		{zzmakeeqlist(zzEQUIV,$1);}
	| eqnets name
		{zzmakeeqlist(zzEQUIV,$2);}
	;

endcell:  zzCELL zzEND
		{zzkeyword=zzEND;}
	  name
		{zzendtype((char *)$4,zztype);}
	;

begdmn:	  zzDOMAIN zzBEGIN
		{zztype=zzDOMAIN; zzkeyword=zzBEGIN;}
	  name
		{zzbegintype((char *)$4,zztype);}
	  | begdmn att
	;

dmninf:	  iolist rowlst chan
	| {zzkeyword=zzGATE;}
	  log ';'
	|
	;

iolist:	  zzIOLIST
		{zzkeyword = zzIOLIST;
			  zzmakeiolist(zzHEAD,-1,-1);}
	  loios ';'
		{zzmakeiolist(zzTAIL,-1,-1);}
	|
	;

loios:	  ios ios
	| loios ios
	;

ios:	  name
		{zzmakeiolist(zzSIGNAL,$1,-1);}
	  iosig
	| ios att
	;

iosig:	  zzRIGHT
		{zzmakeiolist(zzRIGHT,-1,-1);}
	| zzLEFT
		{zzmakeiolist(zzLEFT,-1,-1);}
	| zzTOP
		{zzmakeiolist(zzTOP,-1,-1);}
	| zzBOT
		{zzmakeiolist(zzBOT,-1,-1);}
	| tbrl point
		{zzmakeiolist(zzXY,$2,$1);}
	| tbrl '(' integer '-' integer ',' integer ')'
		{three[0] = $3; three[1] = $5; three[2] = $7;
			  zzmakeiolist(zzX1X2Y,(int)three,$1);}
	| tbrl '(' integer ',' integer '-' integer ')'
		{three[0] = $3; three[1] = $5; three[2] = $7;
			  zzmakeiolist(zzXY1Y2,(int)three,$1);}
	| tbrl '(' integer '-' integer ',' integer '-' integer ')'
		{four[0] = $3; four[1] = $5; four[2] = $7; four[3] = $9;
			  zzmakeiolist(zzX1X2Y1Y2,(int)four,$1);}
	|
	;

rowlst:	  row
	| rowlst row
	;

row:	  rowatt
		{zzkeyword=zzGATE;}
	  log ';'
		{zzmakerowlist(zzTAIL,-1);}
	;

rowatt:	  chan zzROW
		{zzmakerowlist(zzHEAD,-1); zzkeyword=zzROW;}
	  name
		{zzmakerowlist(zzNAME,$4);}
	| rowatt att
	;

chan:	  zzCHANNEL
		{zzkeyword=zzCHANNEL;}
	  name
		{zzfindchan((char *)$3);}
	|
	;

log:	  gate
	| log gate
	;

gate:	  name
		{strcpy(wordpair[0],(char *)$1);}
	  name
		{strcpy(wordpair[1],(char *)$3); 
			  zzmakerowlist(zzGATE,(int)wordpair); zzkeyword=zzSIG;}
	  '(' los ')' 
		{zzkeyword=zzGATE;}
	| gate att
	;

los:	  los1
	| los los1
	;

los1:	  name
		{zzmakerowlist(zzSIGNAL,$1);}
	| name ','
		{zzmakerowlist(zzSIGNAL,$1);}
	;

netstuf:  zzNETS
		{zzkeyword=zzNETS; zzmakenetchrlist(zzHEAD,-1);}
	  lon
	|
	;

lon:	  lon1
	| lon lon1
	;

lon1:	  name 
		{zzmakenetchrlist(zzNAME,$1);}
	  ';'
		{zzmakenetchrlist(zzTAIL,-1);}
	| name 
		{zzmakenetchrlist(zzNAME,$1);}
	  soa ';'
		{zzmakenetchrlist(zzTAIL,-1);}
	;

enddmn:	  zzDOMAIN zzEND
		{zzkeyword=zzEND;}
	  name
		{zzendtype((char *)$4,zztype);}
	;

begchnl:  zzCHANNEL zzBEGIN
		{zztype=zzCHANNEL; zzkeyword=zzBEGIN;}
	  name 
		{zzbegintype((char *)$4,zztype);}
	| begchnl att
	;

trmdesc:  zzTERMLIST
		{zzkeyword=zzTERMLIST; zzmaketermlist(zzHEAD,-1,zztype,-1);}
	  siglst
	|
	;

siglst:	  mkname
	| siglst mkname
	;

mkname:	  snames losot ';'
		{zzmaketermlist(zzTAIL,-1,zztype,-1);}
	;

snames:	  name
		{zzmaketermlist(zzNAME,$1,zztype,-1);}
	| name 
		{strcpy(wordpair[0],(char *)$1);}
	  segname
		{strcpy(wordpair[1],(char *)$3); 
			  zzmaketermlist(zzNAMES,(int)wordpair,zztype,-1);}
	| snames att
	;

segname:  zzID
		{strcpy(word,(char *)$1); $$=(int)word;}
	| integer
		{itoa($1,word); $$=(int)word;}
	| zzNUMBER
		{strcpy(word,(char *)$1); $$=(int)word;}
	;

losot:	  sot sot
	| losot sot
	;

wirdesc:  zzWIRELIST 
		{zzkeyword=zzwhere=zzWIRELIST; zzmakewirelist(zzHEAD,-1);}
	  wrlst
	|
	;

wrlst:	  wrlst1
	| wrlst wrlst1
	;

wrlst1:	  wnames low ';'
		{zzmakewirelist(zzTAIL,-1);}
	  vlst
	;

wnames:	  name
		{zzmakewirelist(zzNAME,$1);}
	| name 
		{strcpy(wordpair[0],(char *)$1);}
	  name
		{strcpy(wordpair[1],(char *)$3); 
			  zzmakewirelist(zzNAMES,(int)wordpair);}
	| wnames att
	;

low:	  wire
	| low wire
	;

wire:	  '{' ptlst1 '}' | wire att ;

ptlst1:	  point	
		{zzmakewirelist(zzPOINT,$1);}
	  point
		{zzmakewirelist(zzPOINT,$3);}
	| ptlst1 point
		{zzmakewirelist(zzPOINT,$2);}
	;

vlst:	  zzVIALIST
		{zzkeyword=zzVIALIST; zzmakevialist(zzHEAD,-1);}
	  lov ';'
		{zzkeyword=zzWIRELIST;
			 zzmakevialist(zzTAIL,-1);}
	|
	;

lov:	  viapts
	| lov viapts
	;

viapts:	  point 
		{zzmakevialist(zzPOINT,$1);}
	| '{' point
		{zzmakevialist(zzPOINT,$2);}
	  point '}'
		{zzmakevialist(zzPOINT,$4);}
	| viapts att
	;

endchnl:  zzCHANNEL zzEND
		{zzkeyword=zzEND;}
	  name
		{zzendtype((char *)$4,zztype);}
	;

name:	  zzID
		{strcpy(word,(char *)$1); $$=(int)word;}
	| integer
		{itoa($1,word); $$=(int)word;}
	| zzNUMBER
		{strcpy(word,(char *)$1); $$=(int)word;}
	;

profile:  zzPROFILE
		{zzkeyword=zzPROFILE;}
	  proftb profrl
	;

proftb:	  zzTOP
		{zzwhere=zzTOP; zzmakeprof(zzTOP,zztype,zzwhere);}
	  ptlst2 ';' zzPROFILE zzBOT
		{zzwhere=zzBOT; zzmakeprof(zzBOT,zztype,zzwhere);}
	  ptlst2 ';'
	| zzBOT
		{zzwhere=zzBOT; zzmakeprof(zzBOT,zztype,zzwhere);}
	  ptlst2 ';' zzPROFILE zzTOP
		{zzwhere=zzTOP; zzmakeprof(zzTOP,zztype,zzwhere);}
	  ptlst2 ';'
	;

profrl:	  profr
	| profr profl
	| profl
	| profl profr
	|
	;

profl:	  zzPROFILE zzLEFT
		{zzwhere=zzLEFT; zzmakeprof(zzLEFT,zztype,zzwhere);}
	  ptlst2 ';'
	;

profr:	  zzPROFILE zzRIGHT
		{zzwhere=zzRIGHT; zzmakeprof(zzRIGHT,zztype,zzwhere);}
	  ptlst2 ';'
	;

ptlst2:	  point
		{zzmakeprof($1,zztype,zzwhere);}
	  point
		{zzmakeprof($3,zztype,zzwhere);}
	| ptlst2 point
		{zzmakeprof($2,zztype,zzwhere);}
	;

point:	  '(' integer ',' integer ')'
		{pair[0]=$2; pair[1]=$4; $$=(int)pair;}
	;

sot:	  natrmnl
	| '{' trmnl '}'
	| '{' trmnl '}' soa
	;

trmnl:	  natrmnl
	| trmnl natrmnl
	;

soa:	  att
	| soa att
	;

att:	  noatt zzID
		{zzaddatt((char *)$1,zzSTRING,$2,zzkeyword,zztype);}
	| noatt zzNUMBER
		{zzaddatt((char *)$1,zzNUMBER,$2,zzkeyword,zztype);}
	| noatt zzQUOTESTR
		{zzaddatt((char *)$1,zzQUOTESTR,$2,zzkeyword,zztype);}
	| noatt integer
		{zzaddatt((char *)$1,zzINTEGER,$2,zzkeyword,zztype);}
	| noatt point
		{zzaddatt((char *)$1,zzPOINT,$2,zzkeyword,zztype);}
	;

noatt:	  zzATT
		{strcpy(attword,(char *)$1);
			for (zzi=0; attword[zzi]!='='; zzi++);
			attword[zzi] = '\0'; $$=(int)attword;}
	;

natrmnl:  zzRIGHT
		{zzmaketermlist(zzRIGHT,-1,zztype,$1);}
	| zzLEFT
		{zzmaketermlist(zzLEFT,-1,zztype,$1);}
	| zzTOP
		{zzmaketermlist(zzTOP,-1,zztype,$1);}
	| zzBOT
		{zzmaketermlist(zzBOT,-1,zztype,$1);}
	| tbrl point
		{zzmaketermlist(zzXY,$2,zztype,$1);}
	| tbrl '(' integer '-' integer ',' integer ')'
		{three[0] = $3; three[1] = $5; three[2] = $7;
			  zzmaketermlist(zzX1X2Y,(int)three,zztype,$1);}
	| tbrl '(' integer ',' integer '-' integer ')'
		{three[0] = $3; three[1] = $5; three[2] = $7;
			  zzmaketermlist(zzXY1Y2,(int)three,zztype,$1);}
	| tbrl '(' integer '-' integer ',' integer '-' integer ')'
		{four[0] = $3; four[1] = $5; four[2] = $7; four[3] = $9;
			  zzmaketermlist(zzX1X2Y1Y2,(int)four,zztype,$1);}
	| natrmnl att
	;

integer:  zzINTEGER
		{$$=$1;}
	| '-' zzINTEGER
		{$$= -($2);}
	| '+' zzINTEGER
		{$$=$2;}
	;

tbrl:	  whside ':'
	| {$$ = -1;}
	;

whside:	  'T' {zzwhere = zzTOP;}
	| 'B' {zzwhere = zzBOT;}
	| 'R' {zzwhere = zzRIGHT;}
	| 'L' {zzwhere = zzLEFT;}
	;

%%
