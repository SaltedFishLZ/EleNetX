/* Copyright 1990 by the Microelectronics Center of North Carolina       
 * All rights reserved.
 */
#include <stdio.h>
#include <strings.h>
#include "y2v.h"

extern int yylineno;

double Scale = 1000;
int noPWR = 0;

static cellPtr allCells = NULL;
cellPtr CurrCell;
char *CurrModule;
char *ModType;

cellPtr cellByName(cellName)
char *cellName;
{
	cellPtr cp = allCells;
	while ((cp != NULL) && (strcmp(cp->name,cellName)))
		cp = cp->next;
	return cp;
}

void verifyPortCount(portCount,instName,cellName)
int portCount;
char *instName,*cellName;
{
     cellPtr cp = cellByName(cellName);
     if ((cp != NULL) && (cp->portCount != portCount))
	fprintf(stderr,"Cell %s(%s) has %d i/o signals instead of %d\n"
		,instName,cp->name,portCount,cp->portCount);
}

void printCellProfile(cell)
cellPtr cell;
{
	pointPtr leftmost,rightmost,pp;
	leftmost = cell->outline;
	while (leftmost->x >= leftmost->clock->x)
		leftmost = leftmost->clock;
	while (leftmost->x >= leftmost->aclock->x)
		leftmost = leftmost->aclock;
	rightmost = cell->outline;
	while (rightmost->x <= rightmost->clock->x)
		rightmost = rightmost->clock;
	while (rightmost->x <= rightmost->aclock->x)
		rightmost = rightmost->aclock;
	printf("profile top ");
	for (pp = leftmost->clock; ; pp = pp->clock) {
		printf("(%d,%d)",pp->x,pp->y);
		if (pp == rightmost)
			break;
		else
			printf(" ");
	}
	while (leftmost->x >= leftmost->clock->x)
		leftmost = leftmost->clock;
	while (rightmost->x <= rightmost->clock->x)
		rightmost = rightmost->clock;
	printf(";\nprofile bot ");
	for (pp = leftmost->aclock; ; pp = pp->aclock) {
		printf("(%d,%d)",pp->x,pp->y);
		if (pp == rightmost)
			break;
		else
			printf(" ");
	}
	printf(";\n");
	pp = cell->outline;
	cell->xmin = cell->xmax = pp->x;
	cell->ymin = cell->ymax = pp->y;
	for (pp=pp->clock; pp != cell->outline; pp=pp->clock) {
		MAXIMIZE(cell->xmax,pp->x);
		MINIMIZE(cell->xmin,pp->x);
		MAXIMIZE(cell->ymax,pp->y);
		MINIMIZE(cell->ymin,pp->y);
	}
}

/* Print IO list.  Give each pin +- 2% slack along the side.
 */

