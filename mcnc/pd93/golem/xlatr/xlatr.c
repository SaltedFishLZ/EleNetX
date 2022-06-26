#include "typedef.h"
#include "xlatr.h"

static long	currentCell = -1;
static cellptr	cellLibrary = NULL;
static long	numLibCells = 0;
static long     numAllPins   = 0;
static long     numAllNets   = 0;

long     numInstances = 0;
long     numAllIOs    = 0;
long    *mergedNetNumbers = NULL;

/* Bounding box of the chip */
long     chip_xmin = LONG_MAX, chip_xmax = LONG_MIN;
long     chip_ymin = LONG_MAX, chip_ymax = LONG_MIN;

static int	doingIOs = 1;	/* Changes to 0 upon finishing reading IOs */

/* Pointers to arrays of pointers to all the IO pads and instances. */
instptr  *allIOs = NULL;
instptr  *allInstances = NULL;

/* Procedures used in the library processing */

struct cell *InitCell (cellID, nrPins, x0, y0, x1, y1, x2, y2, x3, y3)
long cellID;	/* Logical cell ID, between 0 and (numLibCells -1) */
long nrPins;	/* Number of cell's terminals.			   */
long x0, y0, x1, y1, x2, y2, x3, y3; /* Rectangular cell outline   */

/* Returns: pointer to a temporary cell record */
{
    cellptr newCell = (cellptr) malloc(sizeof(*newCell));

    if (newCell == NULL) {
	Message("Run out of memory while trying to allocate cell '%ld'",cellID);
	exit (1);
    }

    newCell->cellFlags = FLG_INPROGRESS;
    newCell->numPins = nrPins;
    newCell->numUsedPins = 0;
    newCell->cellID = cellID;

    {	/* Determine the cell bounding box.
	 */
	long min = LONG_MAX, max = LONG_MIN;
	if (min > x0) min = x0; if (max < x0) max = x0;
	if (min > x1) min = x1; if (max < x1) max = x1;
	if (min > x2) min = x2; if (max < x2) max = x2;
	if (min > x3) min = x3; if (max < x3) max = x3;
	newCell->xmin = min; newCell->xmax = max;

	min = LONG_MAX, max = LONG_MIN;

	if (min > y0) min = y0; if (max < y0) max = y0;
	if (min > y1) min = y1; if (max < y1) max = y1;
	if (min > y2) min = y2; if (max < y2) max = y2;
	if (min > y3) min = y3; if (max < y3) max = y3;
	newCell->ymin = min; newCell->ymax = max;
    }

    {	/* Allocate array for the pin records.
	 */
	pinptr pin;
	newCell->cellPins = pin = (pinptr) calloc((size_t)nrPins, sizeof(*pin));
	if (pin == NULL) {
	    Message("Failed to allocate pins for cell '%ld'", cellID);
	    Message("Could not allocate %ld elements, %ld bytes each"
		    , nrPins, (long) sizeof(*pin));
	    exit (1);
	}
	for ( ; (nrPins--) > 0; pin++ ) {
	    pin->pinFlags = FLG_INVALID;
	    pin->pinGroup = UNCONNECTED;
	}
    }

    {	/* Allocate arrays for numbers of pins routed to the cell boundary.
	 */
	int *aux1, *aux2;
	nrPins = newCell->xmax - newCell->xmin;
	newCell->cellTopPins = aux1
	    = (int*) calloc((size_t) nrPins, sizeof(*newCell->cellTopPins));
	if (aux1 == NULL) {
	    Message("Failed to allocate top pins for cell '%ld'", cellID);
	    Message("Could not allocate %ld elements, %ld bytes each"
		    , nrPins, (long) sizeof(*aux1));
	    exit (1);
	}
	newCell->cellBotPins = aux2
	    = (int*) calloc((size_t) nrPins, sizeof(*newCell->cellBotPins));
	if (aux2 == NULL) {
	    Message("Failed to allocate bottom pins for cell '%ld'", cellID);
	    Message("Could not allocate %ld elements, %ld bytes each"
		    , nrPins, (long) sizeof(*aux2));
	    exit (1);
	}
	for ( ; (nrPins--) > 0; aux1++, aux2++ )
	    *aux1 = *aux2 = UNCONNECTED;     /* Initialize as feed-throughs */
    }

    return newCell;
}

