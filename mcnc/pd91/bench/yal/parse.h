
parse.h contains six files separated by #'s 

  1) benchtypes.h - the types and constants
  2) getmodule.c - the parser
  3) writemodule.c - the routine to output a moudle
  4) ptest.c - a simple main routine to read and write modules
  5) makefile - to make ptest
  6) parse.doc - a short file explaining how to use GetModule, WriteModule

All files are separated by lines of #####'s and the file's name.
################################################################################
benchtype.h
################################################################################
/* Benchparse.h contains the data types, structures, and constants
   for the C-language parser of the standard placement program input
   format specified by Preas and Roberts.


   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.

*/


/* The parser is used by calling the procedure

	GetModule(InputFile,
		  Name,
		  Type, 
		  Width, 
		  Height, 
		  IOListHead, 
		  NetworkListHead, 
		  PlacementListHead); 

        FILE *InputFile;   is an already-opened file where the circuit is 
	NameType Name;
	ModuleType *Type;
	Number *Width;
	Number *Height;
	IOList **IOListHead;  pointer to a linked list of IOList entries
	NetworkList **NetworkListHead;  linked list of Network entries
	PlacementList **PlacementListHead; linked list of Placement entries

*/

/* Definition of Name Type  */ 

/* Arbitrarilty Chosen Maximum Length of a Name */

#define MAXNAMESIZE 50

typedef char NameType[MAXNAMESIZE];


/* Definition of Module Type  */

#define STANDARD    1
#define PAD         2
#define GENERAL     3
#define PARENT      4
#define FEEDTHROUGH 5
#define ENDFILE     6

typedef short ModuleType;


/* Definition of "Number" Type  */

#define NOPOSITION -1.0
#define NOWIDTH    -1.0

typedef float Number;


/* Definition of Terminal Type, and Constants  */

#define I  1
#define O  2
#define B  3
#define PI 4
#define PO 5
#define PB 6
#define F  7

typedef short TerminalType;


/* Definition of Side Type and Constants  */

#define BOTTOM 1
#define RIGHT  2
#define TOP    3
#define LEFT   4
#define NOSIDE 5

typedef short SideType;


/* Definition of Layer Type and Constants  */

#define PDIFF  1
#define NDIFF  2
#define POLY   3
#define METAL1 4
#define METAL2 5
#define NOLAYER 6

typedef short LayerType;


/* Definition of the IOList Element and NULL Pointer  */

#define IOListNull 0

typedef struct IOListType {
    NameType SignalName;
    TerminalType Terminal;
    SideType Side;
    Number Position;
    Number Width;
    LayerType Layer;
    struct IOListType *Link;
    } IOList;


/* Definition of the Signal List Element  and NULL Pointer */

#define SignalListNull 0

typedef struct SignalListType {
    NameType SignalName;
    struct SignalListType *Link;
    } SignalList;


/* Definition of the Network List Element and NULL Pointer  */

#define NetworkListNull 0

typedef struct NetworkListType {
    NameType InstanceName;
    NameType ModuleName;
    SignalList *SignalListHead;
    struct NetworkListType *Link;
    } NetworkList;


/* Definition of Reflection Type and Constants */

#define RFLNONE 1
#define RFLY    2

typedef short ReflectionType;


/* Definition of Rotation Type and Constants */

#define ROT0    1
#define ROT90   2
#define ROT180  3
#define ROT270  4

typedef short RotationType;


/* Definition of the Placement List Element and NULL Pointer */

#define PlacementListNull 0

typedef struct PlacementListType {
    NameType InstanceName;
    Number XLocation;
    Number YLocation;
    ReflectionType Reflection;
    RotationType Rotation;
    struct PlacementListType *Link;
    } PlacementList;



/* Character strings that define the key words and comments */

/* Comment Delimiters */

#define STARTCOMMENT "/*"
#define ENDCOMMENT "*/"

/* End of entry delimeter */

#define LineTerminator ';'

/* Module Parameters */

#define ModuleKeyword       "MODULE"
#define EndModuleKeyword    "ENDMODULE"
#define TypeKeyword         "TYPE"
#define WidthKeyword        "WIDTH"
#define HeightKeyword       "HEIGHT"

/* IOList Delimiters */
#define IOListKeyword       "IOLIST"
#define EndIOListKeyword    "ENDIOLIST"

/* Network Delimiters */
#define NetworkKeyword      "NETWORK"
#define EndNetworkKeyword   "ENDNETWORK"