void printIOList()
{
	register ioPtr ip = CurrCell->pins;
	long xmin,xmax,side;

	/* Compute the parent cell size */
	CurrCell->xmin = ip->locations->xmin;
	CurrCell->xmax = ip->locations->xmax;
	CurrCell->ymin = ip->locations->ymin;
	CurrCell->ymax = ip->locations->ymax;
	for (;ip != NULL; ip = ip->next) {
		register pinPtr pp = ip->locations;
		for (; pp != NULL; pp = pp->next)
		    if ((pp->flags & PIN_NO_POS) == 0) {
			int x = (pp->xmin + pp->xmax)/2;
			int y = (pp->ymin + pp->ymax)/2;
			if(CurrCell->xmin > pp->xmin) CurrCell->xmin = x;
			if(CurrCell->xmax < pp->xmax) CurrCell->xmax = x;
			if(CurrCell->ymin > pp->ymin) CurrCell->ymin = y;
			if(CurrCell->ymax < pp->ymax) CurrCell->ymax = y;
		    }
	}
	printf("iolist\n");
	if ((CurrCell->xmin >= CurrCell->xmax) ||
		 (CurrCell->ymin >= CurrCell->ymax)) {
		/* All pins specified with no dimensions - give them full range */
		for (ip=CurrCell->pins;ip != NULL; ip = ip->next) {
			register pinPtr pp = ip->locations;
			for (; pp != NULL; pp = pp->next)
				if (pp->flags & PIN_BOT)
					printf("   %s B:(0,100)\n",ip->name);
				else if (pp->flags & PIN_TOP)
					printf("   %s T:(0,100)\n",ip->name);
				else if (pp->flags & PIN_LEFT)
					printf("   %s L:(0,100)\n",ip->name);
				else if (pp->flags & PIN_RIGHT)
					printf("   %s R:(0,100)\n",ip->name);
		}
		printf(";\n");
		return;
	}
	for (ip=CurrCell->pins;ip != NULL; ip = ip->next) {
		register pinPtr pp = ip->locations;
		for (; pp != NULL; pp = pp->next)
			if (pp->flags & PIN_BOT)
				pp->ymin = pp->ymax = CurrCell->ymin;
			else if (pp->flags & PIN_TOP)
				pp->ymin = pp->ymax = CurrCell->ymax;
			else if (pp->flags & PIN_LEFT)
				pp->xmin = pp->xmax = CurrCell->xmin;
			else if (pp->flags & PIN_RIGHT)
				pp->xmin = pp->xmax = CurrCell->xmax;
	}
	for (ip=CurrCell->pins;ip != NULL; ip = ip->next) {
		register pinPtr pp = ip->locations;
		for (; pp != NULL; pp = pp->next) {
			register attribPtr ap;
			printf("   %s ",ip->name);
			if ((pp->flags & (PIN_TOP|PIN_BOT)) || (pp->xmin != pp->xmax)) {
				xmin = (pp->xmin + pp->xmax)/2;
				xmax = (pp->xmin + pp->xmax)/2;
				side = CurrCell->xmax - CurrCell->xmin;
				if (xmin == xmax) {
					xmin = (xmax - CurrCell->xmin)*100L;
					xmin -= side/50;
					xmin /= side;
					if (xmin < 0)
						xmin = 0;
					xmax = xmin + 4;
					if (xmax > 100) {
						xmax = 100;
						xmin = 96;
					}
				}

				if (pp->ymin == CurrCell->ymin)
					printf("B:(%ld,%ld) width=%d desired=(%d,%d)",xmin,xmax
						,pp->xmax-pp->xmin,(pp->xmin + pp->xmax)/2,CurrCell->ymin);
				else
					printf("T:(%ld,%ld) width=%d desired=(%d,%d)",xmin,xmax
						,pp->xmax-pp->xmin,(pp->xmin + pp->xmax)/2,CurrCell->ymax);
			} else {
				xmin = (pp->ymin + pp->ymax)/2;
				xmax = (pp->ymin + pp->ymax)/2;
				side = CurrCell->ymax - CurrCell->ymin;
				if (xmin == xmax) {
					xmin = (xmax - CurrCell->ymin)*100L;
					xmin -= side/50;
					xmin /= side;
					if (xmin < 0)
						xmin = 0;
					xmax = xmin + 4;
					if (xmax > 100) {
						xmax = 100;
						xmin = 96;
					}
				}
				if (pp->xmin == CurrCell->xmin)
					printf("L:(%ld,%ld) width=%d desired=(%d,%d)",xmin,xmax
						,pp->ymax-pp->ymin,CurrCell->xmin,(pp->ymin + pp->ymax)/2);
				else
					 printf("R:(%ld,%ld) width=%d desired=(%d,%d)",xmin,xmax
						,pp->ymax-pp->ymin,CurrCell->xmax,(pp->ymin + pp->ymax)/2);
			}
			for (ap = pp->pinAtt ;ap != NULL; ap=ap->next)
				printf(" %s=%s",ap->name,ap->value);

			printf(" pintype=");
			if (ip->flags & IO_PAD)
				printf("pad");

			if (ip->flags & IO_INPUT)
				if (ip->flags & IO_OUTPUT)
					printf("bidirectional");
				else
					printf("input");
			else if (ip->flags & IO_OUTPUT)
				printf("output");
			else if (ip->flags & IO_FEED)
				printf("feedthrough");
			else if (ip->flags & IO_VDD)
				printf("vdd");
			else if (ip->flags & IO_VSS)
				printf("gnd");
			printf("\n");
		}
	}
	printf(";\n");
}

