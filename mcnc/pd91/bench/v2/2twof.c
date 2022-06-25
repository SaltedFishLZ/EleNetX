/*###########################################################################

                                                         ###
                                                         ###
     #########        #########         ######## #    ######       ######## #
   ###      ###      ###      ##       ##       ##       ###      ##       ##
  ###        ###     ###   ######      ######### #       ###      ######### #
  ###        ###     #######  ###         #######        ###         #######
  ###        ###    ###       ###      #        ##       ###      #        ##
   ###      ###     ###       ###      ##        #       ###      ##        #
     #########       ######### ####    # ########     #######     # ########

#############################################################################

     Copyright 1990 by the Microelectronics Center of North Carolina       
              Designed and written by Krzysztof Kozminski.

     This copy, of MCNC proprietary software, is made available exclusively
     for the use as a part of the 'v2' translator.

###########################################################################*/
#include <stdio.h>
#include "unixproto.h"
#include "oasisutils.h"
#include "parser.h"
#include "custom.h"
#include "parser_proto.h"

static FILE *of;
static int cellNum = 0;
static int padNum = 0;
static int twUnit;

/* Data structure for a chip I/O */

typedef struct TPP {
	term_ptr tp;
	pin_ptr  pip;
} *twofPadPtr;

int dim2tw(dim)
int dim;
{
	int res = dim / twUnit;
	if ((res *twUnit) != dim) {
		Message("ROUNDOFF ERROR - choose a smaller timberwolf unit ");
		Message("\n                 The above occurred when dividing %d by %d\n"
			,dim,twUnit);
		exit(1);
	}
	return res;
}

printTwofTerminal(tpp)
twofPadPtr tpp;
{
	register int i;
	register term_ptr tp = tpp->tp;
	register pin_ptr pip = tpp->pip;
	register int orient;
	register char padside;
	switch(pip->shape & TERM_LOC_MASK) {
	case TERM_ON_TOP:		orient = 3; padside = 'T'; break;
	case TERM_ON_LEFT:	orient = 7; padside = 'L'; break;
	case TERM_ON_RIGHT:	orient = 6; padside = 'R'; break;
	case TERM_ON_BOT:		orient = 0; padside = 'B'; break;
	default:
		Message("PROGRAMMING ERROR - undefined terminal %s of %s"
			,tp->whichNet->netName,tp->whichModule->moduleName);
		exit(1);
	}
	UseHashTable(1);
	fprintf(of,"pad %d %s orient %d\n",++padNum,MakeUniqueName("PAD_%s",NULL)
		,orient);
	fprintf(of,"padside %c\n",padside);
	fprintf(of,"left -1 right -1 bottom 1 top 1\n");
	fprintf(of,"pin name %s signal %s 0 1\n",tp->whichNet->netName
		,tp->whichNet->netName);
	fprintf(of,"\n");
}

int comparePads(tpp1,tpp2)
twofPadPtr tpp1,tpp2;
{
	register term_ptr tp1 = tpp1->tp;
	register term_ptr tp2 = tpp2->tp;
	register pin_ptr pip1 = tpp1->pip;
	register pin_ptr pip2 = tpp2->pip;
	register att_ptr ap1=pip1->pinAttribs;
	register att_ptr ap2=pip2->pinAttribs;
	int loc1;
	int loc2;

/* Compare the desired chip sides */
	loc1 = pip1->shape & TERM_LOC_MASK;
	loc2 = pip2->shape & TERM_LOC_MASK;
	if (loc1 != loc2)
		return (loc1-loc2);

/* Compare the pin intervals */
	loc1 = (pip1->xmin + pip1->xmax)/2;
	loc2 = (pip2->xmin + pip2->xmax)/2;
	if (loc1 != loc2)
		return (loc1-loc2);

/* Compare the desired locations. */
	for (;ap1!=NULL; ap1=ap1->next)
		if (strEQ(ap1->attName,"desired"))
			break;
	for (;ap2!=NULL; ap2=ap2->next)
		if (strEQ(ap2->attName,"desired"))
			break;

/* If desired locations missing, return the initial sequence */
	if ((ap1==NULL) || (ap2==NULL))
		if (tp1 != tp2)
			return (tp1-tp2);
		else	
			return (pip1-pip2);

/* either .h or .v will be equal - base the comparison on the unequal ones */
	loc1 = ap1->attValue.pointValue.h;
	loc2 = ap2->attValue.pointValue.h;
	if (loc1 != loc2)
		return (loc1-loc2);
	loc1 = ap1->attValue.pointValue.v;
	loc2 = ap2->attValue.pointValue.v;
	return (loc1-loc2);
}