/* Placement Delimiters */
#define PlacementKeyword    "PLACEMENT"
#define EndPlacementKeyword "ENDPLACEMENT"

/* Module Types */
#define StandardKeyword     "STANDARD"
#define PadKeyword          "PAD"
#define GeneralKeyword      "GENERAL"
#define ParentKeyword       "PARENT"
#define FeedthroughKeyword  "FEEDTHROUGH"

/* IOList Terminal Types */
#define InputTerminal       "I"
#define OutputTerminal      "O"
#define BiDirectionTerminal "B"
#define PadInputTerminal    "PI"
#define PadOutputTerminal   "PO"
#define PadBiTerminal       "PB"
#define FeedThroughTerminal "F"

/* Side Types */
#define BottomSide "BOTTOM"
#define RightSide  "RIGHT"
#define TopSide    "TOP"
#define LeftSide   "LEFT"

/* Layer Types */
#define PDiffLayer   "PDIFF"
#define NDiffLayer   "NDIFF"
#define PolyLayer    "POLY"
#define Metal1Layer  "METAL1"
#define Metal2Layer  "METAL2"

/* Reflections */
#define NoReflection "RFLNONE"
#define YReflection  "RFLY"

/* Rotations */
#define Rot0   "ROT0"
#define Rot90  "ROT90"
#define Rot180 "ROT180"
#define Rot270 "ROT270"
################################################################################
getmodule.c
################################################################################
/* Getmodule - routines for parsing the standard placement input form
   specified by Preas and Roberts.

   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
*/


#include <stdio.h>
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

GetModule(TInputFile, Name, ModType, Width, Height, IOListHead, 
	  NetworkListHead, PlacementListHead)

FILE *TInputFile;   
NameType Name;
ModuleType *ModType;
Number *Width;
Number *Height;
IOList **IOListHead;
NetworkList **NetworkListHead;
PlacementList **PlacementListHead;

