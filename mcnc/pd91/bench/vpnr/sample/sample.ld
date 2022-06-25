## addtop
/*
 *   This code was developed by individuals connected with MCNC
 *   and Duke University.
 *
 *   Copyright 1988 by the Microelectronics Center of North Carolina       
 *   All rights reserved.
 *
 *   $Source: /mcnc/pi/hill/modgen89/bench/src/sample/RCS/sample.ld,v $
 *   $Date: 89/01/20 09:47:48 $
 *   $Author: kk $
 *   $Revision: 1.1 $
 *
 */

#include <stdio.h>

/* This is a simple example of using 'load' to read some info from the cell
 * database.  The code is meant to be readable, with no emphasis whatsoever
 * on the optimality, diagnostics, foolproofness, and such.
 *
 * The program below will read transistor lists from a database and will
 * print it out in a neat readable form.
 */

/* Declare my custom data structure */

typedef struct transistor {
	char gate[30];
	char drain[30];
	char source[30];
	char name[30];
	double width, length;
	int ttype;
	struct transistor *next;	/* Links all transistors     */
	} trans, *transptr;

/* Declare my global variables */

transptr CurrTransistor=NULL;	/* The most recently read transistor */
transptr AllTransistors=NULL;	/* List of all transistors in a cell */
char CellName[30];		/* Name of the currently read cell   */
char Terminal[30];		/* Name of the currently read terminal */





## attribute translist integer

/* Integer attribute of a transistor has been read. */

if (strcmp(zzw,"width")  == 0)
	CurrTransistor->width = (double) zzn;
else if (strcmp(zzw,"length")  == 0)
	CurrTransistor->length = (double) zzn;

## attribute translist decimal

/* Decimal attribute of a transistor has been read. */

if (strcmp(zzw,"width")  == 0)
	CurrTransistor->width = zzd;
else if (strcmp(zzw,"length")  == 0)
	CurrTransistor->length = zzd;

## attribute translist string

/* String attribute has been read */

if (strcmp(zzw,"type") == 0)
	CurrTransistor->ttype = zzs[0];


## begin cell

strcpy(CellName,zzs);	/* Get the cell name */




## translist addtop

register transptr  newtr;	/*  Auxiliary variable */



## translist head

AllTransistors = NULL;	/* Initialize the list of trnsistor for the cell */




## translist name

/* A transistor has been just read. Initialize the data structure */

newtr = (transptr) malloc(sizeof(*newtr));
newtr->width = newtr->length = 0;
newtr->ttype = '?';
newtr->gate[0] = newtr->source[0] = newtr->drain[0] = 0;
(void) strcpy(newtr->name,zzs);

/* Append in front of the list */

newtr->next = AllTransistors;
AllTransistors = newtr;

/* Store in CurrTransistor to pass the data location for terminals
 * attribute reading.
 */

CurrTransistor = newtr;

## translist gate
strcpy(CurrTransistor->gate,zzw);

## translist source
strcpy(CurrTransistor->source,zzw);

## translist drain
strcpy(CurrTransistor->drain,zzw);

## translist tail

/* The list of transistors has just ended.   Print it out in a nice readable
 * format.
 */

newtr = AllTransistors;
if (newtr == NULL)
	printf ("No transistors found in leaf cell '%s' !\n\n\n",CellName);
else {
	register int num = 1;
	printf ("List of transistors found in leaf cell '%s':\n",CellName);
	for ( ; newtr != NULL; newtr=newtr->next) {
		if ( newtr->ttype != '?')
			printf("\ttr#%d [%s] (%c-type):\n",num++,newtr->name
				,(char)newtr->ttype);
		else
			printf("\ttr#%d [%s] (UNDEFINED-type):\n",num++
				,newtr->name);
		printf("\t\tsource connected to signal '%s'\n",newtr->source);
		printf("\t\tgate connected to signal '%s'\n",newtr->gate);
		printf("\t\tdrain connected to signal '%s'\n",newtr->drain);
		printf("\t\t\tlength:");
		if (newtr->length > 0)
			printf(" %lg units\n",newtr->length);
		else
			printf("ERRONEOUS: negative or never declared\n");
		printf("\t\t\twidth:");
		if (newtr->width > 0)
			printf(" %lg units\n",newtr->width);
		else
			printf("ERRONEOUS: negative or never declared\n");
		printf("\n");
	}
	printf("\n\n");
}

## addend

/* Our program runs as a filter, reading from the standard input and writing
 * to the standard output.
 */

main ()
{
	yyparse();
}


## termlist cell name

strcpy(Terminal,zzs);	/* Get the terminal name */



## attribute terminal string

if (strcmp(zzw,"type") == 0)
	printf("In cell '%s', type of the signal '%s' is '%s'\n",CellName
		,Terminal,zzs);