int pinCompFun(pin1,pin2)
pinptr pin1, pin2;
{
    return (pin1->ymin - pin2->ymin);
}

int pinCompFun1(pin1,pin2)
pinptr pin1, pin2;
{
    return (pin1->pinID - pin2->pinID);
}

int assignToGrid(cell, thisPin, offset)
cellptr cell;		/* Cell under consideration.	    */
pinptr  thisPin;	/* Pin to assign.		    */
int offset;		/* Offset from thisPin's coordinate */

/* Returns 1 if assignment successful */
{
    int numGrids = cell->xmax - cell->xmin;
    int pinDesiredHere = (thisPin->xmin + thisPin->xmax) / 2 + offset;

    if (pinDesiredHere < 0)
	return 0;
    if (pinDesiredHere >= numGrids)
	return 0;
    if ( (cell->cellTopPins[pinDesiredHere] == UNCONNECTED) &&
	 (cell->cellBotPins[pinDesiredHere] == UNCONNECTED)) {
	cell->cellTopPins[pinDesiredHere] = cell->cellBotPins[pinDesiredHere]
		= thisPin->pinID;
	thisPin->pinGrid = pinDesiredHere;
	return 1;
    }
    return 0;
}

struct cell *AddCell2Library (library,cell)
struct cell *library;	/* Pointer to the beginning of the cell library */
struct cell *cell;	/* Pointer to the temporary cell record	*/

/* Returns: first argument */
{
    long cellID = cell->cellID;
    cellptr aux = library + cellID;

    if (library == NULL) {
	Message("ERROR - library not allocated while adding cell '%ld'"
		, cell->cellID);
	exit (1);
    } else if (library != cellLibrary) {
	PgmErrorReport("Library mismatch: file '%s' line '%ld'", __FILE__
		, (long) __LINE__);
	exit (1);
    }

    if ((cellID < 0) || (cellID >= numLibCells)) {
	Message("Cell '%ld' outside the permitted range <0,%ld>", cellID
		, numLibCells);
	exit (1);
    }
    if (cell->cellFlags != FLG_COMPLETED) {
	Message("Cell '%ld' does not seem to have all %d pins declared", cellID
		, cell->numPins);
	exit (1);
    }

    /* Process the cell pins and 'route' them to the cell boundary.
     */
    {
	int numGrids = cell->xmax - cell->xmin;
	/* Sort the pins by their vertical coordinates.
	 */
	qsort((char*) (cell->cellPins), (unsigned) cell->numPins
		, (unsigned) sizeof (*(cell->cellPins)), pinCompFun);

	if (numGrids < cell->numPins) {
	    Message("Cell %ld has more pins (%d) than grids (%d)", cell->cellID
		, cell->numPins, numGrids);
	    Message("Cannot handle this yet");
	    exit (1);
	} else {
	    pinptr first=cell->cellPins, last=first + cell->numPins - 1;

	    /* Process the pins that are nearest to their boundary first, and
	     * 'route' them to the nearest free horizontal grid within the cell
	     */
	    while (last >= first) {
		pinptr this;
		int i;

		/* Select the pin to process.
		 */
		if ( (first->ymin - cell->ymin) <= (cell->ymax - last->ymax)) {
		    this = first;
		    first += 1;
		} else {
		    this = last;
		    last -= 1;
		}

		/* Process the pin.
		 */
		for (i=0; i< numGrids; i++)
		    if (assignToGrid(cell, this, i))
			break;
		    else if (assignToGrid(cell, this, -i))
			break;
		if (i == numGrids) {
		    PgmErrorReport("Could not preroute pin %d of cell %ld"
			, this->pinID, cell->cellID);
		    exit(1);
		}
	    }
	}

	/* Sort the pins by their pin IDs to bring them bask into the
	 * original order.
	 */
	qsort((char*) (cell->cellPins), (unsigned) cell->numPins
		, (unsigned) sizeof (*(cell->cellPins)), pinCompFun1);

    }
    *aux = *cell;
    (void) free(cell);
    return library;
}

struct cell *AddPin2Cell (cell, pinID, llx, lly, urx, ury, LLX, LLY, URX, URY)
struct cell *cell;	/* Pointer to the temporary cell record.     */
long pinID;		/* Pin ID, between 0 and (cell->numPins - 1) */
long llx, lly, urx, ury;/* Pin rectangle			     */
long LLX, LLY, URX, URY;/* Repeat of the pin rectangle. 	     */