void printTermlist()
{
	register ioPtr ip = CurrCell->pins;

	printf("termlist\n");
	for (;ip != NULL; ip = ip->next) {
		register pinPtr pp = ip->locations;
		register attribPtr ap;
		printf("   %s { ",ip->name);
		for (; pp != NULL; pp = pp->next) {
			if (pp->xmin != pp->xmax)
				printf("(%d-%d,",pp->xmin,pp->xmax);
			else
				printf("(%d,",pp->xmin);
			if (pp->ymin != pp->ymax)
				printf("%d-%d) ",pp->ymin,pp->ymax);
			else
				printf("%d) ",pp->ymin);
			if (pp->layer != NULL)
				printf("layer=%s ",pp->layer);
			for (ap = pp->pinAtt ;ap != NULL; ap=ap->next)
				printf(" %s=%s",ap->name,ap->value);
		}
		printf("}");
		for (ap = ip->ioAtt ;ap != NULL; ap=ap->next)
			printf(" %s=%s",ap->name,ap->value);

		printf(" pintype=");
		if (ip->flags & IO_PAD)
			printf("pad");

		if (ip->flags & IO_INPUT)
			if (ip->flags & IO_OUTPUT)
				printf((ip->flags & IO_PAD) ? "bidirectional" : "bus" );
			else
				printf("input");
		else if (ip->flags & IO_OUTPUT)
			printf("output");
		else if (ip->flags & IO_FEED)
			printf("feedthrough");
		else if (ip->flags & IO_VDD)
			printf("vdd");
		else if (ip->flags & IO_VSS)
			printf("gnd");
		printf(";\n");
	}
}

void printInterface()
{
	if (strcmp(ModType,"domain"))
		printTermlist();
	else
		printIOList();
}

void addCellAttrib(attName,attVal)
char *attName, *attVal;
{
	register attribPtr ap = (attribPtr) malloc(sizeof(*ap));
	ap->name = attName;
	ap->value = attVal;
	ap->next = CurrCell->cellAtt;
	CurrCell->cellAtt = ap;
}

pinPtr addIOAttrib(pp,attName,attVal)
pinPtr pp;
char *attName, *attVal;
{
	register attribPtr ap = (attribPtr) malloc(sizeof(*ap));
	ap->name = attName;
	ap->value = attVal;
	if (pp->io == NULL) {
		fprintf(stderr,"PROGRAMMING ERROR - pp->io == NULL\n");
		exit(1);
	}
	ap->next = pp->io->ioAtt;
	pp->io->ioAtt = ap;
	return (pp);
}

/* Used to read in pins for which only the desired side is known.  These
 * pins may occur only in PARENT cells.  The x-locations of the pins
 * on the top/bottom sides and y-locations of pins on the left/right
 * are set here to be outside of the bounding box to signify the undefined
 * positions.
 */
pinPtr addDesiredLocation(ip,side)
ioPtr ip;
char *side;
{
	pinPtr pp = (pinPtr) malloc(sizeof(*pp));

	if (strcmp(ModType,"domain"))
		fprintf(stderr,"WARNING: Missing pin coordinates, line %d\n"
			,yylineno);
	pp->io = ip;
	pp->next = ip->locations;
	ip->locations = pp;
	pp->flags = 0;
	pp->pinAtt = NULL;
	pp->layer = NULL;
	if (!strcmp(side,"BOTTOM")) {
		pp->flags = PIN_BOT|PIN_NO_POS;
		pp->ymin = pp->ymax = CurrCell->ymin;
		pp->xmin = pp->xmax = CurrCell->xmin - 1;
	} else if (!strcmp(side,"TOP")) {
		pp->flags = PIN_TOP|PIN_NO_POS;
		pp->ymin = pp->ymax = CurrCell->ymax;
		pp->xmin = pp->xmax = CurrCell->xmin - 1;
	} else if (!strcmp(side,"LEFT")) {
		pp->flags = PIN_LEFT|PIN_NO_POS;
		pp->xmin = pp->xmax = CurrCell->xmin;
		pp->ymin = pp->ymax = CurrCell->ymin - 1;
	} else if (!strcmp(side,"RIGHT")) {
		pp->flags = PIN_RIGHT|PIN_NO_POS;
		pp->xmin = pp->xmax = CurrCell->xmax;
		pp->ymin = pp->ymax = CurrCell->ymin - 1;
	} else {
		fprintf(stderr,"WARNING: Illegal pin location %s, line %d\n"
			,side,yylineno);
	}
	return pp;
}