printCellOrMacroPin(cp,i,xoff,yoff,subCell)
module_ptr cp;
int i,xoff,yoff;
inst_ptr subCell;
{
	register term_ptr tp = cp->terminals[i];
	register pin_ptr pip = tp->pins[0];
	register att_ptr ap = tp->termAttribs;
	char *sigName = "TW_PASS_THRU";
	register int k;

	for (;ap!=NULL; ap=ap->next)
		if (strEQ(ap->attName,"pintype"))
			break;

	if ((ap != NULL) && strEQ(ap->attValue.stringValue,"vdd"))
		sigName = tp->whichNet->netName;
	else if ((ap == NULL) || strNE(ap->attValue.stringValue,"feedthrough"))
		sigName = subCell->interface[i]->net->netName;
	fprintf(of,"pin name %s_0 signal %s %d %d\n",tp->whichNet->netName,sigName
		,dim2tw((pip->xmin+pip->xmax)/2)-xoff
		,dim2tw((pip->ymin+pip->ymax)/2)-yoff);
	for (k=1; k<tp->numPins; k++) {
		pip = tp->pins[k];
		fprintf(of,"   equiv name %s_%d %d %d\n",tp->whichNet->netName,k
			,dim2tw((pip->xmin+pip->xmax)/2)-xoff
			,dim2tw((pip->ymin+pip->ymax)/2)-yoff);
	}
}

printTwofMacroInstance(subCell)
inst_ptr subCell;
{
	int xmin,ymin,xmax,ymax;
	int xoff, yoff;
	register int i;

	module_ptr cp = subCell->reference;

	fprintf(of,"pad %d %s orient 0\n",++padNum,subCell->instName);
	fprintf(of,"padside MM\n");
	calcCellBox(cp,&xmin,&xmax,&ymin,&ymax);
	xmin = dim2tw(xmin); xmax = dim2tw(xmax);
	ymin = dim2tw(ymin); ymax = dim2tw(ymax);
	xoff = (xmin+xmax)/2;
	yoff = (ymin+ymax)/2;

	fprintf(of,"left %d right %d bottom %d top %d\n",xmin-xoff,xmax-xoff
		,ymin-yoff,ymax-yoff);
	for (i=0; i<cp->numTerminals; i++)
		printCellOrMacroPin(cp,i,xoff,yoff,subCell);
	fprintf(of,"\n");
}

printTwofCellInstance(subCell)
inst_ptr subCell;
{
	int xmin,ymin,xmax,ymax;
	int xoff, yoff;
	register int i;

	module_ptr cp = subCell->reference;

	fprintf(of,"cell %d %s\n",++cellNum,subCell->instName);
	calcCellBox(cp,&xmin,&xmax,&ymin,&ymax);
	xmin = dim2tw(xmin); xmax = dim2tw(xmax);
	ymin = dim2tw(ymin); ymax = dim2tw(ymax);
	xoff = (xmin+xmax)/2;
	yoff = (ymin+ymax)/2;

	fprintf(of,"left %d right %d bottom %d top %d\n",xmin-xoff,xmax-xoff
		,ymin-yoff,ymax-yoff);
	for (i=0; i<cp->numTerminals; i++)
		printCellOrMacroPin(cp,i,xoff,yoff,subCell);
	fprintf(of,"\n");
}

void outputTimberwolf(fName,unit)
char *fName;
int unit;
{
   register int i,j;
	register module_ptr parent = AllDomains[NumDomains-1];	/* Parent cell */
	twofPadPtr tpp = NULL;
	int npads = 0;
	twUnit = unit;

   of = fopen(fName,"w");
   if (of == NULL) {
      Message("FATAL ERROR - could not open output file %s\n",fName);
      exit(1);
   }

	/* Print standard cells */
	for (i=0; i<parent->numSubModules; i++) {
		register inst_ptr subCell = parent->subModules[i];
		register module_ptr thisCell = subCell->reference;
		register att_ptr ap = thisCell->moduleAttribs;
		for (;ap!=NULL; ap=ap->next)
			if (strEQ(ap->attName,"type"))
				if (strEQ(ap->attValue.stringValue,"STANDARD"))
					break;
				else
					goto next;
		/* if (ap == NULL)
			Message("WARNING: cell %s assumed to be a standard cell"
				,thisCell->moduleName); */
		printTwofCellInstance(subCell);
		next:;
	}

	/* Print macro blocks */
	for (i=0; i<parent->numSubModules; i++) {
		register inst_ptr subCell = parent->subModules[i];
		register module_ptr thisCell = subCell->reference;
		register att_ptr ap = thisCell->moduleAttribs;
		for (;ap!=NULL; ap=ap->next)
			if (strEQ(ap->attName,"type"))
				if (strNE(ap->attValue.stringValue,"STANDARD"))
					printTwofMacroInstance(subCell);
	}

	/* Count the number of actual pads */
 	for (i=0; i<parent->numTerminals; i++)
		npads += parent->terminals[i]->numPins;

	/* Build the table of pads for timberwolf */
	tpp = (twofPadPtr) malloc(npads*sizeof(*tpp));
	npads = 0;
	for (i=0; i<parent->numTerminals; i++)
		for (j=0; j<parent->terminals[i]->numPins; j++) {
			tpp[npads].tp = parent->terminals[i];
			tpp[npads++].pip = parent->terminals[i]->pins[j];
		}

	/* Sort pins accordingly to the desired sequences on each side so that
	 * timberwolf can place them uniformly
	 */
	qsort(tpp,npads,sizeof(*tpp),comparePads);

	/* Print pads */
 	for (i=0; i<npads; i++)
		printTwofTerminal(tpp + i);

   (void) fclose(of);
}
/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set ts=3 sw=3:
 */
