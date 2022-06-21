/* File name :   getmodule.c    */


/* Getmodule - routines for parsing the standard placement input form
   specified by Preas and Roberts.

   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
*/


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "benchtypes.h"

/* Useful Types */

#define false 0
#define true  1

typedef short Boolean;

Number ConvertToNumber();
char *GetMemory();

/* Global Variables */

char InputField[100];
int LineNumber = 1;

Boolean EOFDetected = false;
Boolean ExpectingEOF;
Boolean LineTerminated;

FILE *InputFile;

/* Constants  */

#define FATAL 1
#define WARNING 2


/* The main routine called to get the next module.  Depending on the
   type of module, the Network List and the Placement List may or may not
   be empty.  */

GetModule(TInputFile, Name, ModType, Dimensions, IOListHead, 
	  NetworkListHead, PlacementListHead, CriticalNetListHead)
FILE *TInputFile;   
NameType Name;
ModuleType *ModType;
CoordPair **Dimensions;
IOList **IOListHead;
NetworkList **NetworkListHead;
PlacementList **PlacementListHead;
CriticalNetList **CriticalNetListHead;
{

    InputFile = TInputFile;
    LineTerminated = false;

    /* Set all the optional return arguments to Null at start */

    *IOListHead          = IOListNull;
    *Dimensions          = CoordPairNull;
    *NetworkListHead     = NetworkListNull;
    *PlacementListHead   = PlacementListNull;
    *CriticalNetListHead = CriticalNetListNull;


    /* Get Module Declaration, return with ENDFILE type if end of file */

    ExpectingEOF = true;
    ReadField(InputField);

    if (EOFDetected) {
	*ModType = ENDFILE;
	return;
    }

    if (!strequal(ModuleKeyword,InputField)) 
	Error("Missing Module Declaration", FATAL);
    

    ExpectingEOF = false;
    ReadField(Name);

    GetLineTerminator();

    /* Get Type */

    ReadField(InputField);

    if (!strequal(TypeKeyword,InputField)) 
	Error("Missing Type Declaration - Must be after MODULE", FATAL);
    
    ReadField(InputField);

    if (strequal(StandardKeyword,InputField))         *ModType = STANDARD;
    else if (strequal(PadKeyword,InputField))         *ModType = PAD;
    else if (strequal(GeneralKeyword,InputField))     *ModType = GENERAL;
    else if (strequal(ParentKeyword,InputField))      *ModType = PARENT;
    else if (strequal(FeedthroughKeyword,InputField)) *ModType = FEEDTHROUGH;

    else Error("Unknown Module Type Specification",FATAL);
    
    GetLineTerminator();

    /* Loop to pick up the next field and process it - either
       width, height, IOList, NetworkList or PlacementList */

    ReadField(InputField);

    while ( !strequal(EndModuleKeyword,InputField)) {

	if (strequal(DimensionKeyword,InputField)) {
	    
	    GetDimensions(Dimensions);
	}

	else if (strequal(IOListKeyword,InputField)) 

	    GetIOList(IOListHead);

	else if (strequal(NetworkKeyword,InputField)) 

	    GetNetworkList(NetworkListHead);

	else if (strequal(PlacementKeyword,InputField)) 

	    GetPlacementList(PlacementListHead);

	else if (strequal(CriticalNetKeyword,InputField)) 

	    GetCriticalNetList(CriticalNetListHead);

	else Error("Unknown Keyword",FATAL);

	ReadField(InputField);
    }

    GetLineTerminator();
}


/* GetDimensions reads in the X Y pairs of co-ordinates of the corners of
   the rectilinear cell */

GetDimensions(Dimensions)
CoordPair **Dimensions;
{
    Boolean EndDetected;
    CoordPair *NewPair, *DimensionListTail;
    char IOField[100];

    *Dimensions = CoordPairNull;

    EndDetected = false;
    while(!EndDetected) {

	/* Read X-coordinate */

	ReadField(IOField);

	if (LineTerminated) {
	    EndDetected = true;
	    LineTerminated = false;
	}

	else {

	    NewPair = (CoordPair *) GetMemory(sizeof(CoordPair));

	    NewPair->X     = ConvertToNumber(IOField);
	    NewPair->Link  = CoordPairNull;

	    /* Read Y-coordinate */

	    ReadField(IOField);
	    if (!LineTerminated || LineTerminated && strlen(IOField) != 0) 

		NewPair->Y     = ConvertToNumber(IOField);

	    else Error("Line Terminated when expected Y Dimension Coordinate ",FATAL);


	    if (*Dimensions == CoordPairNull)  
		*Dimensions = NewPair;
	    else
		DimensionListTail->Link = NewPair;
	    
	    DimensionListTail = NewPair;
	}
    }

}
/* NextValidField returns a number from the next field, which must
   be a valid number */