/* Used to read in pins for which the desired side, position, and layer
 * are known.  The pin box is set to the appropriate segment.
 */
pinPtr addLocation1(ip,side,position,width,layer)
ioPtr ip;
char *side;
int position;
int width;
char *layer;
{
	pinPtr pp = (pinPtr) malloc(sizeof(*pp));

	pp->io = ip;
	pp->next = ip->locations;
	ip->locations = pp;
	pp->flags = 0;
	pp->pinAtt = NULL;
	pp->layer = layer;
	width /= 2;
	if (!strcmp(side,"BOTTOM")) {
		pp->flags = PIN_BOT;
		pp->ymin = pp->ymax = CurrCell->ymin;
		pp->xmin = position - width;
		pp->xmax = position + width;
	} else if (!strcmp(side,"TOP")) {
		pp->flags = PIN_TOP;
		pp->ymin = pp->ymax = CurrCell->ymax;
		pp->xmin = position - width;
		pp->xmax = position + width;
	} else if (!strcmp(side,"LEFT")) {
		pp->flags = PIN_LEFT;
		pp->xmin = pp->xmax = CurrCell->xmin;
		pp->ymin = position - width;
		pp->ymax = position + width;
	} else if (!strcmp(side,"RIGHT")) {
		pp->flags = PIN_RIGHT;
		pp->xmin = pp->xmax = CurrCell->xmax;
		pp->ymin = position - width;
		pp->ymax = position + width;
	} else {
		fprintf(stderr,"WARNING: Illegal pin location %s, line %d\n"
			,side,yylineno);
	}
	return pp;
}

pinPtr addLocation2(ip,x,y,width,layer)
ioPtr ip;
int x,y;
int width;
char *layer;
{
	pinPtr pp = (pinPtr) malloc(sizeof(*pp));
	pointPtr ptp = CurrCell->outline;
	pointPtr ntp = ptp->clock;
	int xmin,xmax,ymin,ymax;

	pp->io = ip;
	pp->next = ip->locations;
	ip->locations = pp;
	pp->flags = 0;
	pp->pinAtt = NULL;
	pp->layer = layer;
	pp->ymin = pp->ymax = pp->xmin = pp->xmax = 0;
	width /= 2;

	/* For each segment of the cell outline, check if the pin is located
	 * on this segment.  If it is, set the pin box so that it agrees with
	 * the direction of the segment and return. 'ptp' and 'ntp' are two
	 * consecutive points on the outline.
	 */
	do {
		xmin = ptp->x;
		xmax = ntp->x;
		ymin = ptp->y;
		ymax = ntp->y;
		if (xmin > xmax) {
			xmin = ntp->x;
			xmax = ptp->x;
		}
		if (ymin > ymax) {
			ymin = ntp->y;
			ymax = ptp->y;
		}
		if ((xmin <= x) && (xmax >= x) && (ymin <= y) && (ymax >= y)) {
			if (xmin != xmax) {
				pp->ymin = pp->ymax = y;
				pp->xmin = x - width;
				pp->xmax = x + width;
			} else {
				pp->xmin = pp->xmax = x;
				pp->ymin = y - width;
				pp->ymax = y + width;
			}
			return pp;
		}
		ptp = ntp;
		ntp = ntp->clock;
	} while (ptp != CurrCell->outline);

	/* Did not find the segment - the pin is somewhere in the interior.
	 * Make it a square pin.
	 */
	pp->xmin = x - width;
	pp->xmax = x + width;
	pp->ymin = y - width;
	pp->ymax = y + width;
	fprintf(stderr
		,"WARNING: pin (%d,%d) on line %d lies in the cell interior.\n"
		,x,y,yylineno);
	fprintf(stderr,"          Its shape was adjusted to be a square.\n");
	return pp;
	/* OLD CODE that was used to adjust the pin position to the
	 * top or bottom boundary.

	ymin = y - CurrCell->ymin;
	ymax = y - CurrCell->ymax;
	if (ymin < 0) ymin = -ymin;
	if (ymax < 0) ymax = -ymax;
	if (ymin < ymax) {
		pp->ymin = pp->ymax = CurrCell->ymin;
		pp->xmin = x - width;
		pp->xmax = x + width;
	} else {
		pp->ymin = pp->ymax = CurrCell->ymax;
		pp->xmin = x - width;
		pp->xmax = x + width;
	}
	fprintf(stderr,"WARNING: pin (%d,%d) adjusted to (%d,%d), line %d\n"
		,x,y,x,pp->ymin,yylineno);
	fprintf(stderr,"WARNING: pin should fall on the cell boundary\n");
	return pp;
	 */
}

