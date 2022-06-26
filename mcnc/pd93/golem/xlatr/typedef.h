#ifdef MCNC
#include "portability.h"
#include "unixproto.h"
#include "niceprint.h"
#else
#include "non_mcnc.h"
#define InitNicePrint(a,b,c) printf(b);printf(c)
#define FlushNicePrint(a,b) printf("\n")
#define NicePrint(a,b) printf(b)
#endif

#define UNCONNECTED (-1)

typedef struct pin {
    int pinFlags;
    int pinGroup;		/* Identifies pins sharing the same coords */
    int pinID;			/* Pin number, unique within cell.	   */
    int pinGrid;
    long xmin, xmax ,ymin, ymax;/* Pin rectangle.			   */
} pinrecord, *pinptr;

typedef struct cell {
    int cellFlags;
    int numPins;
    int numUsedPins;	/* Number of pins actually used in the netlist	*/
    long cellID;	/* Cell number, unique within the library.	*/
    long xmin, xmax ,ymin, ymax;	/* Cell bounding box.		*/
    pinptr cellPins;	/* numPins elements				*/
    int *cellTopPins;	/* (xmax-xmin) elements - one per grid point	*/
    int *cellBotPins;	/* (xmax-xmin) elements - one per grid point	*/
} cellrecord, *cellptr;

#define FLG_INVALID	0x001
#define FLG_INPROGRESS	0x002
#define FLG_COMPLETED	0x004
#define FLG_USED	0x008
#define FLG_IOPAD	0x010

#define FLG_LEFT	0x100
#define FLG_RITE	0x200
#define FLG_BOT		0x400
#define FLG_TOP		0x800

typedef struct terminal {
    long pinID;			/* Number of pin in the current cell */
    long netID;			/* Number of the net connected to the cell */
} termrecord, *termptr;

typedef struct instance {
    int instFlags;
    int orient;			/* Orientation of the instance */
    cellptr master;		/* Pointer to the master cell  */
    long x,y;			/* Coordinates of the instance */
    long interface[1];		/* NetIDs of nets connected to all pins.
				 * Unconnected pins have (-1) in their field.
				 */
} instrecord, *instptr;

extern long     numInstances;
extern long     numAllIOs   ;
extern long    *mergedNetNumbers;

extern long     chip_xmin, chip_xmax;
extern long     chip_ymin, chip_ymax;

extern instptr  *allIOs;
extern instptr  *allInstances;