/* Returns: first argument */
{
    long cellID = cell->cellID;
    pinptr pin = cell->cellPins + pinID;

    if ((pinID < 0) || (pinID >= cell->numPins)) {
	Message("Pin %d in cell '%ld' outside the permitted range <0,%d>"
		, pinID, cellID, cell->numPins);
	exit (1);
    }
    if (cell->cellFlags != FLG_INPROGRESS) {
	Message("Trying to add pin '%d' to a complete cell '%ld'", pinID
		, cellID);
	exit (1);
    }
    if (llx != LLX) {
	Message("Inconsistency in pin %d cell '%ld' llx(%ld) != LLX(%ld)"
		, pinID, cellID, llx, LLX);
	exit (1);
    } else if (lly != LLY) {
	Message("Inconsistency in pin %d cell '%ld' lly(%ld) != LLY(%ld)"
		, pinID, cellID, lly, LLY);
	exit (1);
    } else if (urx != URX) {
	Message("Inconsistency in pin %d cell '%ld' urx(%ld) != URX(%ld)"
		, pinID, cellID, urx, URX);
	exit (1);
    } else if (ury != URY) {
	Message("Inconsistency in pin %d cell '%ld' ury(%ld) != URY(%ld)"
		, pinID, cellID, ury, URY);
	exit (1);
    }

    if (pin->pinFlags % FLG_INVALID) {
	Message("Duplicate pin %d in cell '%ld'", pinID, cellID);
	exit(1);
    } else {
	long min = LONG_MAX, max = LONG_MIN;
	if (min > llx) min = llx; if (max < llx) max = llx;
	if (min > LLX) min = LLX; if (max < LLX) max = LLX;
	pin->xmin = min; pin->xmax = max;

	if ((min < cell->xmin) || (max > cell->xmax)) {
	    Message("Pin %ld located outside the boundary of cell %ld",pinID
		, cellID);
	    exit (1);
	} else if (max == cell->xmax) {
	    Message("Pin %ld located on the right boundary of cell %ld",pinID
		, cellID);
	    Message("Cannot handle such cells in this translator version.");
	    exit (1);
	}

	min = LONG_MAX, max = LONG_MIN;

	if (min > lly) min = lly; if (max < lly) max = lly;
	if (min > LLY) min = LLY; if (max < LLY) max = LLY;
	pin->ymin = min; pin->ymax = max;

	if ((min < cell->ymin) || (max > cell->ymax)) {
	    Message("Pin %ld located outside the boundary of cell %ld",pinID
		, cellID);
	    exit (1);
	} else if (max == cell->ymax) {
	    Message("Pin %ld located on the top boundary of cell %ld",pinID
		, cellID);
	    Message("Cannot handle such cells in this translator version.");
	    exit (1);
	}
    }
    pin->pinFlags = FLG_COMPLETED;
    pin->pinID = pinID;

    if (pinID == (cell->numPins - 1))
	cell->cellFlags = FLG_COMPLETED;

    return cell;
}

struct cell *InitLibrary (numCells)
long numCells;	/* NUmber of cell in the library */

/* Returns: pointer to the array of permanent cell records */
{
    cellptr library, cell;

    numLibCells = numCells;
    cell = library = (cellptr) calloc((size_t) numCells, sizeof(*library));
    if (library == NULL) {
	Message("Library too big");
	Message("Could not allocate %ld elements, %d bytes each"
		, numLibCells, (long) sizeof(*library));
	exit (1);
    }
    for ( ; numCells>0; numCells--, cell++ )
	cell->cellFlags = FLG_INVALID;
    cellLibrary = library;
    return library;
}

