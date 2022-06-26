#include "typedef.h"


extern long *mergedNetNumbers;

void PrintVPNRcell(fp, cell, mergeNets, usedOnly, routePins)
FILE *fp;
cellptr cell;
int mergeNets, usedOnly, routePins;
{
    int k, doPads = cell->cellFlags & FLG_IOPAD;
    long xmin = 4 * cell->xmin;
    long ymin = 4 * cell->ymin;
    long xmax = 4 * cell->xmax;
    long ymax = 4 * cell->ymax;
    pinptr pin;

    fprintf(fp, "cell begin c%ld %s scale=0.25\n", cell->cellID
	    , cell->cellFlags & FLG_IOPAD ? "type=PAD" : "type=STANDARD");
    fprintf(fp, "  profile top (%ld,%ld) (%ld,%ld);\n",xmin,ymax,xmax,ymax);
    fprintf(fp, "  profile bot (%ld,%ld) (%ld,%ld);\n",xmin,ymin,xmax,ymin);
    fprintf(fp, "  termlist\n");
    for (pin = cell->cellPins, k=0; k<cell->numPins; k++, pin++) {
	xmin = 4 * pin->xmin + 2;
	ymin = 4 * pin->ymin + 2;
	xmax = 4 * pin->xmax + 2;
	ymax = 4 * pin->ymax + 2;
     /* Print the pin.
      */
	if ((xmin == xmax) && (ymin == ymax)) {
	    xmin -= 1; ymin -= 1;
	    xmax += 1; ymax += 1;

	    if (mergeNets && (pin->pinGroup < pin->pinID)
		    && (pin->pinGroup != UNCONNECTED))
		continue;
	    else if (usedOnly && ((pin->pinFlags & FLG_USED) == 0))
		continue;
	    else if (doPads)
		fprintf(fp, "    Pin%d {(%ld-%ld,%ld-%ld)}", pin->pinID, xmin
			, xmax, ymin, ymax);
	    else {
		if (routePins) {
		    int x = 4 * (cell->xmin + pin->pinGrid) + 2;
		    fprintf(fp, "    Pin%d {(%ld-%ld,%ld) (%ld-%ld,%ld)}"
			    , pin->pinID
			    , x-1, x+1, 4*cell->ymin
			    , x-1, x+1, 4*cell->ymax);
		} else	/* Not route pins */
		    fprintf(fp, "    Pin%d {(%ld-%ld,%ld-%ld)}", pin->pinID
			, xmin, xmax, ymin, ymax);
	    }

	    if ( (pin->pinFlags & FLG_USED) == 0)
		fprintf(fp, " type=IGNORED");

	    if (pin->pinGroup != UNCONNECTED)
		fprintf(fp, " group=%d", pin->pinGroup);

	    fprintf(fp, ";\n");
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
		long x = 4 * (cell->xmin + k) + 2;
		fprintf(fp, "    u%d {(%ld-%ld,%ld) (%ld-%ld,%ld)}", k
			, x-1, x+1, 4*cell->ymin
			, x-1, x+1, 4*cell->ymax);
		fprintf(fp, " pintype=feedthrough;\n");
	    }
	}

    fprintf(fp, "cell end c%ld\n", cell->cellID);

}

static long hangingSignal = 0;

void PrintVPNRinst(fp,netInstance,inst,i, usedOnly, mergeNets)
FILE *fp;
long *netInstance;
instptr inst;
long i;
int usedOnly;	/* If 1, only pins actually used are ever output. */
int mergeNets;	/* If 1, only first of the coinciding pins is output */
{
    cellptr master = inst->master;
    long j, k = 0, np = master->numPins;
    int parenPrinted = 0;

    fprintf(fp, "  c%ld i%ld ", master->cellID, i);
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
	    fprintf(fp, "%c%s%ld", parenPrinted ? ',' : '(', name, net);
	    parenPrinted = 1;
	}
    }
    fprintf(fp, ")");
    if (master->cellFlags & FLG_IOPAD)
	fprintf(fp, " lowerLeft=(%ld,%ld) orient=%d", 4*inst->x, 4*inst->y
		, inst->orient);
    fprintf(fp, "\n");
    if (usedOnly && (master->numUsedPins != k)) {
	Message("Found %ld nets in c%ld i%ld, expected %ld", k
	    , master->cellID, i, master->numUsedPins);
	exit(1);
    }
}

