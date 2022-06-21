/* Copyright 1990 by the Microelectronics Center of North Carolina       
 * All rights reserved.
 */

/* The data structures are rather primitive - they are all linked lists */

#define MAXIMIZE(a,b)   if ((a) < (b)) a = b;
#define MINIMIZE(a,b)   if ((a) > (b)) a = b;

typedef struct ATT   attrib, *attribPtr;
typedef struct PIN   pin, *pinPtr;
typedef struct IO    io, *ioPtr;
typedef struct CELL  cellDef, *cellPtr;
typedef struct POINT point, *pointPtr;

/***********************************************************************/
/* Any attribute of the form <name_string>=<value_string>
 */

struct ATT  { char *name, *value; attribPtr next; } ;

/* A pin in any cell definition.
 */
struct PIN  {
	int xmin, ymin, xmax, ymax;	/* Pin rectangle.	 */
	char *layer;			/* On which layer.	 */
	int flags;			/* Pin flags (see below) */
	pinPtr next;			/* Next pin in the list for a IO */
	ioPtr  io;			/* Pin belongs to this IO.	 */
	attribPtr pinAtt;		/* Pin attributes.	 */
};
#define PIN_BOT		0x01
#define PIN_TOP		0x02
#define PIN_LEFT	0x04
#define PIN_RIGHT	0x08
#define PIN_NO_POS	0x10

/* An I/O port (a set of electrically equivalent pins)
 */
struct IO   {
	char *name;		/* Port name. */
	pinPtr locations;	/* List of pins belonging to the port.	*/
	int flags;		/* Port flags.	*/
	ioPtr next;		/* Next port in a cell.	*/
	attribPtr ioAtt;	/* Port attributes. */
} ;

/* Flags that can be assigned to I/O ports.
 */
#define IO_INPUT    0x0001
#define IO_OUTPUT   0x0002
#define IO_PAD      0x0004
#define IO_FEED     0x0008
#define IO_VDD      0x0010
#define IO_VSS      0x0020

/* A point in a cell outline (for general cells).
 */
struct POINT {
	int x,y;		/* Coordinates */
	pointPtr clock;		/* Clockwise link */
	pointPtr aclock;	/* Anti-clockwise link. */
} ;

/* A cell.
 */
struct CELL {
	char *name;			/* Cell name.		*/
	ioPtr pins;			/* Cell ports.		*/
	int xmin, ymin, xmax, ymax;	/* Cell bounding box.	*/
	int portCount;
	attribPtr cellAtt;		/* Cell attributes.	*/
	pointPtr outline;		/* Cell boundary - sequence of points */
	cellPtr next;
} ;

extern cellPtr CurrCell;
extern char *CurrModule;
extern char *ModType;
extern double Scale;

extern char *malloc();
extern int fprintf();
extern void exit();
extern int sscanf();
extern int printf();
extern int yyparse();