int writeDBcells(fp, doPads, routePins, usedOnly, mergeNets,cf)
FILE *fp;	/* File to write to */
int  doPads;	/* If true - write pads. If false - write regular cells */
int routePins;	/* If 1, pins are 'routed' to the cell boundary.  */
int usedOnly;	/* If 1, only pins actually used are ever output. */
int mergeNets;	/* If 1, only first of the coinciding pins is output */
cellFun cf;
{
    static int firstTimeHere = 2;	/* During the first 2 calls, cells 
					 * undergo special counting procedures
					 */
    cellptr cell = cellLibrary;
    long i;
    int singlePinCells = 0;

    for ( i=0; i<numLibCells; i++, cell++) {
	pinptr pin;
	long cw;
	int k;
	int coinciding = 0;
	long xmin = cell->xmin;
	long ymin = cell->ymin;
	long xmax = cell->xmax;
	long ymax = cell->ymax;

	if ((cell->cellFlags & FLG_USED) == 0)
	    continue;

	if (doPads && ((cell->cellFlags & FLG_IOPAD) == 0))
	    continue;
	if ((!doPads) && (cell->cellFlags & FLG_IOPAD))
	    continue;

	for (pin = cell->cellPins, k=0; k<cell->numPins; k++, pin++) {
	    xmin = pin->xmin;
	    ymin = pin->ymin;
	    xmax = pin->xmax;
	    ymax = pin->ymax;
	    if ((firstTimeHere > 0) && (pin->pinFlags & FLG_USED))
		cell->numUsedPins += 1;

	/* Determine which pins coincide and assign them the same pin group.
	 */
	    if ((firstTimeHere > 0) && (pin->pinGroup == UNCONNECTED)) {
		int j;
		for (j=k+1; j<cell->numPins; j++) {
		    pinptr pin1 = cell->cellPins + j;
		    if ((pin1->pinGroup == UNCONNECTED) && (xmin == pin1->xmin)
			&& (ymin == pin1->ymin) && (xmax == pin1->xmax)
			&& (ymax == pin1->ymax)) {
			    pin1->pinGroup = pin->pinGroup = k;
			    coinciding += 1;
		    } 
		}
	    }
	}
	
	if ((firstTimeHere > 0) && (cell->numUsedPins == 1))
	    singlePinCells += 1;
	
	(*cf)(fp, cell, mergeNets, usedOnly, routePins);

	if (coinciding)
	    fprintf(fp, "/* %d coinciding pins in cell %ld */\n", coinciding
		, cell->cellID);
    }
    firstTimeHere -= 1;
    return singlePinCells;
}

void WriteDB(cktName, routePins, usedOnly, mergeNets, cf)
char *cktName;	/* Basename of the file to write */
int routePins;	/* If 1, pins are 'routed' to the cell boundary.  */
int usedOnly;	/* If 1, only pins actually used are ever output. */
int mergeNets;	/* If 1, only first of the coinciding pins is output */
cellFun cf;
{
    int i;
    FILE *fp = fopen(cktName,"w");

    if (fp == NULL) {
	Message("Failed to open file '%s' for writing", cktName);
	exit (1);
    }
    i = writeDBcells(fp, 1, routePins, usedOnly, mergeNets, cf);
    i += writeDBcells(fp, 0, routePins, usedOnly, mergeNets, cf);
    if (i) {
	static int noneYet = 1;
	cellptr aux = cellLibrary;
	register long i;

	Message("The following cells have only a single pin used:");
	InitNicePrint(stderr, "         ", "         ");
	for ( i=0; i<numLibCells; i++, aux++)
	    if (aux->numUsedPins == 1) {
		char buf[128];
		sprintf(buf, "c%ld", i);
		if (!noneYet)
		    NicePrint(stderr, ", ");
		FlushNicePrint(stderr, 10);
		NicePrint(stderr, buf );
		noneYet = 0;
	    }
	FlushNicePrint(stderr, MAX_NICE_LEN);
    }
    (void) fclose(fp);
}


instptr *InitInstances(nrIO, nrMacros, nrInst, nrNets, nrPins)
long nrIO;	/* Number of all IOs in the circuit.			*/
long nrMacros;	/* Number of all macros in the circuit, expected zero.	*/
long nrInst;	/* Number of all cell instances.			*/
long nrNets;	/* Number of all nets in the circuit.			*/
long nrPins;	/* NUmber of all pins in the circuit.			*/