{

    InputFile = TInputFile;
    LineTerminated = false;

    /* Set all the optional return arguments to Null at start */

    *Width  = 0.0;
    *Height = 0.0;

    *IOListHead        = IOListNull;
    *NetworkListHead   = NetworkListNull;
    *PlacementListHead = PlacementListNull;


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

	if (strequal(WidthKeyword,InputField)) {
	    
	    ReadField(InputField);

	    *Width = ConvertToNumber(InputField);
	
	    GetLineTerminator();
	}

	else if (strequal(HeightKeyword,InputField)) {
	
	    ReadField(InputField);

	    *Height = ConvertToNumber(InputField);
	    
	    GetLineTerminator();
	}

	else if (strequal(IOListKeyword,InputField)) 

	    GetIOList(IOListHead);

	else if (strequal(NetworkKeyword,InputField)) 

	    GetNetworkList(NetworkListHead);

	else if (strequal(PlacementKeyword,InputField)) 

	    GetPlacementList(PlacementListHead);

	else Error("Unknown Keyword",FATAL);

	ReadField(InputField);
    }

    GetLineTerminator();
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

	    NewIOPin->Side     = NOSIDE;
	    NewIOPin->Position = NOPOSITION;
	    NewIOPin->Layer    = NOLAYER;
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

	    else Error("Unknown Terminal Type Specification",FATAL);

	    if (!LineTerminated) {

		ReadField(IOField);

		if (!LineTerminated || LineTerminated && strlen(IOField) != 0){ 

		    if (strequal(BottomSide,IOField)) 
			NewIOPin->Side = BOTTOM;
		    else if (strequal(RightSide,IOField)) 
			NewIOPin->Side = RIGHT;
		    else if (strequal(TopSide,IOField)) 
			NewIOPin->Side = TOP;
		    else if (strequal(LeftSide,IOField)) 
			NewIOPin->Side = LEFT;
		    
		    else Error("Unknown Side Type Specification",FATAL);

		    if (!LineTerminated) {

			ReadField(IOField);

			if (!LineTerminated ||
			    LineTerminated && strlen(IOField) != 0) { 

			    NewIOPin->Position = ConvertToNumber(IOField);

			    if (!LineTerminated) {

				ReadField(IOField);

				if (!LineTerminated ||
				    LineTerminated && strlen(IOField) != 0) { 

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

					  GetLineTerminator();
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


/* Support Routines  */

/*  Memory Allocation and De-Allocation Routines */

int fred;
char *malloc();

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



################################################################################
writemodule.c
################################################################################
/* Writemodule - routines for printing out a module in the format
   specified by Preas and Roberts.  
 
   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
 */


#include <stdio.h>
#include <strings.h>
#include "benchtypes.h"


FILE *OutputFile;


WriteModule(TOutputFile, Name, ModType, Width, Height, IOListHead,
	  NetworkListHead, PlacementListHead)


FILE *TOutputFile;   
NameType Name;
ModuleType ModType;
Number Width;
Number Height;
IOList *IOListHead;
NetworkList *NetworkListHead;
PlacementList *PlacementListHead;

{
    IOList *IOListPin;
    NetworkList *CurrentNetworkEntry;
    SignalList *CurrentSignal;
    PlacementList *CurrentPlacementEntry;


    OutputFile = TOutputFile;

    fprintf(OutputFile,"\n%s %s;\n",ModuleKeyword,Name);

    fprintf(OutputFile,"   %s ",TypeKeyword);

    switch(ModType) {

    case STANDARD:
	fprintf(OutputFile,"%s",StandardKeyword);
	break;

    case PAD:
	fprintf(OutputFile,"%s",PadKeyword);
	break;

    case GENERAL:
	fprintf(OutputFile,"%s",GeneralKeyword);
	break;

    case PARENT:
	fprintf(OutputFile,"%s",ParentKeyword);
	break;

    case FEEDTHROUGH:
	fprintf(OutputFile,"%s",FeedthroughKeyword);
	break;

    default:
	fprintf(OutputFile,"UNKNOWN!!!");
	break;
    }

    fprintf(OutputFile,";\n");

    if (Width != 0.0)
	fprintf(OutputFile,"   %s %1.1f;\n",WidthKeyword,Width);

    if (Height != 0.0)
	fprintf(OutputFile,"   %s %1.1f;\n",HeightKeyword,Height);

    IOListPin = IOListHead;

    if (IOListHead != IOListNull) fprintf(OutputFile,"   %s;\n",IOListKeyword);

    while (IOListPin != IOListNull) {
	
	fprintf(OutputFile,"      %s ",IOListPin->SignalName);

	switch(IOListPin->Terminal) {

	case I:
	    fprintf(OutputFile,"%s",InputTerminal);
	    break;

	case O:
	    fprintf(OutputFile,"%s",OutputTerminal);
	    break;

	case B:
	    fprintf(OutputFile,"%s",BiDirectionTerminal);
	    break;

	case PI:
	    fprintf(OutputFile,"%s",PadInputTerminal);
	    break;

	case PO:
	    fprintf(OutputFile,"%s",PadOutputTerminal);
	    break;

	case PB:
	    fprintf(OutputFile,"%s",PadBiTerminal);
	    break;

	case F:
	    fprintf(OutputFile,"%s",FeedThroughTerminal);
	    break;

	default:
	    fprintf(OutputFile,"UNKNOWN!!! ");
	    break;
	}

	if (IOListPin->Side != NOSIDE) {

	    switch (IOListPin->Side) {

	    case BOTTOM:
	      fprintf(OutputFile," %s",BottomSide);
	      break;

	    case RIGHT:
	      fprintf(OutputFile," %s",RightSide);
	      break;

	    case TOP:
	      fprintf(OutputFile," %s",TopSide);
	      break;

	    case LEFT:
	      fprintf(OutputFile," %s",LeftSide);
	      break;

	    default:
	      fprintf(OutputFile,"UNKNOWN!!!");
	      break;
	    }


	    if (IOListPin->Position != NOPOSITION) {
		fprintf(OutputFile," %1.1f",IOListPin->Position);

		if (IOListPin->Width != NOWIDTH) {
		    fprintf(OutputFile," %1.1f",IOListPin->Width);

		    if (IOListPin->Layer != NOLAYER) {

			
			switch (IOListPin->Layer) {

			case PDIFF:
			    fprintf(OutputFile," %s", PDiffLayer);
			    break;

			case NDIFF:
			    fprintf(OutputFile," %s", NDiffLayer);
			    break;

			case POLY:
			    fprintf(OutputFile," %s", PolyLayer);
			    break;

			case METAL1:
			    fprintf(OutputFile," %s", Metal1Layer);
			    break;

			case METAL2:
			    fprintf(OutputFile," %s", Metal2Layer);
			    break;

			default:
			    fprintf(OutputFile," UNKNOWN LAYER!!");
			    break;
			}
		    }
		}
	    }
	}


	fprintf(OutputFile,";\n");
	IOListPin = IOListPin->Link;
    }

    if (IOListHead != IOListNull) 
	fprintf(OutputFile,"   %s;\n",EndIOListKeyword);
    
    if (NetworkListHead != NetworkListNull) {

	fprintf(OutputFile,"   %s;\n",NetworkKeyword);

	CurrentNetworkEntry = NetworkListHead;

	while (CurrentNetworkEntry != NetworkListNull) {

	    fprintf(OutputFile,"     %s %s",CurrentNetworkEntry->InstanceName,
		CurrentNetworkEntry->ModuleName);

	    CurrentSignal = CurrentNetworkEntry->SignalListHead;

	    while(CurrentSignal != SignalListNull) {
		
		fprintf(OutputFile," %s",CurrentSignal->SignalName);

		CurrentSignal = CurrentSignal->Link;
	    }

	    fprintf(OutputFile,";\n");

	    CurrentNetworkEntry = CurrentNetworkEntry->Link;
	}

	fprintf(OutputFile,"   %s;\n",EndNetworkKeyword);
    }

    if (PlacementListHead != PlacementListNull) {

	fprintf(OutputFile,"   %s;\n",PlacementKeyword);

	CurrentPlacementEntry = PlacementListHead;

	while (CurrentPlacementEntry != PlacementListNull) {

	    fprintf(OutputFile,"     %s %1.1f %1.1f",
		CurrentPlacementEntry->InstanceName,
		CurrentPlacementEntry->XLocation,
		CurrentPlacementEntry->YLocation);
	    
	    switch(CurrentPlacementEntry->Reflection) {

	    case RFLNONE:
		break;

	    case RFLY:
		fprintf(OutputFile," %s",YReflection);
		break;

	    default:
		fprintf(OutputFile," UNKNOWN REFLECTION");
		break;
	    }
	       
	    switch(CurrentPlacementEntry->Rotation) {

	    case ROT0:
		break;
	    
	    case ROT90:
		fprintf(OutputFile," %s",Rot90);
		break;

	    case ROT180:
		fprintf(OutputFile," %s",Rot180);
		break;

	    case ROT270:
		fprintf(OutputFile," %s",Rot270);
		break;

	    default:
		fprintf(OutputFile," UNKNOWN ROTATION");
		break;
	    }

	    fprintf(OutputFile,";\n");
		

	    CurrentPlacementEntry = CurrentPlacementEntry->Link;
	}

	fprintf(OutputFile,"   %s;\n",EndPlacementKeyword);
    }

    
    fprintf(OutputFile,"   %s;\n",EndModuleKeyword);
}

################################################################################
ptest.c
################################################################################
/* ptest - test the Preas/Roberts placement language parser 

   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
*/

#include <stdio.h>
#include <strings.h>
#include "benchtypes.h"

#define false 0
#define true  1

FILE *CheckOpen();

main(argc,argv) 
char *argv[];

{ 
    FILE *InputFile, *OutputFile;

    NameType Name;
    ModuleType ModType;
    Number Width;
    Number Height;
    IOList *IOListHead;
    NetworkList *NetworkListHead;
    PlacementList *PlacementListHead;
    short Done;
    int ModuleNumber;


    if (argc < 2 ) 
	printf("Usage: ptest inputfile [outputfile]");


    /* Open input and output files */

    InputFile = CheckOpen(argv[1],"r");

    if (strlen(argv[2]) != 0)
	OutputFile = CheckOpen(argv[2],"w");
    else
	OutputFile = stdout;

    Done = false;
    for(ModuleNumber = 1; !Done; ModuleNumber++) {

	GetModule(InputFile, Name, &ModType, &Width, &Height, &IOListHead, 
		  &NetworkListHead, &PlacementListHead);
	
	if (ModType == ENDFILE) Done = true;

	else {
	    printf("Got Module %d\n",ModuleNumber);

	    WriteModule(OutputFile, Name, ModType, Width, Height, IOListHead, 
		      NetworkListHead, PlacementListHead);
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

################################################################################
makefile
################################################################################
CFLAGS=-g 

ptest: ptest.o getmodule.o writemodule.o 
	cc ${CFLAGS} -o ptest ptest.o getmodule.o writemodule.o 

ptest.o: ptest.c benchtypes.h
	cc ${CFLAGS} -c ptest.c

getmodule.o: getmodule.c benchtypes.h
	cc ${CFLAGS} -c getmodule.c

writemodule.o: writemodule.c benchtypes.h
	cc ${CFLAGS} -c writemodule.c

################################################################################
parse.doc
################################################################################
		Preas-Roberts Placement Language Parser Documentation 

Copyright (C) 1987 by Jonathan Rose.  All rights reserved.

This document describes the Preas-Roberts placement language parser.
The parser is written in the C language.


There is only one routine used to interface to the parser: GetModule.
This routine reads from a specified input file and provides all of the
information contained in the next "MODULE" in that file. 

The calling sequence is as follows:

	#include "benchtypes.h"
	GetModule(InputFile, Name, ModType, Width, Height, IOListHead, 
		  NetworkListHead, PlacementListHead);

The data types are:

	FILE *InputFile;   
	NameType Name;
	ModuleType *ModType;
	Number *Width;
	Number *Height;
	IOList **IOListHead;
	NetworkList **NetworkListHead;
	PlacementList **PlacementListHead;


The data types used (aside from FILE, which is in <stdio.h>) are all described
in the file benchtypes.h.  There are constants associated with some of the
types, and are also given in that file.


Inputs to GetModule:
--------------------

InputFile is a an inppointer to a file that is already opened by fopen.

Outputs from GetModule:
-----------------------

Name is character string that is the name of the Module.

ModType is one of 6 constants, defined in benchtypes.h, that indicated
the type of module.  The constant names are:
        STANDARD    
        PAD        
        GENERAL   
        PARENT   
        FEEDTHROUGH
        ENDFILE
The constant "ENDFILE" indicates that the end of file was reached and that 
no module was read.

The Width and Height are as given in the corresponding line.  If Width and
Height are not specified, they are set to 0.

IOListHead is a pointer to a pointer to the IOList type defined in
benchtypes.h.  It is the head pointer of a linked list of IO pin 
entries for the module.  Each entry contains:
1) SignalName - a string.
2) Terminal Type - one of the constants I,O,B,PI,PO,PB,F.
3) Side Type - one of the constants BOTTOM,LEFT,TOP,RIGHT. 
   or is set to the constant NOSIDE if none was specified.
4) Position - a floating number if specified.  The constant NOPOSITION if not.
5) Width - a floating number if specified.  The constant NOWIDTH if not.
6) Layer - one of the constants PDIFF,NDIFF,POLY,METAL1,METAL2 if specified.
   or is set to the constant NOLAYER if not specified.
7) Link - a pointer to the next element in the IOList.  If this is the
   last then it is set to IOListNull.
The pointer *IOListHead is set to IOListNull if an IOList not is given.

NetworkListHead is a pointer to a pointer to the NetworkList type defined in
benchtypes.h.  It is the head of a linked list of network entries, each
containing:
1) InstanceName - a string.
2) ModuleName - a string.
3) SignalName - the head of linked list of signal names.
4) Link - pointer to the next element. Set to NetworkListNull if it is
   the last element.  