ioPtr newIO(pinName,pType)
char *pinName, *pType;
{
    register ioPtr ip = CurrCell->pins;
    register char *c;
    
    for (c=pinName; *c != 0; c++)
	if (*c == '(')
		*c = '<';
	else if (*c == ')')
		*c = '>';

    /* Look for an I/O port witht the same name */

    while ((ip != NULL) && strcmp(ip->name,pinName))
        ip = ip->next;

    /* If not found - create a new IO port */

    if (ip == NULL) {
        ip = (ioPtr) malloc(sizeof(*ip));
        ip->name = pinName;
        ip->locations = NULL;
	ip->next = NULL;
	ip->ioAtt = NULL;
	if (strcmp(pType,"F") &&
		(!noPWR || (strcmp(pType,"PWR") && strcmp(pType,"GND"))))
		CurrCell->portCount += 1;
        if (CurrCell->pins == NULL)
        	CurrCell->pins = ip;
	else {	/* Add at the end of the list of the cell IO ports */
		register ioPtr here = CurrCell->pins;
		while (here->next != NULL)
			here = here->next;
		here->next = ip;
	}
        if (!strcmp(pType,"I"))	      ip->flags = IO_INPUT;
        else if(!strcmp(pType,"O"))   ip->flags = IO_OUTPUT;
        else if(!strcmp(pType,"B"))   ip->flags = IO_INPUT | IO_OUTPUT;
        else if(!strcmp(pType,"PI"))  ip->flags = IO_INPUT | IO_PAD;
        else if(!strcmp(pType,"PO"))  ip->flags = IO_OUTPUT | IO_PAD;
        else if(!strcmp(pType,"PB"))  ip->flags = IO_INPUT | IO_OUTPUT | IO_PAD;
        else if(!strcmp(pType,"F"))   ip->flags = IO_FEED;
        else if(!strcmp(pType,"PWR")) ip->flags = IO_VDD;
        else if(!strcmp(pType,"GND")) ip->flags = IO_VSS;
        else {
            fprintf(stderr,"Unknown pin type %s\n",pType);
            exit(1);
        }
    }
    return (ip);
}

void addProfilePoint(x,y,cell)
int x,y;
cellPtr cell;
{
	pointPtr ptp = (pointPtr) malloc(sizeof(*ptp));
	ptp->x = x;
	ptp->y = y;
	if (cell->outline == NULL)
		cell->outline = ptp->clock = ptp->aclock = ptp;
	else {
		ptp->clock = cell->outline;
		ptp->aclock = cell->outline->aclock;
		cell->outline->aclock->clock = ptp;
		cell->outline->aclock = ptp;
		cell->outline = ptp;
	}
}

cellPtr newCell()
{
    register cellPtr cp = (cellPtr) malloc(sizeof(*cp));
    cp->name = CurrModule;
    cp->pins = NULL;
    cp->cellAtt = NULL;
    cp->outline = NULL;
    cp->portCount = 0;
    cp->next = allCells;
    allCells = cp;
    return (cp);
}

/* Much needed function to make a clone of a string */

char *strclone(str)
char *str;
{
  extern char *strcpy();
  char *s = malloc((unsigned) strlen(str) + 1);
  strcpy(s,str);
  return(s);
}

void main(argc,argv)
int argc;
char *argv[];
{
    argc--;
    argv++;
    if ( (argc>0) && !strcmp(argv[0],"-m")) {
				/* This for 'mixed' benchmarks only */
	noPWR = 1;		/* Do not count PWR/GND pins in the I/O list */
        argc--;
        argv++;
	fprintf(stderr,"No P/G counted\n");
    }
    if ( argc > 0)
        sscanf(argv[0],"%le",Scale);
    yyparse();
}