/* Returns: pointer to the location just past the end of the array of
 *          pointers to all IOs.  Subsequent addition of the cell
 *	    instances with AddInstance() backspaces through the array.
 */
{
    if (nrInst <= 0) {
	Message("Nonsensical number of instances (%ld) read.", nrInst);
	exit (1);
    } else
	numInstances = nrInst;

    if (nrMacros != 0) {
	Message("This version of the translator cannot handle macros,");
	Message("and this circuit has %ld macros.", nrMacros);
	exit (1);
    }

    if (nrNets <= 0) {
	Message("Nonsensical number of nets (%ld) read.", nrNets);
	exit (1);
    } else
	numAllNets = nrNets;

    if (nrPins <= 0) {
	Message("Nonsensical number of pins (%ld) read.", nrPins);
	exit (1);
    } else
	numAllPins = nrPins;

    if (nrIO <= 0) {
	Message("Nonsensical number of IOs (%ld) read.", nrIO);
	exit (1);
    } else
	numAllIOs = nrIO;

    allIOs = (instptr*) calloc((size_t) nrIO, sizeof(*allIOs));
    if (allIOs == NULL) {
	Message("Failed to allocate pointers to %ld IOs, %ld bytes each"
		, nrIO, (long) sizeof(*allIOs));
	exit (1);
    }
    allInstances = (instptr*) calloc((size_t) nrInst, sizeof(*allInstances));
    if (allInstances == NULL) {
	Message("Failed to allocate pointers to %ld instances, %ld bytes each"
		, nrInst, (long) sizeof(*allInstances));
	exit (1);
    }
    return (allIOs + nrIO);
}

instptr NewInstance(counter, refID, master, nrpins, x, y, orient)
long  counter;	/* Number of the instance, between 0 and (numInstances - 1)
		 * For IOs it is between 0 and (numAllIOs -1)
		 */
long  refID;	/* Enumerates all instances, IOs and subcells alike.	*/
long  master;	/* Number of the instantiated cell.			*/
long  nrpins;	/* Number of the pins with nets connected to them.	*/
long  x, y;	/* Coords of this instance relative to the lower left corner
		 * of the chip.
		 */
long  orient;	/* Instance orientatios. 1 - normal, 2 - flip horizontally,
		 * 4 - flip vertically, 8- rotate 180 deg.
		 */

/* Returns: a pointer to the instance record. */
{
    instptr newInst;
    cellptr theCell = cellLibrary + master;

/* Make sure this instance makes sense */

    if ((master <0) || (master >= numLibCells)) {
	Message("Instance %ld (ctr=%ld) references nonexistent cell %ld", refID
		, counter, master);
	exit (1);
    }
    if (nrpins > theCell->numPins) {
	Message("Too many pins (%ld) in instance %ld of cell %ld", nrpins
		, refID, master);
	exit (1);
    }
    theCell->cellFlags |= FLG_USED;	/* Mark master cell as used */
    nrpins  = theCell->numPins;		/* Max out the number of pins */

    {	/* Allocate memory for the instance + the max number of pins */
	register size_t instSize = sizeof(*newInst) + nrpins * sizeof(long);

	newInst = (instptr) malloc(instSize);
	if (newInst == NULL) {
	    Message("Run out of memory while trying to allocate instance ");
	    Message("'%ld' of cell %ld", refID, master);
	    exit (1);
	}
    }

    newInst->master = cellLibrary + master;
    newInst->x = x;
    newInst->y = y;
    newInst->orient = orient;
    newInst->instFlags = 0;
    {	/* Initialize all pins to 'unconnected' */
	register int k = nrpins;
	while ( k-- > 0)
	   newInst->interface[k] = UNCONNECTED;
    }

    return newInst;
}

instptr *AddInstance(lastInst, inst)
instptr *lastInst; /* Pointer to the pointer of last processed instance/IO */
instptr  inst;	   /* Pointer to the instance/IO to be added.		   */

