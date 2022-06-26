%{
#ifdef MCNC
#include "oasisutils.h"
#include "lexflex.h"
#include "includefile.h"
#else
#include "non_mcnc.h"
#endif

#include "xlatr.h"

#define YYDEBUG 0
#undef DEBUG
#define DEBUG
#undef DEBUG

#define TRACE_YACC
#undef TRACE_YACC

#ifdef DEBUG
#define RECOG(a)  printf(MKSTRING( -> a))
#else
#define RECOG(a)
#endif

#ifdef TRACE_YACC
#define TRACE(a)  printf(MKSTRING(\n verified a))
#else
#define TRACE(a)
#endif

extern int      yylineno;

%}

%union {
	long integer;
	double flpt;
	char *text;
	struct cell *cell_ptr;
	struct instance *inst_ptr;
	struct instance **inst_hnd;
	struct terminal *term_ptr;
}

%token INTEGER, FLOAT, NEWLINE, EQUAL, NRIOS, NRMACROS, NRMODS, NRNETS, NRPINS
%token CELL_COUNT, CHAN_COUNT, ROW_COUNT, SHAPE_HEAD

%type <integer> INTEGER, CELL_COUNT, CHAN_COUNT, ROW_COUNT
%type <flpt> FLOAT
%type <text> NEWLINE, EQUAL, NRIOS, NRMACROS, NRMODS, NRNETS, NRPINS, SHAPE_HEAD
%type <cell_ptr> libfile, cell
%type <inst_ptr> instance
%type <inst_hnd> desfile
%type <term_ptr> term


%start file

%%

file:     libfile | desfile | chnfile | dpcfile ;

/********************************************/

libfile: CELL_COUNT NEWLINE { $$=InitLibrary($1); }
	| libfile cell
	  INTEGER NEWLINE INTEGER NEWLINE INTEGER NEWLINE INTEGER NEWLINE
	  { $$=AddCell2Library($1,$2); }
	;

cell:
	INTEGER	/* #1 cellID */
	INTEGER	/* #2 one    */
	INTEGER	/* #3 nrpins */
							NEWLINE
	SHAPE_HEAD	/* Assumed always 4 corners */ 
							NEWLINE
	INTEGER	/* #7  x0 */
	INTEGER	/* #8  y0 */
	INTEGER	/* #9  x1 */
	INTEGER	/* #10 y1 */
	INTEGER	/* #11 x2 */
	INTEGER	/* #12 y2 */
	INTEGER	/* #13 x3 */
	INTEGER	/* #14 y3 */
							NEWLINE
	{
	    if ($2 != 1) {
		Message("Number '1' expected to follow cellID=%d in line %d"
		    , $1, yylineno);
		Message("Found '%d' instead", $2);
	    }
	    $$ = InitCell($1, $3, $7, $8, $9, $10, $11, $12, $13, $14);
	}
      |	cell
	INTEGER	/* #2  pinID	*/
	INTEGER	/* #3  zero 	*/
	INTEGER	/* #4  llx  	*/
	INTEGER	/* #5  lly  	*/
	INTEGER	/* #6  urx  	*/
	INTEGER	/* #7  ury  	*/
	INTEGER	/* #8  zero 	*/
	INTEGER	/* #9  one  	*/
	INTEGER	/* #10 zero 	*/
							NEWLINE
	INTEGER	/* #12 llx 	*/
	INTEGER	/* #13 lly 	*/
	INTEGER	/* #14 urx 	*/
	INTEGER	/* #15 ury 	*/
	INTEGER	/* #16 zero	*/
	INTEGER	/* #17 255 	*/
	INTEGER	/* #18 zero	*/
	FLOAT	/* #19 zero	*/
	FLOAT	/* #20 zero	*/
	FLOAT	/* #21 zero	*/
							NEWLINE
	{ $$ = AddPin2Cell($1, $2, $4, $5, $6, $7, $12, $13, $14, $15); }
	;

/********************************************/

desfile: 
    NRIOS	/* #1  	*/
    EQUAL	/* #2  	*/
    INTEGER	/* #3  	*/
							NEWLINE
    NRMACROS	/* #5  	*/
    EQUAL	/* #6  	*/
    INTEGER	/* #7  	*/
							NEWLINE
    NRMODS	/* #9  	*/
    EQUAL	/* #10 	*/
    INTEGER	/* #11 	*/
							NEWLINE
    NRNETS	/* #13 	*/
    EQUAL	/* #14 	*/
    INTEGER	/* #15 	*/
							NEWLINE
    NRPINS	/* #17 	*/
    EQUAL	/* #18 	*/
    INTEGER	/* #19 	*/
							NEWLINE
    { $$ = InitInstances($3, $7, $11, $15, $19); }
    | desfile instance { $$ = AddInstance($1, $2); }
    | desfile term { $$ = AddTerminal($1, $2); }
    ;

instance:
	INTEGER	/* #1  counter	*/
	INTEGER	/* #2  refID	*/
	INTEGER	/* #3  master	*/
	INTEGER	/* #4  zero	*/
	INTEGER	/* #5  nrpins	*/
	INTEGER	/* #6  xcoord	*/
	INTEGER	/* #7  ycoord	*/
	INTEGER	/* #8  orient	*/
							NEWLINE
	{ $$=NewInstance($1, $2, $3, $5, $6, $7, $8); }
	;

term:
	INTEGER	/* #1  counter	*/
	INTEGER	/* #2  pinID	*/
	INTEGER	/* #3  netID	*/
	INTEGER	/* #4  zero	*/
							NEWLINE
	{ $$=NewTerminal($1, $2, $3); }
	;

/********************************************/

chnfile: CHAN_COUNT NEWLINE | chnfile INTEGER NEWLINE ;

/********************************************/

dpcfile:
	ROW_COUNT /* #1 number of rows	*/
							NEWLINE
	INTEGER	  /* #3 llx	*/
	INTEGER	  /* #4 lly	*/
	INTEGER	  /* #5 urx	*/
	INTEGER	  /* #6 ury	*/
							NEWLINE
	| dpcfile row
	;

row:
	INTEGER	/* #1  llx	*/
	INTEGER	/* #2  lly	*/
	INTEGER	/* #3  urx	*/
	INTEGER	/* #4  ury	*/
	FLOAT	/* #5  dens	*/
	INTEGER	/* #6  orient	*/
	INTEGER	/* #7  rowID	*/
	FLOAT	/* #8  minden	*/
	FLOAT	/* #9  maxden	*/
							NEWLINE
	;
%%

/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set sw=4:
 */