void WriteVPNRnet(fp, netInstance, cktName, usedOnly, mergeNets)
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

    /* Print domain header
     */
    fprintf(fp, "domain begin %s\n", cktName);
    fprintf(fp, "  profile top (%ld,%ld) (%ld,%ld);\n", 4*chip_xmin, 4*chip_ymax
	   , 4*chip_xmax, 4*chip_ymax);
    fprintf(fp, "  profile bot (%ld,%ld) (%ld,%ld);\niolist\n", 4*chip_xmin
	   , 4*chip_ymin, 4*chip_xmax, 4*chip_ymin);

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
		long x, y, interval;
		pinptr pin = master->cellPins + j;
		char side;

		net = mergedNetNumbers[net];

		switch (inst->orient) {
		case 1:
		    x = inst->x + (pin->xmin + pin->xmax)/2;
		    y = inst->y + (pin->ymin + pin->ymax)/2;
		    break;
		case 2:
		    x = inst->x + master->xmax - master->xmin
			- (pin->xmin + pin->xmax)/2;
		    y = inst->y + (pin->ymin + pin->ymax)/2;
		    break;
		case 4:
		    x = inst->x + (pin->xmin + pin->xmax)/2;
		    y = inst->y + master->ymax - master->ymin
			- (pin->ymin + pin->ymax)/2;
		    break;
		case 8:
		    x = inst->x + master->xmax - master->xmin
			- (pin->xmin + pin->xmax)/2;
		    y = inst->y + master->ymax - master->ymin
			- (pin->ymin + pin->ymax)/2;
		    break;
		default:
		    Message("Strange orientation %d of pad instance %ld of %ld"
			, inst->orient, numAllIOs - i - 1, master->cellID);
		    exit (1);
		}
		if (inst->instFlags & FLG_LEFT) {
		    interval = (y - chip_ymin) * 100;
		    interval /= chip_height;
		    side = 'L';
		} else if (inst->instFlags & FLG_RITE) {
		    interval = (y - chip_ymin) * 100;
		    interval /= chip_height;
		    side = 'R';
		} else if (inst->instFlags & FLG_TOP) {
		    interval = (x - chip_xmin) * 100;
		    interval /= chip_width;
		    side = 'T';
		} else if (inst->instFlags & FLG_BOT) {
		    interval = (x - chip_xmin) * 100;
		    interval /= chip_width;
		    side = 'B';
		} else {
		    Message("No chip side for pin %ld, pad instance %ld of %ld"
			, j, numAllIOs - i - 1, master->cellID);
		    exit (1);
		}
		if (interval == 100)
		    interval = 99;
		x *= 4;
		y *= 4;
		fprintf(fp, "n%ld %c:(%ld,%ld) original=(%ld,%ld)\n"
			, net, side, interval, interval+1, x, y);
		netInstance[net] += 1;
	    }
	}
    }

    /* Print cell instances
     */
    fprintf(fp, ";\nrow 1\n");
    for (i=0; i< numInstances; i++)
	PrintVPNRinst(fp, netInstance, allInstances[numInstances-i-1], i
		, usedOnly, mergeNets);
	
    for (i=0; i< numAllIOs; i++)
	PrintVPNRinst(fp, netInstance, allIOs[i], numInstances + i, usedOnly
		, mergeNets);
    /* Print domain ending
     */
    fprintf(fp, ";\ndomain end %s\n", cktName);
    (void) fclose(fp);
}


/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set sw=4:
 */