/* Returns: pointer to a location of the instance/IO just added */
{
    lastInst -= 1;	/* Backspace */

    if (doingIOs) {
	long x = inst->x;
	long y = inst->y;
	if (lastInst < allIOs) {
	/* We run off the array of IOs.  This means inst is a cell instance.
	 */
	    lastInst = allInstances + numInstances - 1;
	    doingIOs = 0;
	} else {
	/* Mark master cell as the IO pad */
	    inst->master->cellFlags |= FLG_IOPAD;

	/* Adjust bounding box of the chip
	 */
	    if (x < chip_xmin) chip_xmin = x;
	    if (x > chip_xmax) chip_xmax = x;
	    if (y < chip_ymin) chip_ymin = y;
	    if (y > chip_ymax) chip_ymax = y;
	}
    }
    if (! doingIOs) {	/* This is a cell instance */
	cellptr master = inst->master;
	if (lastInst < allInstances) {
	    Message("Inst. of cell %ld is in excess of %ld expected instances"
		, master->cellID, numInstances);
	    Message("Suspect instance location (%ld,%ld), orientation %d"
		, inst->x, inst->y, inst->orient);
	    exit (1);
	} else if (master->cellFlags & FLG_IOPAD) {
	    Message("IO pad %ld also referenced as a cell", master->cellID);
	    exit (1);
	}
    }

    *lastInst = inst;
    return lastInst;
}

termptr NewTerminal(counter, pinID, netID)
long counter;
long pinID;
long netID;
{
    static struct terminal term;
    static long count = 0;

    if (count != counter) {
	Message("Pin count expected %ld, read %ld (pinID=%ld, netID=%ld)"
		, count, counter, pinID, netID);
	exit (1);
    }
    count += 1;

    term.pinID = pinID;
    term.netID = netID;

    return &term;
}

instptr *AddTerminal(lastInst, term)
instptr *lastInst; /* Pointer to pointer to instance that gets a new terminal */
termptr term;	   /* Pointer to the terminal data */

/* Returns: first argument */
{
    long pinID = term->pinID;
    long netID = term->netID;
    char *what;
    long instNum;
    instptr inst = *lastInst;
    cellptr master = inst->master;

    if (doingIOs) {
	what = "IO";
	instNum = numAllIOs - (lastInst - allIOs) - 1;
    } else {
	what = "cell";
	instNum = numInstances - (lastInst - allInstances) - 1;
    }

    if ((netID < 0) || (netID >= numAllNets)) {
	Message("Nonsensical net ID (%ld) for pin %ld of %s instance %ld of %ld"
		, netID, pinID, what, instNum, master->cellID);
	exit (1);
    }

    if ((pinID < 0) || (pinID >= master->numPins)) {
	Message("Nonsensical pin ID (%ld) of %s instance %ld of %ld"
		, pinID, what, instNum, master->cellID);
	exit (1);
    } else if (inst->interface[pinID] != UNCONNECTED) {
	Message("Pin %ld of %s instance %ld of %ld already connected"
		, pinID, what, instNum, master->cellID);
	if (netID == inst->interface[pinID]) {
	    Message("Duplicate specification of net %ld ignored", netID);
	} else {
	    Message("This would short nets %ld and %ld", netID	
		, inst->interface[pinID]);
	    exit (1);
	}
    } else {
	inst->interface[pinID] = netID;
	master->cellPins[pinID].pinFlags |= FLG_USED;
    }
    return lastInst;
}

void ProcesIOpads()
{
    long i;

    /* Look at all IO pads and classify to what side does each belong.
     */
    for (i=0; i< numAllIOs; i++) {
	instptr inst = allIOs[i];
	char *where = NULL;

	if (inst->x == chip_xmin) {
	    if (inst->y == chip_ymin) where = "lower left";
	    if (inst->y == chip_ymax) where = "upper left";
	    inst->instFlags |= FLG_LEFT;
	} else if (inst->x == chip_xmax) {
	    if (inst->y == chip_ymin) where = "lower right";
	    if (inst->y == chip_ymax) where = "upper right";
	    inst->instFlags |= FLG_RITE;
	} else if (inst->y == chip_ymin) {
	    inst->instFlags |= FLG_BOT;
	} else if (inst->y == chip_ymax) {
	    inst->instFlags |= FLG_TOP;
	} else {
	    Message("Pad inst. %ld of %ld (%ld,%ld) not located at the "
		, numAllIOs - i - 1, inst->master->cellID, inst->x, inst->y);
	    Message("boundary (%ld,%ld)-(%ld,%ld)", chip_xmin, chip_ymin
		, chip_xmax, chip_ymax);
	    Message("Current version does not know how to handle such pads");
	    exit (1);
	}
	if (where) {
	    Message("Pad inst. %ld of %ld located in %s corner"
		, numAllIOs - i - 1, inst->master->cellID, where);
	    Message("Current version does not know how to handle such pads");
	    exit (1);
	}
    }

    /* Update the top/right coords by the pad width
     */
    for (i=0; i< numAllIOs; i++) {
	instptr inst = allIOs[i];

	if (inst->instFlags & (FLG_TOP|FLG_RITE)) {
	    cellptr master = inst->master;
	    if (inst->instFlags & FLG_TOP) {
		long y = inst->y + master->ymax - master->ymin;
		if (y > chip_ymax) {
		    Message("Chip Ymax %ld -> %ld", chip_ymax, y);
		    chip_ymax = y;
		}
	    } else {
		long x = inst->x + master->xmax - master->xmin;
		if (x > chip_xmax) {
		    Message("Chip Xmax %ld -> %ld", chip_xmax, x);
		    chip_xmax = x;
		}
	    }
	}
    }

}