Number NextValidField()
{
    char IOField[100];

    ReadField(IOField);

    if (!LineTerminated || LineTerminated && strlen(IOField) != 0) 
	return(ConvertToNumber(IOField));
    else
	Error("Line Terminated when expecting number",FATAL);

}


/* Current or Voltage reads the max current and/or voltage specification,
   returning true if the IO Field was one or the other */

Boolean CurrentOrVoltage(KeyField,NewIOPin)
char KeyField[100];
IOList *NewIOPin;
{
    char IOField[100];
    Boolean Found;

    Found = false;

    if (strequal(KeyField,CurrentKeyword)) {
	Found = true;

	NewIOPin->Current = NextValidField();

	ReadField(IOField);
	if (!LineTerminated || LineTerminated && strlen(IOField) != 0) {
	    if (strequal(IOField,VoltageKeyword)) 
		NewIOPin->Voltage = NextValidField();
	    else Error("Invalid Syntax after CURRENT",FATAL);
	}
	GetLineTerminator();

    }
	
    else if (strequal(KeyField,VoltageKeyword)) {
	Found = true;
	NewIOPin->Voltage = NextValidField();
	ReadField(IOField);
	if (!LineTerminated || LineTerminated && strlen(IOField) != 0) {
	    if (strequal(IOField,CurrentKeyword)) 
		NewIOPin->Current = NextValidField();
	    else Error("Invalid Syntax after VOLTAGE",FATAL);
	}
	GetLineTerminator();
    }
	
    return(Found);
}


/* GetIOList reads in the IO List  */

