#include "typedef.h"


extern long *mergedNetNumbers;

void PrintYALcell(fp, cell, mergeNets, usedOnly, routePins)
FILE *fp;
cellptr cell;
int mergeNets, usedOnly, routePins;
{
    int k, doPads = cell->cellFlags & FLG_IOPAD;
    long xmin = cell->xmin;
    long ymin = cell->ymin;
    long xmax = cell->xmax;
    long ymax = cell->ymax;
    pinptr pin;

    fprintf(fp, "MODULE c%ld;\n  TYPE %s;\n", cell->cellID
	    , cell->cellFlags & FLG_IOPAD ? "PAD" : "STANDARD");
    fprintf(fp, "  DIMENSIONS %ld %ld %ld %ld %ld %ld %ld %ld;\n  IOLIST;\n"
    	,xmax,ymin,xmax,ymax,xmin,ymax,xmin,ymin);
    for (pin = cell->cellPins, k=0; k<cell->numPins; k++, pin++) {
	xmin = pin->xmin; ymin = pin->ymin; xmax = pin->xmax; ymax = pin->ymax;
     /* Print the pin.
      */
	if ((xmin == xmax) && (ymin == ymax)) {
	    if (mergeNets && (pin->pinGroup < pin->pinID)
		    && (pin->pinGroup != UNCONNECTED))
		continue;
	    else if (usedOnly && ((pin->pinFlags & FLG_USED) == 0))
		continue;
	    else if (doPads)
		fprintf(fp, "    Pin%d B %ld.5 %ld.5 0.5 METAL2;\n", pin->pinID
			, (xmin+xmax)/2,(ymin+ymax)/2);
	    else {
		if (routePins) {
		    int x = cell->xmin + pin->pinGrid;
		    fprintf(fp, "    Pin%d B %ld.5 %ld 0.5 METAL2;\n"
			    , pin->pinID, x, cell->ymin);
		    fprintf(fp, "    Pin%d B %ld.5 %ld 0.5 METAL2;\n"
			    , pin->pinID, x, cell->ymax);
		} else	/* Not route pins */
		    fprintf(fp, "    Pin%d B %ld.5 %ld.5 0.5 METAL2;\n", pin->pinID
			    , (xmin+xmax)/2,(ymin+ymax)/2);
	    }
	} else {
	    Message("Do not know how to handle a non-point pin %ld  ", k);
	    Message("in cell %ld", cell->cellID);
	    exit (1);
	}
    }
    if (routePins && !doPads) /* Print feedthroughs */
	for (k=cell->xmax - cell->xmin; k-->0; ) {
	    int pin1 = cell->cellBotPins[k];
	    int pin2 = cell->cellTopPins[k];
	    pinptr pin = cell->cellPins + pin1;
	    if (pin1 != pin2)
		continue;
	    if ((pin1 == UNCONNECTED)
		|| (usedOnly && ((pin->pinFlags & FLG_USED) == 0))
		|| (mergeNets && (pin->pinGroup < pin->pinID)
		    && (pin->pinGroup != UNCONNECTED))) {
		fprintf(fp, "    u%d F %ld.5 %ld 0.5 METAL2;\n", k
			, cell->xmin + k,cell->ymin);
		fprintf(fp, "    u%d F %ld.5 %ld 0.5 METAL2;\n", k
			, cell->xmin + k,cell->ymax);
	    }
	}

    fprintf(fp, "  ENDIOLIST;\nENDMODULE;\n");
}

static long hangingSignal = 0;