void NetStats(netInstance) /* Process net statistics. */
long *netInstance;
{
    long max = 0;
    long *histogram = NULL;
    long i;

    static oddTimesOnly = 1;

    if (oddTimesOnly = 1 - oddTimesOnly)
	return;

    for (i=0; i<numAllNets; i++)
	if (netInstance[i] > max)
	    max = netInstance[i];

    histogram = (long*)calloc((size_t)(max+1), sizeof(*histogram));

    if (histogram == NULL) {
	Message("Failed to allocate net histogram array");
	Message("Could not allocate %ld elements, %ld bytes each"
		, max, (long) sizeof(*histogram));
	exit (1);
    }

    for (i=0; i<numAllNets; i++)
	histogram[netInstance[i]] += 1;
    
    Message("Maximal net cardinality = %ld  ", max);
    for (i=0; i<=max; i++)
	if (histogram[i]) {
	    Message("%ld nets with %ld pins  ", histogram[i], i);
	}
    Message("");

    if (histogram[1]) {
	static int noneYet = 1;
	Message("Suspect nets with cardinality 1:");
	InitNicePrint(stderr, "         ", "         ");
	for (i=0; i<numAllNets; i++)
	    if (netInstance[i] == 1) {
		char buf[128];
		sprintf(buf, "n%ld", i);
		if (!noneYet)
		    NicePrint(stderr, ", ");
		FlushNicePrint(stderr, 10);
		NicePrint(stderr, buf );
		noneYet = 0;
	    }
	FlushNicePrint(stderr, MAX_NICE_LEN);
    }
    (void) free((char*)histogram);
}

void WriteVPNR(cktName, usedOnly, mergeNets, nf)
char *cktName;	/* Basename of the file to write */
int usedOnly;	/* If 1, only pins actually used are ever output. */
int mergeNets;	/* If 1, only first of the coinciding pins is output */
netFun nf;
{
    FILE *fp = fopen(cktName,"w");
    register long i;
    long *netInstance = (long*)calloc((size_t)numAllNets, sizeof(*netInstance));
    long chip_width, chip_height;
    static int firstTimeHere = 1;

    if (netInstance == NULL) {
	Message("Failed to allocate net usage array");
	Message("Could not allocate %ld elements, %ld bytes each"
		, numAllNets, (long) sizeof(*netInstance));
	exit (1);
    }

    if (fp == NULL) {
	Message("Failed to open file '%s' for writing", cktName);
	exit (1);
    }

    if (firstTimeHere) {
	ProcesIOpads();
	mergedNetNumbers = (long*)calloc((size_t) numAllNets
					, sizeof(*mergedNetNumbers));
	if (mergedNetNumbers == NULL) {
	    Message("Failed to allocate merged net array");
	    Message("Could not allocate %ld elements, %ld bytes each"
		    , numAllNets, (long) sizeof(*mergedNetNumbers));
	    exit (1);
	}
	for (i=0; i<numAllNets; i++)
	    mergedNetNumbers[i] = i;
    }
    firstTimeHere = 0;

    (*nf)(fp, netInstance,cktName, usedOnly, mergeNets);

    /* Process net statistics. */
    NetStats(netInstance);
    (void) free((char*)netInstance);
}

