%{
/* Copyright 1990 by the Microelectronics Center of North Carolina       
 * All rights reserved.
 */
#include <stdio.h>
#include <strings.h>
#include "y2v.h"

extern int yylineno;
static int portCount;
static lineLen = 0;
%}

%token NAME,MODULE,TYPE,WIDTH,HEIGHT,IOLIST,ENDIOLIST,ENDMODULE,NUMBER,DECIMAL
%token SEMI,NETWORK,ENDNETWORK,DIMENSIONS,CURRENT,VOLTAGE
%start file

%%

file: module | file module ;

module: begin_module
    IOLIST SEMI
    iolist ENDIOLIST SEMI { printInterface(); }
    network
    ENDMODULE SEMI
        { printf("%s end %s\n",ModType,CurrModule); }
    ;

begin_module: start_module
        { addProfilePoint(0,0,CurrCell); addProfilePoint(0,0,CurrCell);
          addProfilePoint(0,0,CurrCell); addProfilePoint(0,0,CurrCell);
        }
    | start_module width_height
    | start_module dimensions;

start_module: MODULE NAME SEMI TYPE NAME SEMI
        { extern cellPtr newCell();
	  CurrModule=(char*)$2;
          if (strcmp($5,"PARENT"))
            ModType = "cell";
          else
            ModType = "domain";
          printf("\n%s begin %s type=%s scale=%g\n",ModType,$2,$5
		,1.0/Scale);
          CurrCell = newCell();
	  addCellAttrib("type",$5);
        } ;

width_height: WIDTH number SEMI HEIGHT number SEMI
        { addProfilePoint(0,0,CurrCell); addProfilePoint($2,0,CurrCell);
          addProfilePoint($2,$5,CurrCell); addProfilePoint(0,$5,CurrCell);
	  printCellProfile(CurrCell);
        } ;

dimensions: DIMENSIONS
	| dimensions point
	| dimensions SEMI
		{ printCellProfile(CurrCell); }
	;

point: number number
	{ addProfilePoint($1,$2,CurrCell); } ;

network:
	| NETWORK SEMI
		{ printf("row 1\n"); }
	  instances ENDNETWORK SEMI
		{ printf(";\n"); }
	;

instances: instance | instances instance ;

instance:
	  NAME NAME { printf("   %s\t%s(",$2,$1); portCount = lineLen = 0; }
	  inputs output SEMI
	  {
	     verifyPortCount(portCount,$1,$2);
	     printf(")\n");
	     
	  };

inputs:
	| inputs NAME { maybeNL($2);  printf("%s,",$2); portCount += 1; }
	| inputs NUMBER { maybeNL($2); printf("%s,",$2); portCount += 1; }
	;

output:	  NAME { maybeNL($1); printf("%s",$1); portCount += 1; }
	| NUMBER { maybeNL($1); printf("%s",$1); portCount += 1; };

iolist: pin SEMI | iolist pin SEMI { /* | pinbeg SEMI | iolist pinbeg SEMI */};

pin:  pinbeg NAME number number NAME    { $$=addLocation1($1,$2,$3,$4,$5); }
    | pinbeg number number number NAME  { $$=addLocation2($1,$2,$3,$4,$5); }
    | pinbeg NAME                       { $$=addDesiredLocation($1,$2);    }
    | pinbeg NAME number                { $$=addLocation1($1,$2,$3,0,NULL);}
    | pin CURRENT DECIMAL               { $$=addIOAttrib($1,"CURRENT",$3);}
    | pin VOLTAGE DECIMAL               { $$=addIOAttrib($1,"VOLTAGE",$3);}
    | pinbeg
    ;

pinbeg: NAME NAME { $$=newIO($1,$2); } ;

number:
      NUMBER  { int tmp;  sscanf($1,"%d",&tmp); $$=scalenumber($1,(double)tmp);}
    | DECIMAL { double tmp;sscanf($1,"%le",&tmp); $$=scalenumber($1,tmp);}
%%

maybeNL(s)
char *s;
{
	int i = strlen(s);
	lineLen += i+1;
	if (lineLen > 75) {
		printf("\n");
		lineLen = i;
	}
}

yyerror(s) char *s;
{
    fprintf(stderr,"%s on line %d.\n",s,yylineno);
    exit (1);
}

#define abs(x) (x>=0 ? x : -(x))

int scalenumber(text,number)
char *text;
double number;
{
	double m = (number > 0) ? 1 : -1;
	double newnumber = (number+m/(2*Scale))*Scale;
	int result = (int) newnumber;
	double check = ((double) result)/((double)Scale);
	double diff = check - number;
	static warn = 1;
	if ((abs(diff) > 0.000001*(abs(check)+abs(number)))
		&& warn) {
		warn = 0;
		fprintf(stderr,"WARNING: scaling factor (%g) too small\n"
			,Scale);
		fprintf(stderr,"         to express %lf as integer multiples\n"
			,number);
		fprintf(stderr,"         of elementary units. Obtained %lf\n"
			,check);
		fprintf(stderr,"         Returned %d (original %s)\n",result
			,text);
	}
	return result;
}