The pointer *NetworkListHead is set to NetworkListNull if no Network was
given in the module definition.

PlacementListHead is a pointer to a pointer to the PlacementList type defined
in benchtypes.h  It is the head of a linked list of placement entries, each
containing:
1) InstanceName - a string.
2) XLocation - a floating point number.
3) YLocation - a floating point number.
4) Reflection - one of the constants RFLNONE or RFLY. (default RFLNONE)
5) Rotation - one of the constants ROT0,ROT90,ROT180,ROT270. (default ROT0)
6) Link - pointer to the next element.  PlacementListNull if is last.
The pointer *PlacementListHead is set to PlacementListNull if no Placement was
given in the module definition.


____________________________________________________________________________
Notes:

1) The parser only checks syntax.  If a syntax error is found, it is reported,
   along with the line number, and the program exits.  Thus, unfortunately,
   only one error is found at a time.  The errors are reported on the
   standard error output. 

2) The parser does not do semantic checking - i.e. it doesn't look for 
   duplicate names of modules or pins or any such thing.  It doesn't check
   to see if, a "CIRCUIT" type module in fact has an NETWORK specified.  All
   this is left to the user.

3) Comments (delimited by /* and */) must have white space around the
   delimiters.  i.e a comment /*Hi there*/ is not allowed, but /* Hi there */
   is.

_________________________________________________________________


The routine WriteModule outputs the Preas-Roberts form of a module,
starting from the form produced by GetModule.

WriteModule(OutputFile, Name, ModType, Width, Height, IOListHead,
	  NetworkListHead, PlacementListHead)


FILE *TOutputFile;   
NameType Name;
ModuleType ModType;
Number Width;
Number Height;
IOList *IOListHead;
NetworkList *NetworkListHead;
PlacementList *PlacementListHead;


Note that the pointers have one less level of indirection than in GetModule,
since this is an output routine.

################################################################################