void MergeInstanceNets(inst,instNum)
instptr inst;	/* Cell or pad instance to process */
long instNum;
{
    cellptr master = inst->master;
    long j, np = master->numPins;

    for (j=0; j<np; j++) {
	long net = inst->interface[j];		/* Net connected to pin */
	pinptr pin = master->cellPins + j;	/* Pin definition.	*/
	int firstPin = pin->pinGroup;

	if ((firstPin != UNCONNECTED)		/* Mergeable pin.     */
	    && (firstPin < pin->pinID)		/* Not first in group */
	    && (pin->pinFlags & FLG_USED)	/* Pin is usable.     */
	    && (net != UNCONNECTED)		/* Actually connected */
	   ) {
	    /* This is the first pin in the group of pins containing the
	     * current one.
	     */
	    pinptr masterPin = master->cellPins + firstPin;
	    /* This is the net connected to the image of the masterPin
	     * in the current instance.
	     */
	    long masterNet   = inst->interface[firstPin];
	    long oldMaster = masterNet;
	    long oldNet = net;

	    if (masterNet == UNCONNECTED) {
	    /* If the masterPin image is unconnected, connect it to the net
	     * connected to the current pin.
	     */
		inst->interface[firstPin] = net;
		/* Handle the case if the master pin was never connected
		 * to a net.
		 */
		if ((masterPin->pinFlags & FLG_USED) == 0) {
		    masterPin->pinFlags |= FLG_USED;
		    master->numUsedPins += 1;
		}
		continue;	/* No need to merge nets in this case */
	    } else if (masterNet == net)
		continue;	/* Nets identical, no need to merge */

	    /* At this point, we know that the master pin and the current
	     * coinciding pin are connected to different nets.  We first
	     * find the lowest number each of the nets was merged into.
	     * Then we replace the higher one with a lower one.
	     */

	    while (mergedNetNumbers[net] != net)
		net = mergedNetNumbers[net];
	    while (mergedNetNumbers[masterNet] != masterNet)
		masterNet = mergedNetNumbers[masterNet];
#if 0
	    if (masterNet != net)
		Message("Instance %ld, merging%s %ld and %ld", instNum
		, ((oldNet != net)||(masterNet != oldMaster)) ? " parents of":""
		, oldNet, oldMaster);
#endif
	    if (masterNet < net) {
		inst->interface[j] = mergedNetNumbers[net]
			= mergedNetNumbers[oldNet] = mergedNetNumbers[oldMaster]
			= masterNet;
#if 0
		Message("Merged %ld(<-%ld) into %ld(<-%ld)", net, oldNet
			, masterNet, oldMaster);
#endif
	    } else if (masterNet > net) {
		inst->interface[firstPin] = mergedNetNumbers[masterNet]
			= mergedNetNumbers[oldNet] = mergedNetNumbers[oldMaster]
			= net;
#if 0
		Message("Merged %ld(<-%ld) into %ld(<-%ld)", masterNet
			, oldMaster, net, oldNet);
#endif
	    }
	}
    }
}

void MergeNets()
{
    long i, merged;
    long iteration = 0;

    Message("Merging nets on coinciding pins in all I/O pad instances");
    for (i=0; i< numAllIOs; i++)
	MergeInstanceNets(allIOs[i],i);
	
    Message("Merging nets on coinciding pins in all cell instances");
    for (i=0; i< numInstances; i++)
	MergeInstanceNets(allInstances[numInstances-i-1],i);

    /* The original merging information needs to be collapsed so that the
     * multiple-mergers are handled correctly.  I think only one iteration
     * needs to be done, since nets are always merged to the lower number,
     * co by the time we get to a higher number, all lower ones are at their
     * final values, hence by induction the current number will be also set
     * to the final value.  Just in case I messed up, another pass is used
     * to verify the thing.
     */

    do {
	merged = 0;
	iteration += 1;
	for (i=0; i<numAllNets; i++) {
	    long masterNet = mergedNetNumbers[i];
	    if (masterNet != i)	{ /* Net was merged with a lower-numbered one */
		long realMaster = mergedNetNumbers[masterNet];
		if (realMaster != masterNet) {
		    mergedNetNumbers[i] = realMaster;
		    merged = 1;
#if 0
		    Message("Iteration %ld, merged %ld into %ld", iteration
			, i, realMaster);
#endif
		}
	    }
	}
    } while (merged);

    if (iteration > 2) {
	Message("Major algorithm deficiency possible, see 'MergeNets()'");
    }
}

/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set sw=4:
 */