void PrintYALinst(fp,netInstance,inst,i, usedOnly, mergeNets)
FILE *fp;
long *netInstance;
instptr inst;
long i;
int usedOnly;	/* If 1, only pins actually used are ever output. */
int mergeNets;	/* If 1, only first of the coinciding pins is output */
{
    cellptr master = inst->master;
    long j, k = 0, np = master->numPins;

    fprintf(fp, "    i%ld c%ld ", i, master->cellID);
    for (j=0; j<np; j++) {
	pinptr pin = master->cellPins + j;
	if (mergeNets && (pin->pinGroup < pin->pinID)
		&& (pin->pinGroup != UNCONNECTED)) {
	    if ( ! (usedOnly && ((pin->pinFlags & FLG_USED) == 0)) )
		k += 1;
	    continue;
	} else if (usedOnly && ((pin->pinFlags & FLG_USED) == 0))
	    continue;
	else {
	    long net = inst->interface[j];
	    char *name;
	    if (net == UNCONNECTED) {
		name = "h";
		net  = hangingSignal ++;
	    } else {
		name = "n";
		net = mergedNetNumbers[net];
		netInstance[net] += 1;
	    }
	    k += 1;
	    fprintf(fp, " %s%ld", name, net);
	}
    }
    fprintf(fp, ";\n");
}

void WriteYALnet(fp, netInstance, cktName, usedOnly, mergeNets)
FILE *fp;
long *netInstance;
char *cktName;	/* Basename of the file to write */
int usedOnly;	/* If 1, only pins actually used are ever output. */
int mergeNets;	/* If 1, only first of the coinciding pins is output */
{
    long chip_width = chip_xmax - chip_xmin;
    long chip_height = chip_ymax - chip_ymin;
    long i;

    hangingSignal = 0;

    /* Print header
     */
    fprintf(fp, "MODULE %s;\n  TYPE PARENT", cktName);
    fprintf(fp, "  DIMENSIONS %ld %ld %ld %ld %ld %ld %ld %ld;\nIOLIST;\n"
    	, chip_xmax, chip_ymin, chip_xmax, chip_ymax, chip_xmin, chip_ymax
	, chip_xmin, chip_ymin);

    /* Print IO locations.
     */
    for (i=0; i< numAllIOs; i++) {
	instptr inst = allIOs[i];
	cellptr master = inst->master;
	long j, np = master->numPins;
	int verticalSide = inst->instFlags & (FLG_LEFT|FLG_RITE);

	for (j=0; j<np; j++) {
	    long net = inst->interface[j];

	    if (net != UNCONNECTED) {
		long x, y;
		pinptr pin = master->cellPins + j;

		net = mergedNetNumbers[net];

		if (verticalSide)
		    fprintf(fp, "    n%ld B %s %ld;\n", net
			, (inst->instFlags & FLG_LEFT) ? "LEFT" : "RIGHT"
			, inst->y);
		else
		    fprintf(fp, "    n%ld B %s %ld;\n", net
			, (inst->instFlags & FLG_TOP) ? "TOP" : "BOTTOM"
			, inst->x);
		netInstance[net] += 1;
	    }
	}
    }
    /* Print cell instances
     */
    fprintf(fp, "  ENDIOLIST;\n  NETWORK;\n");
    for (i=0; i< numInstances; i++)
	PrintYALinst(fp, netInstance, allInstances[numInstances-i-1], i
		, usedOnly, mergeNets);
	
    for (i=0; i< numAllIOs; i++)
	PrintYALinst(fp, netInstance, allIOs[i], numInstances + i, usedOnly
		, mergeNets);

    fprintf(fp, "  ENDNETWORK;\n  PLACEMENT;\n");
    for (i=0; i< numAllIOs; i++) {
	instptr inst = allIOs[i];
	char *xform;
	switch (inst->orient) {
	    case 1:
		xform = "";
		break;
	    case 2:
		xform = "RFLY";
		break;
	    case 4:
		xform = "RFLY ROT180";
		break;
	    case 8:
		xform = "ROT180";
		break;
	    default:
		Message("Bogus orientation %d of pad instance %ld of %ld"
			, inst->orient, i, inst->master->cellID);
		exit (1);
	}
	fprintf(fp, "    i%ld %ld %ld %s;\n", numInstances + i, inst->x, inst->y
		, xform);
    }
    fprintf(fp, "  ENDPLACEMENT;\nENDMODULE;\n");
    (void) fclose(fp);
}


/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set sw=4:
 */