GetIOList(IOListHead) 
IOList **IOListHead;
{
    char IOField[100];
    Boolean EndDetected;
    IOList *NewIOPin;
    IOList *IOListTail;

    *IOListHead = IOListNull;
    
    GetLineTerminator();
    

    /* Main loop to read each pin  */

    EndDetected = false;
    while(!EndDetected) {

	/* Read Signal Name or end of IO List */

	ReadField(IOField);

        if (strequal(EndIOListKeyword,IOField)) {
	    EndDetected = true;
	    GetLineTerminator();
	}

	else {  /* Create a New IO Pin */

	    NewIOPin = (IOList *) GetMemory(sizeof(IOList));

	    NewIOPin->XPosition = NOPOSITION;
	    NewIOPin->YPosition = NOPOSITION;
	    NewIOPin->Layer    = NOLAYER;
	    NewIOPin->Current  = NOCURRENT;
	    NewIOPin->Voltage  = NOVOLTAGE;
	    NewIOPin->Width    = NOWIDTH;
	    NewIOPin->Link     = IOListNull;

	    strcpy(NewIOPin->SignalName,IOField);

	    /* Read Terminal Type */

	    ReadField(IOField);

	    if (strequal(InputTerminal,IOField))         
		NewIOPin->Terminal = I;
	    else if (strequal(OutputTerminal,IOField))         
		NewIOPin->Terminal = O;
	    else if (strequal(BiDirectionTerminal,IOField))         
		NewIOPin->Terminal = B;
	    else if (strequal(PadInputTerminal,IOField))         
		NewIOPin->Terminal = PI;
	    else if (strequal(PadOutputTerminal,IOField))         
		NewIOPin->Terminal = PO;
	    else if (strequal(PadBiTerminal,IOField))         
		NewIOPin->Terminal = PB;
	    else if (strequal(FeedThroughTerminal,IOField))         
		NewIOPin->Terminal = F;
	    else if (strequal(PowerTerminal,IOField))         
		NewIOPin->Terminal = PWR;
	    else if (strequal(GroundTerminal,IOField))         
		NewIOPin->Terminal = GND;

	    else Error("Unknown Terminal Type Specification",FATAL);

	    if (!LineTerminated) {

		ReadField(IOField);

		if (!LineTerminated || LineTerminated && strlen(IOField) != 0){ 

		/* The position is either a side name or an actual x position */ 
		    if (strequal(BottomSide,IOField)) 
			NewIOPin->XPosition = BOTTOM;
		    else if (strequal(RightSide,IOField)) 
			NewIOPin->XPosition = RIGHT;
		    else if (strequal(TopSide,IOField)) 
			NewIOPin->XPosition = TOP;
		    else if (strequal(LeftSide,IOField)) 
			NewIOPin->XPosition = LEFT;
		    
		    else NewIOPin->XPosition = ConvertToNumber(IOField);

		    ReadField(IOField);
		    if (!LineTerminated ||
			LineTerminated && strlen(IOField) != 0) {

			NewIOPin->YPosition = ConvertToNumber(IOField);
			

			ReadField(IOField);
			if (!LineTerminated ||
			    LineTerminated && strlen(IOField) != 0) { 

			    if (!CurrentOrVoltage(IOField,NewIOPin)) {
				NewIOPin->Width = ConvertToNumber(IOField);
				if (!LineTerminated) {

				    ReadField(IOField);

				    if (!LineTerminated ||
					LineTerminated && 
					strlen(IOField) != 0) { 

				      if (strequal(PDiffLayer,IOField)) 
					    NewIOPin->Layer = PDIFF;
				      else if (strequal(NDiffLayer,IOField)) 
					    NewIOPin->Layer = NDIFF;
				      else if (strequal(PolyLayer,IOField)) 
					    NewIOPin->Layer = POLY;
				      else if (strequal(Metal1Layer,IOField)) 
					    NewIOPin->Layer = METAL1;
				      else if (strequal(Metal2Layer,IOField)) 
					    NewIOPin->Layer = METAL2;
				      
				      else Error("Unknown Layer Specification", FATAL);
				      if (!LineTerminated) {
					 ReadField(IOField);

					if (!LineTerminated && 
					    !CurrentOrVoltage(IOField,NewIOPin)) 
					    Error("Extraneous text on IOLIST",FATAL);
					}

				    }
				}
			      }
			    }
			}
		    }
		}

		/* Link the new IO Pin */

		if (*IOListHead == IOListNull) 
		    *IOListHead = NewIOPin;
		else
		    IOListTail->Link = NewIOPin;
		
		IOListTail = NewIOPin;

		LineTerminated = false;
	    }

	}
}


GetNetworkList(NetworkListHead)
NetworkList **NetworkListHead;
{ 
    char NetworkField[100];
    Boolean EndDetected;
    Boolean SignalListEnd;
    NetworkList *NewNetworkEntry, *NetworkListTail;
    SignalList *NewSignal, *SignalListTail;

    *NetworkListHead = NetworkListNull;
    
    GetLineTerminator();

    /* Main loop to read each Network Entry  */

    EndDetected = false;
    while(!EndDetected) {

	/* Read Instance Name or end of Network List */

	ReadField(NetworkField);

        if (strequal(EndNetworkKeyword,NetworkField)) {
	    EndDetected = true;
	    GetLineTerminator();
	}

	else {  /* Create a New Network Entry */

	    NewNetworkEntry = (NetworkList *) GetMemory(sizeof(NetworkList));

            NewNetworkEntry->SignalListHead = SignalListNull;
            NewNetworkEntry->Link           = NetworkListNull;

	    strcpy(NewNetworkEntry->InstanceName,NetworkField);

	    /* Read Module Name */

	    ReadField(NewNetworkEntry->ModuleName);

	    /* Get the signal names and link onto this entry's list */

	    SignalListEnd = false;

	    while (!SignalListEnd) {

		if (LineTerminated) 
		    SignalListEnd = true;
		
		else {
		    ReadField(NetworkField);

		    if (LineTerminated && strlen(NetworkField) == 0)
			SignalListEnd = true;
		    
		    else {

			/* Create a New Signal */

			NewSignal = (SignalList *) GetMemory(sizeof(SignalList));
			NewSignal->Link = SignalListNull;
			strcpy(NewSignal->SignalName,NetworkField);

			/* Link onto this entry's list */

			if (NewNetworkEntry->SignalListHead == NetworkListNull)
			    NewNetworkEntry->SignalListHead = NewSignal;
			else
			    SignalListTail->Link = NewSignal;

			SignalListTail = NewSignal;
	            }


		}

	    }

	    /* Link the new NetworkEntry */

	    if (*NetworkListHead == NetworkListNull) 
		*NetworkListHead = NewNetworkEntry;
	    else
		NetworkListTail->Link = NewNetworkEntry;
	    
	    NetworkListTail = NewNetworkEntry;

	    LineTerminated = false;
	}
    }
}

