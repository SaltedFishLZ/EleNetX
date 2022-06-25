/* file name     : writemodule.c */


/* Writemodule - routines for printing out a module in the format
   specified by Preas and Roberts.  
 
   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
 */


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "benchtypes.h"


FILE *OutputFile;


WriteModule(TOutputFile, Name, ModType, Dimensions, IOListHead,
	  NetworkListHead, PlacementListHead,CriticalNetListHead)


FILE *TOutputFile;   
NameType Name;
ModuleType ModType;
CoordPair *Dimensions;
IOList *IOListHead;
NetworkList *NetworkListHead;
PlacementList *PlacementListHead;
CriticalNetList *CriticalNetListHead;

{
    IOList *IOListPin;
    CoordPair *CurrentPair;
    NetworkList *CurrentNetworkEntry;
    SignalList *CurrentSignal;
    PlacementList *CurrentPlacementEntry;
    CriticalNetList *CurrentCriticalNetEntry;


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

    if (Dimensions != CoordPairNull)
	fprintf(OutputFile,"   %s ",DimensionKeyword);

    CurrentPair = Dimensions;
    while(CurrentPair != CoordPairNull) {

	fprintf(OutputFile,"  %1.1f %1.1f",CurrentPair->X, CurrentPair->Y);

	CurrentPair = CurrentPair->Link;
    }

    if (Dimensions != CoordPairNull) fprintf(OutputFile,";\n");

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

	case PWR:
	    fprintf(OutputFile,"%s",PowerTerminal);
	    break;

	case GND:
	    fprintf(OutputFile,"%s",GroundTerminal);
	    break;

	default:
	    fprintf(OutputFile,"UNKNOWN!!! ");
	    break;
	}

	if (IOListPin->XPosition != NOPOSITION) {

	    if (IOListPin->XPosition == BOTTOM) {
	      fprintf(OutputFile," %s",BottomSide);
	    }

	    else if (IOListPin->XPosition == RIGHT) {
	      fprintf(OutputFile," %s",RightSide);
	    }

	    else if (IOListPin->XPosition == TOP) {
	      fprintf(OutputFile," %s",TopSide);
	    }

	    else if (IOListPin->XPosition == LEFT) {
	      fprintf(OutputFile," %s",LeftSide);
	    }

	    else fprintf(OutputFile," %1.1f",IOListPin->XPosition);

	    fprintf(OutputFile," %1.1f",IOListPin->YPosition);
	    }


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

	if (IOListPin->Current != NOCURRENT) 
	    fprintf(OutputFile," %s %1.1f",CurrentKeyword,IOListPin->Current);
	if (IOListPin->Voltage != NOVOLTAGE) 
	    fprintf(OutputFile," %s %1.1f",VoltageKeyword,IOListPin->Voltage);

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



    if (CriticalNetListHead != CriticalNetListNull) {

	fprintf(OutputFile,"   %s;\n",CriticalNetKeyword);

	CurrentCriticalNetEntry = CriticalNetListHead;

	while (CurrentCriticalNetEntry != CriticalNetListNull) {

	    fprintf(OutputFile,"     %s %1.1f",
		CurrentCriticalNetEntry->SignalName,
		CurrentCriticalNetEntry->MaximumLength);
	    
	       
	    fprintf(OutputFile,";\n");
		
	    CurrentCriticalNetEntry = CurrentCriticalNetEntry->Link;
	}

	fprintf(OutputFile,"   %s;\n",EndCriticalNetKeyword);
    }

    
    fprintf(OutputFile,"   %s;\n",EndModuleKeyword);
}
