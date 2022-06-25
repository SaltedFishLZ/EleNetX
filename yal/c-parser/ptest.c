/* file name : ptest.c   */

/* ptest - test the Preas/Roberts placement language parser 

   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "benchtypes.h"

#define false 0
#define true  1

FILE *CheckOpen();

void main(int argc, char* argv[])
{ 
    FILE *InputFile, *OutputFile;

    /*
    NameType Name;
    ModuleType ModType;
    IOList *IOListHead;
    CoordPair *Dimensions;
    NetworkList *NetworkListHead;
    PlacementList *PlacementListHead;
    CriticalNetList *CriticalNetListHead;
    */
    short Done;
    int ModuleNumber;


    if (argc < 2 ) {
	printf("Usage: ptest inputfile [outputfile]\n");
	exit(-1);
    }


    /* Open input and output files */

    InputFile = CheckOpen(argv[1],"r");

    if (strlen(argv[2]) != 0)
	OutputFile = CheckOpen(argv[2],"w");
    else
	OutputFile = stdout;

    Done = false;
    for(ModuleNumber = 1; !Done; ModuleNumber++) {

    Module mod;
	GetModule(InputFile, mod.Name, &mod.ModType, &mod.Dimensions, &mod.IOListHead, 
		  &mod.NetworkListHead, &mod.PlacementListHead, &mod.CriticalNetListHead);
	
	if (mod.ModType == ENDFILE) Done = true;

	else {
	    printf("Got Module %d\n",ModuleNumber);

	    WriteModule(OutputFile, mod.Name, mod.ModType, mod.Dimensions, mod.IOListHead, 
		      mod.NetworkListHead, mod.PlacementListHead, mod.CriticalNetListHead);
	}
    }
}

FILE *CheckOpen(FileName,OpenType)
char *FileName, *OpenType;
{
    FILE *FPointer;

    FPointer = fopen(FileName,OpenType);

    if (FPointer != NULL) return(FPointer);

    printf("Cannot open file %s\n",FileName);

    exit(-1);

    return(NULL);
}