GetPlacementList(PlacementListHead)
PlacementList **PlacementListHead;
{

    char PlacementField[100];
    Boolean EndDetected;
    PlacementList *NewPlacementEntry, *PlacementListTail;

    *PlacementListHead = PlacementListNull;
    
    GetLineTerminator();

    /* Main loop to read each Placement Entry  */

    EndDetected = false;
    while(!EndDetected) {

	/* Read Instance Name or end of Placement List */

	ReadField(PlacementField);

        if (strequal(EndPlacementKeyword,PlacementField)) {
	    EndDetected = true;
	    GetLineTerminator();
	}

	else {  /* Create a New Placement Entry */

	    NewPlacementEntry = 
		(PlacementList *) GetMemory(sizeof(PlacementList));

            NewPlacementEntry->Reflection = RFLNONE;
            NewPlacementEntry->Rotation  = ROT0;
            NewPlacementEntry->Link      = PlacementListNull;

	    strcpy(NewPlacementEntry->InstanceName,PlacementField);

	    /* Read X Location */

	    ReadField(PlacementField);

            NewPlacementEntry->XLocation = ConvertToNumber(PlacementField);

	    /* Read Y Location */

	    ReadField(PlacementField);

            NewPlacementEntry->YLocation = ConvertToNumber(PlacementField);

	    /* Check for End of Line or reflections, rotations */

	    while (!LineTerminated) {
		ReadField(PlacementField);

		if ( !(LineTerminated && strlen(PlacementField) == 0) ) {

		    if (strequal(NoReflection,PlacementField)) 
			NewPlacementEntry->Reflection = RFLNONE;

		    else if (strequal(YReflection,PlacementField)) 
			NewPlacementEntry->Reflection = RFLY;

		    else if (strequal(Rot0,PlacementField)) 
			NewPlacementEntry->Rotation = ROT0;

		    else if (strequal(Rot90,PlacementField)) 
			NewPlacementEntry->Rotation = ROT90;

		    else if (strequal(Rot180,PlacementField)) 
			NewPlacementEntry->Rotation = ROT180;

		    else if (strequal(Rot270,PlacementField)) 
			NewPlacementEntry->Rotation = ROT270;

		    else Error("Unknown Rotation or Reflection type",FATAL);
		}
	    }

	    LineTerminated = false;

	    /* Link the new placement entry */

	    if (*PlacementListHead == PlacementListNull) 
		*PlacementListHead = NewPlacementEntry;
	    else
		PlacementListTail->Link = NewPlacementEntry;
	    
	    PlacementListTail = NewPlacementEntry;
	}
    }
}



GetCriticalNetList(CriticalNetListHead)
CriticalNetList **CriticalNetListHead;
{

    char CriticalNetField[100];
    Boolean EndDetected;
    CriticalNetList *NewCriticalNetEntry, *CriticalNetListTail;

    *CriticalNetListHead = CriticalNetListNull;
    
    GetLineTerminator();

    /* Main loop to read each CriticalNet Entry  */

    EndDetected = false;
    while(!EndDetected) {

	/* Read Signal Name or end of CriticalNet List */

	ReadField(CriticalNetField);

        if (strequal(EndCriticalNetKeyword,CriticalNetField)) {
	    EndDetected = true;
	    GetLineTerminator();
	}

	else {  /* Create a New CriticalNet Entry */

	    NewCriticalNetEntry = 
		(CriticalNetList *) GetMemory(sizeof(CriticalNetList));

            NewCriticalNetEntry->Link      = CriticalNetListNull;

	    strcpy(NewCriticalNetEntry->SignalName,CriticalNetField);

	    /* Read Critical Net Maximum Length */

	    ReadField(CriticalNetField);

            NewCriticalNetEntry->MaximumLength = 
		   ConvertToNumber(CriticalNetField);

	    GetLineTerminator();

	    LineTerminated = false;

	    /* Link the new CriticalNet entry */

	    if (*CriticalNetListHead == CriticalNetListNull) 
		*CriticalNetListHead = NewCriticalNetEntry;
	    else
		CriticalNetListTail->Link = NewCriticalNetEntry;
	    
	    CriticalNetListTail = NewCriticalNetEntry;
	}
    }
}


/* Support Routines  */

/*  Memory Allocation and De-Allocation Routines */

int fred;
/* char *malloc(); */

char *GetMemory(NumberOfBytes)
int NumberOfBytes;
{
    return(malloc(NumberOfBytes));
}


ReleaseMemory(MemPointer)
char *MemPointer;
{
    free(MemPointer);
}

/* Error is called when an exception occurs - FATAL ones cause
   an exit, while WARNINGS are just printed and execution continues */ 

Error(String,  ErrorType)
char *String;
int ErrorType;
{
    if (ErrorType == WARNING) fprintf(stderr,"Warning: ");
    else  fprintf(stderr,"Fatal Error: ");

    fprintf(stderr,"%s; on line %d of input file\n",String,LineNumber);

    if (ErrorType == WARNING) return;
    else exit(-1);
}


/* ReadField reads a field from the input stream, ignoring comments */

ReadField (Text)
char *Text;
{
    int FieldLength;

    if (LineTerminated) {
	strcpy(Text,"");
	return;
    }

    GetField(Text);

    while (strequal(STARTCOMMENT,Text)) { 

	/* get fields until end of comment */

	GetField(Text);

	while ( !strequal(ENDCOMMENT,Text)) {
	    if (EOFDetected) Error("Non-terminated Comment",FATAL);
	    GetField(Text);
	}
	GetField(Text);
    }

    /* Strip off the line terminator if it is attached to the field,
       and set end of logical line indicator */

    FieldLength = strlen(Text) - 1;

    if (Text[FieldLength] == LineTerminator)  {
	LineTerminated = true;
	Text[FieldLength] = 0;
    }
}


/* GetField reads a field from the input stream.  A field is any set of
   characters delimited by blanks, tabs or newlines.  */

GetField(Text)
char *Text;
{
    int ErrFlag;

    ErrFlag = ScanField(Text);

    if (ErrFlag == EOF && !ExpectingEOF) 
	Error("Unexpected End of File",FATAL);

    else if (ErrFlag == EOF && ExpectingEOF) 
	EOFDetected = true;
}


/* ScanField picks up the next field, whilst counting the number of
   lines that go by */

#define BLANK ' '
#define TAB   '\t'
#define NEWLINE '\n'

ScanField(Text)
char *Text;
{
    char NextChar;
    char *TextP;

    /* Skip over blanks, tabs and newlines, counting newlines */

    NextChar = getc(InputFile);

    if (NextChar == EOF) return(EOF);

    while (NextChar == BLANK || NextChar == TAB || NextChar == NEWLINE) {
	
	if (NextChar == NEWLINE) LineNumber++;
	NextChar = getc(InputFile);
	if (NextChar == EOF) return(EOF);
    }

    TextP = Text;
    *TextP++ = NextChar;

    /* Collect Characters until a white space or end of file */

    NextChar = getc(InputFile);

    while (NextChar != BLANK && NextChar != TAB && NextChar != NEWLINE &&
	   NextChar != EOF) {

	*TextP++ = NextChar;
	NextChar = getc(InputFile);
    }

    *TextP = 0;
    if (NextChar == NEWLINE) LineNumber++;

    return(1);
}



/* ConvertToNumber does the conversion from ascii to a floating point
   number */

Number ConvertToNumber(Text)
char *Text;
{
    Number ReturnValue;
    int ErrFlag;

    ErrFlag = sscanf(Text,"%f",&ReturnValue);

    if (ErrFlag != 1) Error("Illegal Number specified",FATAL);

    return(ReturnValue);
}

/* Get Line Terminator looks for the line terminator as the next
   field if it wasn't already encountered */

GetLineTerminator()
{
    char TermField[100];

    if (LineTerminated) {
	LineTerminated = false;
	return;
    }
    
    else {
	ReadField(TermField);

	if (LineTerminated)  LineTerminated = false;

	else Error("Missing Line Terminator (;)",FATAL);
	
    }
}

/* strequal returns true if the two strings are the same, false otherwise */

strequal(s1,s2)
char *s1,*s2;
{
    if (strcmp(s1,s2) == 0 ) return (true);

    else return(false);
}



