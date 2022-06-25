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
/* This file outputs what I think is EDIF version 2 0 0.  It is extensively
 * commented, so if I made any errors and your program cannot read EDIF
 * produced here, it should be easy to fix.  Before fixing anything, please
 * get acquainted with the description of the data structires in 'custom.h'.
 * Also, please let me know of any changes/fixes so that I can incorporate
 * them in the future releases.
 * 
 * Kris Kozminski (kk@mcnc.org) (919)-248-1969
 *
 * NOTES ON TERMINOLOGY:
 *
 *    VPNR           EDIF              DATA STRUCTURES in "custom.h"
 *   --------------------------------------------------------------
 *    domain         cell              Module
 *    cell           cell              Module
 *    terminal       port              Term
 *    terminal pin   connectLocation   Pin
 *                   portRef           IOPort
 */

#include <stdio.h>
#include "unixproto.h"
#include "oasisutils.h"
#include "parser.h"
#include "custom.h"
#include "parser_proto.h"

#define CAP_SCALE 1000  /* Capacitance scaling factor from VPNR (picoFarads)  */
                        /* to EDIF (femtoFarads).                             */

#define TIME_SCALE 1000000 /* Time scaling factor from VPNR (nanoSeconds)     */
                           /* to EDIF (femtoSeconds).                         */

#define FAN_SCALE  1000    /* Time scaling factor for the load-dependent part */
                           /* of the EDIF delay.  In VPNR it is expressed in  */
                           /* nanoseconds per picofarad, while in EDIF it is  */
                           /* in femtoseconds per femtofarad.                 */

#define CURRENT_SCALE 1000000	/* Current is expressed in microamps */
#define VOLTAGE_SCALE 1000		/* Voltage is expressed in milivolts */

static FILE *of;     /* Pointer to the EDIF output file. */
int iocount = 0;     /* Counter of all pins in all pin instances - used to
                      * verify the correct number of net 'portRefs' in EDIF.
                      */

/******************************************************************************/
/* This routine translates a VPNR 'pintype' attribute into one of the pin types
 * allowed in EDIF.
 */
 
static char *findEdifTerminalType(tp)
term_ptr tp;   /* Pointer to the terminal to process */
{
   register att_ptr ap = tp->termAttribs;
   register int i;
   static char bidirW[] = "INOUT)\n            (comment \"WARNING: guessed pin direction\")";

   /* The pin type translation table */
#define NUM_PIN_TYPES 12
   static struct { char *vpnr; char *edif; } corresp[NUM_PIN_TYPES] = {
      { "bus", "INOUT" }         ,{ "pi", "INPUT" }
     ,{ "input", "INPUT" }       ,{ "reset", "INPUT" }
     ,{ "clock", "INPUT" }       ,{ "po", "OUTPUT" }
     ,{ "output", "OUTPUT" }     ,{ "padinput", "INPUT" }
     ,{ "padoutput", "OUTPUT" }  ,{ "padbidirectional", "INOUT" }
     ,{ "pio", "INOUT" }         ,{ "vdd", "INOUT" }
   };

   /* Find 'pintype' attribute and translate its value */
   for (;ap!=NULL; ap=ap->next)
      if (strEQ(ap->attName,"pintype"))
         for (i=0; i<NUM_PIN_TYPES; i++)
            if (strEQ(ap->attValue.stringValue,corresp[i].vpnr))
               return(corresp[i].edif);

   /* If there was no 'pintype' attribute, guess 'INOUT', write a warning
    * (if so desired) and increment the warning counter.
    */
   if (doWarnings)
      if (commentWarnings) return(bidirW);      /* INOUT + comment */
      else
         Message("WARNING: guessed direction of pin %s in cell %s"
            ,tp->whichNet->netName,tp->whichModule->moduleName);
   WARN(WARN_PTYPE);
   return(corresp[0].edif);   /* INOUT */
}

/******************************************************************************/
/* This routine is used for printing VPNR terminal attributes.
 */

void attrib2edif(cp,tp,ap,fmt,scale)
module_ptr cp;
term_ptr tp;
att_ptr ap;
char *fmt;
int scale;
{
	switch (ap->attType) {
	case zzINTEGER: fprintf(of,fmt,ap->attValue.intValue*scale); break;
	case zzNUMBER: fprintf(of,fmt,(int) (ap->attValue.floatValue*scale)); break;
	default:
		Message("PROGRAMMING ERROR - non-numeric '%s' term %s, cell %s"
			,ap->attName,cp->moduleName,tp->whichNet->netName);
		exit(1);
	}
}

/******************************************************************************/
/* This routine returns a numeric value of a scaled attribute.
 */

int attrib2int(cp,tp,ap,scale)
module_ptr cp;
term_ptr tp;
att_ptr ap;
int scale;
{
	int res;
	switch (ap->attType) {
	case zzINTEGER: res = ap->attValue.intValue*scale; break;
	case zzNUMBER:  res = (int) (ap->attValue.floatValue*scale); break;
	default:
		Message("PROGRAMMING ERROR - non-numeric '%s' term %s, cell %s"
			,ap->attName,cp->moduleName,tp->whichNet->netName);
		exit(1);
	}
	return (res);
}

/******************************************************************************/
/* This routine prints an EDIF declaration of a cell terminal.  It also checks
 * that all attributes of a terminal were translated into EDIF.  This routine is
 * used within the 'interface' construct in EDIF.  The allowed voltage drop
 * and required current capacity are printed as 'userData' since I could not
 * fint the appropriate EDIF construct.
 *    (userData VOLTAGE <value>)
 *    (userData CURRENT <value>)
 * The values are milivolts and microamps.
 */

static void printEdifCellTerminal(cp,k)
module_ptr cp;    /* pointer to the cell.             */
int k;            /* Number of the terminal to print. */
{
   register term_ptr tp = cp->terminals[k];  /* Terminal to print.   */
   register att_ptr ap = tp->termAttribs;    /* Terminal attributes. */
   Boolean doDirection = true;   /* Will be set false for feedthrus. */
   char *acLoadFormat = "            (acLoad %d)\n";
   char *voltageFormat = "            (userData VOLTAGE %d)\n";
   char *currentFormat = "            (userData CURRENT %d)\n";

/* Characteristics of the port delay - initialized to something unreasonable
 */
#define DELAY_IMPOSSIBLE -1000000
   int tlh = DELAY_IMPOSSIBLE; int tlhf = DELAY_IMPOSSIBLE;
   int thl = DELAY_IMPOSSIBLE; int thlf = DELAY_IMPOSSIBLE;

/* In VPNR, port name is also the name of the net as seen locally in the
 * cell or domain.
 */
   fprintf(of,"          (port %s\n",tp->whichNet->netName);

/* Process port attributes.
 */
   for (;ap!=NULL; ap=ap->next)
      if (strEQ(ap->attName,"pintype")) {
         if (strEQ(ap->attValue.stringValue,"feedthrough")) {
            fprintf(of,"            (unused)\n");
            doDirection = false;
         } /* else ignore - will be handled below - in findEdifTerminalType */
      } else if (strEQ(ap->attName,"loads")) {
			attrib2edif(cp,tp,ap,acLoadFormat,CAP_SCALE);
		} else if (strEQ(ap->attName,"CURRENT")) {
			attrib2edif(cp,tp,ap,voltageFormat,CURRENT_SCALE);
		} else if (strEQ(ap->attName,"VOLTAGE")) {
			attrib2edif(cp,tp,ap,currentFormat,VOLTAGE_SCALE);
      } else if (strEQ(ap->attName,"nominal_rise_delay")) {
			tlh = attrib2int(cp,tp,ap,TIME_SCALE);
      } else if (strEQ(ap->attName,"nominal_rise_fan")) {
			tlhf = attrib2int(cp,tp,ap,FAN_SCALE);
      } else if (strEQ(ap->attName,"nominal_fall_delay")) {
			thl = attrib2int(cp,tp,ap,TIME_SCALE);
      } else if (strEQ(ap->attName,"nominal_fall_fan")) {
			thlf = attrib2int(cp,tp,ap,FAN_SCALE);
      } else
         Message("PROGRAMMING ERROR: attribute %s of terminal %s in cell %s ignored"
            ,ap->attName,tp->whichNet->netName,cp->moduleName);

/* Print the port delay specifications.
 */
   if ((thl != DELAY_IMPOSSIBLE) && (thlf != DELAY_IMPOSSIBLE)) {
      fprintf(of,"            (portDelay (derivation calculated) ");
      fprintf(of,"(loadDelay %d %d) (transition H L))\n",thl,thlf);
   } else if ((thl != DELAY_IMPOSSIBLE) || (thlf != DELAY_IMPOSSIBLE))
      Message("WARNING: ignored incomplete delay specs for pin %s in cell %s"
         ,tp->whichNet->netName,cp->moduleName);

   if ((tlh != DELAY_IMPOSSIBLE) && (tlhf != DELAY_IMPOSSIBLE)) {
      fprintf(of,"            (portDelay (derivation calculated) ");
      fprintf(of,"(loadDelay %d %d) (transition L H))\n",tlh,tlhf);
   } else if ((thl != DELAY_IMPOSSIBLE) || (thlf != DELAY_IMPOSSIBLE))
      Message("WARNING: ignored incomplete delay specs for pin %s in cell %s"
         ,tp->whichNet->netName,cp->moduleName);

/* If not a feed-thru, print the port direction.
 */
   if (doDirection)
      fprintf(of,"            (direction %s)\n"
         ,findEdifTerminalType(tp));
   fprintf(of,"          )\n");  /* end-of-port */
}

/******************************************************************************/
/* This routine finds out the layer on which the given pin resides.  It also
 * verifies that all pin attributes are processed into EDIF.  If layer is not
 * declared in VPNR, it is guessed on METAL2.
 */

static char *findEdifPinLayer(pip,tp)
pin_ptr pip;
term_ptr tp;
{
   register att_ptr ap = pip->pinAttribs;    /* Pin attributes */
   static char metal2[] = "METAL2";          /* Default layer  */
   static char metal2W[] = "METAL2 (comment \"WARNING: guessed pin layer\")";
   char *layer = NULL;                       /* Will poin to the pin layer */

/* Scan the attributes and find the layer.
 */
   for (;ap!=NULL; ap=ap->next)
      if (strEQ(ap->attName,"layer"))
         layer = ap->attValue.stringValue;
      else if (strEQ(ap->attName,"width"))
         ; /* Ignore - this was put in VPNR only for back-compatibility to yal*/
      else if (strEQ(ap->attName,"desired"))
         ; /* Ignore - this is processed when printing domain pin locations   */
      else {
         Message("PROGAMMING ERROR: Attribute %s of pin (%d-%d,%d-%d) of the "
            ,ap->attName,pip->xmin,pip->xmax,pip->ymin,pip->ymax);
         Message("                  the terminal %s ignored in cell %s"
            ,tp->whichNet->netName,tp->whichModule->moduleName);
      }

   if (layer != NULL) return layer;          /* Layer found - no problem */

   /* If there was no 'layer' attribute, guess 'METAL2', write a warning
    * (if so desired) and increment the warning counter.
    */
   if (doWarnings)
      if (commentWarnings)
         return (metal2W);    /* METAL2 + comment */
      else
         Message("WARNING: layer of pin '%s' in cell '%s' defaulted to METAL2",
            tp->whichNet->netName,tp->whichModule->moduleName);

   WARN(WARN_PLAYER);
   return (metal2);           /* METAL2 */
}

/******************************************************************************/
/* This routine prints all pins of a cell - in EDIF they are just different
 * connectLocations.  This routine is used within the 'contents' construct in
 * EDIF.
 */

static void printEdifCellPins(cp,k,cxmin,cxmax,cymin,cymax)
module_ptr cp;    /* Cell with the pins.                                   */
int k;            /* Number of the port whose locations should be printed. */
int cxmin,cxmax,cymin,cymax;  /* Bouinding box of the cell                 */
{
   register term_ptr tp = cp->terminals[k];  /* Port of interest. */
   register int l;                           /* Just a counter.   */

   fprintf(of,"          (portImplementation %s\n",tp->whichNet->netName);
   fprintf(of,"            (connectLocation\n");

/* Print all locations.
 */
   for (l=0; l<tp->numPins; l++) {
      register pin_ptr pip = tp->pins[l];    /* Port location to print.    */
      int xmin = pip->xmin;
      int xmax = pip->xmax;
      int ymin = pip->ymin;
      int ymax = pip->ymax;

      fprintf(of,"              (figure %s",findEdifPinLayer(pip,tp));

/* Be careful to output the location as a true rectangle.  Pins on the cell
 * boundary that are segments in VPNR are adjusted to be squares that lie
 * in the cell interior.  Pins that are points are output as such.
 */
      if (xmin == xmax) {  /* Vertical VPNR pin */
         if (ymin == ymax)
            ;/* Do not do anything */
         else if (xmin == cxmin)   /* Add pin width towards the cell interior */
            xmax += (ymax - ymin);
         else if (xmin == cxmax)   /* Add pin width towards the cell interior */
            xmin -= (ymax - ymin);
         else {   /* Adjust the width, watch out to remain inside the cell.   */
            xmin -= (ymax - ymin)/2;
            if (xmin < cxmin) xmin = cxmin;
            xmax = xmin + (ymax - ymin);
         }
      } else if (ymin == cymin) /* Add pin width towards the cell interior */
         ymax += (xmax - xmin);
      else if (ymin == cymax)   /* Add pin width towards the cell interior */
         ymin -= (xmax - xmin);
      else {   /* Adjust the width, watch out to remain inside the cell.   */
         ymin -= (xmax - xmin)/2;
         if (ymin < cymin) ymin = cymin;
         ymax = ymin + (xmax - xmin);
      }
      if (xmin != xmax)
         fprintf(of," (rectangle (pt %d %d) (pt %d %d))",xmin,ymin,xmax,ymax);
      else
         fprintf(of," (dot (pt %d %d))",xmin,ymin);
         
      fprintf(of," )\n");  /* end-of-figure */
   }
   fprintf(of,"            )\n");/* end-of-connectLocation */
   fprintf(of,"          )\n");  /* end-of-portImplementation */
}

/******************************************************************************/
/* This routine prints all pins of a terminal of a domain (topmost cell). Since
 * the bonding pads for all vpnr domains are defined as intervals and EDIF seems
 * to be missing the appropriate construct, the data is output as a
 * self-explanatory userData.  The format is:
 *    (userData interval <side> <from> <to>)
 * where <side> is TOP, BOTTOM, LEFT, or RIGHT, and <from> and <to> determine
 * the admissible interval (expresed in percents of the chip width/height).
 * This routine is used within the 'contents' construct in EDIF.
 */

static void printEdifDomainPins(cp,k)
module_ptr cp;    /* Domain containing a terminal. */
int k;            /* Number of the terminal to output.   */
{
   register term_ptr tp = cp->terminals[k];  /* Terminal of interest */
   register int l;                           /* Just a counter.      */

   fprintf(of,"          (portImplementation %s\n",tp->whichNet->netName);

/* Print all locations of the port pins.
 */
   for (l=0; l<tp->numPins; l++) {
      register pin_ptr pip = tp->pins[l];
      register att_ptr ap=pip->pinAttribs;
      int x = (pip->xmin + pip->xmax)/2;  /* Use center of the pin */
      int y = (pip->ymin + pip->ymax)/2;

/* Find the desired location, if any.
 */
      for (ap=pip->pinAttribs; ap != NULL; ap=ap->next)
         if (strEQ(ap->attName,"desired"))
            break;

/* Print pin location.
 */
      if (pip->shape & TERM_INTERVAL) {
         fprintf(of,"            (userData interval");
         switch (pip->shape & TERM_LOC_MASK) {
         case TERM_ON_TOP:    fprintf(of," TOP"); break;
         case TERM_ON_BOT:    fprintf(of," BOTTOM"); break;
         case TERM_ON_LEFT:   fprintf(of," LEFT"); break;
         case TERM_ON_RIGHT:  fprintf(of," RIGHT"); break;
         default:
            Message("Cannot handle pin shape '%d' yet (cell %s, terminal %s)\n"
               ,pip->shape,tp->whichModule->moduleName
               ,tp->whichNet->netName);
            exit(1);
         }
         if (pip->shape & (TERM_ON_TOP|TERM_ON_BOT))
            fprintf(of," %d %d)\n",pip->xmin,pip->xmax);
         else
            fprintf(of," %d %d)\n",pip->ymin,pip->ymax);
         if (ap != NULL) {
            fprintf(of,"            (comment \"Please treat the point below only as a DESIRED location\")\n");
            fprintf(of,"            (connectLocation\n");
            fprintf(of,"              (figure %s (dot (pt %d %d)))\n"
               ,findEdifPinLayer(pip,tp),ap->attValue.pointValue.h
               ,ap->attValue.pointValue.v);
            fprintf(of,"            )\n");/* end-of-connectLocation */
         } else
            (void)findEdifPinLayer(pip,tp);  /* Just to verify pin attribs. */
      } else {
         Message("PROGRAMMING ERROR - cannot handle non-interval domain pins");
         exit (1);
      }
   } 
   fprintf(of,"          )\n");  /* end-of-portImplementation */
}

/******************************************************************************/
/* This routine prints an EDIF net.
 */

static void printEdifNet(cp,k)
module_ptr cp;                /* Module containing the net. */
int k;
{
   net_ptr np = cp->nets[k];  /* Net to print.              */
   int ioc = iocount;

   if (np->netFlags & NET_PRINTED)
      return;
   np->netFlags |= NET_PRINTED;

   fprintf(of,"          (net %s (joined\n",np->netName);

/* Print references to cell (actually, domain) terminals.  'iocount' is
 * deceremented here by the number of all pins belonging to the terminal.
 * Any such terminals correspond to multiple pads assigned to the same net
 * (usually Vdd/GND).
 */
   for (k=0; k<cp->numTerminals; k++) {
      register term_ptr tp = cp->terminals[k];
      if (tp->whichNet == np) {
         iocount -= tp->numPins;
         fprintf(of,"            (portRef %s)\n",np->netName);
      }
   }
/* Print references to subcell terminals.  Note that the reference to the
 * 'np->subTerminals[k]->portDef' is the definition of the subcell terminal,
 * so that 'np->subTerminals[k]->portDef->whichNet->netName' will produce the
 * name of the net local to the subcell - which happens to be also the name
 * of the subcell port.
 */
   for (k=0; k<np->numTerms; k++) {
      iocount -= 1;
      fprintf(of,"            (portRef %s (instanceRef %s))\n"
         ,np->subTerminals[k]->portDef->whichNet->netName
         ,np->subTerminals[k]->interfaceBak->instName);
   }

   fprintf(of,"          ))\n"); /* end-of-joined, end-of-netnet */
   if ((ioc - iocount) < 2)
      Message("WARNING: net %s is attached to a single pin",np->netName);
}

/******************************************************************************/
/* This routine prints edif technology section.  I did not translate the
 * entire VPNR technology section - only one VPNR library has it anyway.
 */
edifTechnology()
{
   fprintf(of,"    (technology\n");
   fprintf(of,"      (numberDefinition\n");
   fprintf(of,"        (comment \"All dimensions are in nanometers\")\n");
   fprintf(of,"        (scale 1 (E 1 -9) (unit DISTANCE))\n");
   fprintf(of,"        (comment \"All times are in femtoseconds\")\n");
   fprintf(of,"        (scale 1 (E 1 -15) (unit TIME))\n");
   fprintf(of,"        (comment \"All capacitances are in femtofarads\")\n");
   fprintf(of,"        (scale 1 (E 1 -15) (unit CAPACITANCE))\n");
   fprintf(of,"        (comment \"All currents are in microamps\")\n");
   fprintf(of,"        (scale 1 (E 1 -6) (unit CURRENT))\n");
   fprintf(of,"        (comment \"All voltages are in milivolts\")\n");
   fprintf(of,"        (scale 1 (E 1 -3) (unit VOLTAGE))\n");
   fprintf(of,"      )\n");   /* end-of-numberDefinition */
   fprintf(of,"    )\n");     /* end-of-technology */
}

/******************************************************************************/
/* This routine prints entire EDIF stuff.
 */

void outputEdif(fName)
char *fName;
{
   register int i;
   char *basename = strclone(fName);
   module_ptr mp = NULL;
   char *cellLibName;
   char *domainLibName;

   UseHashTable(1);  /* contains ALL names ever seen */
   domainLibName = MakeUniqueName(NULL,NULL);
   cellLibName = MakeUniqueName(NULL,NULL);

   of = fopen(fName,"w");
   if (of == NULL) {
      Message("FATAL ERROR - could not open output file %s\n",fName);
      exit(1);
   } else {
      char *c;
      if (c = rindex(basename,'.'))
         *c = 0;
   }

/* Print edif file header */
   fprintf(of,"(edif %s\n",basename);
   fprintf(of,"  (edifVersion 2 0 0)\n");
   fprintf(of,"  (edifLevel 0)\n");
   fprintf(of,"  (keywordMap (keywordLevel 0))\n");

/* Print edif cell library */
   if (doLibrary) {
      fprintf(of,"  (library %s\n",cellLibName);
      fprintf(of,"    (edifLevel 0)\n");

      edifTechnology();

      for (i=0; i<NumCells; i++) {
         register module_ptr cp = AllCells[i];
         register int k;
         int xmin,xmax,ymin,ymax;

         fprintf(of,"    (cell %s\n",cp->moduleName);
         fprintf(of,"      (cellType GENERIC)\n");
         fprintf(of,"      (view layout\n");
         fprintf(of,"        (viewType MASKLAYOUT)\n");

/* Print port definitions */
         fprintf(of,"        (interface\n");
         for (k=0; k<cp->numTerminals; k++)
            printEdifCellTerminal(cp,k);
         fprintf(of,"        )\n"); /* end-of-interface */

/* Print port locations */
         fprintf(of,"        (contents\n");
         calcCellBox(cp,&xmin,&xmax,&ymin,&ymax);
         fprintf(of,"          (boundingBox (rectangle (pt %d %d) (pt %d %d)))\n"
            ,xmin,ymin,xmax,ymax);
         for (k=0; k<cp->numTerminals; k++)
            printEdifCellPins(cp,k,xmin,xmax,ymin,ymax);

         fprintf(of,"        )\n"); /* end-of-contents */

         fprintf(of,"      )\n");   /* end-of-view */
         fprintf(of,"    )\n");     /* end-of-cell */
      }
      fprintf(of,"  )\n");          /* end-of-library */
   }

/* Print domains in a separate library */
   if (doCircuit) {
      register module_ptr cp;
      Message("INFO: iocount %d",iocount);

      fprintf(of,"  (library %s\n",domainLibName);
      fprintf(of,"    (edifLevel 0)\n");

      edifTechnology();

      for (i=0; i<NumDomains; i++) {
         register int k;
         int xmin,xmax,ymin,ymax;

         cp = AllDomains[i];
         fprintf(of,"    (cell %s\n",cp->moduleName);
         fprintf(of,"      (cellType GENERIC)\n");
         fprintf(of,"      (view layout\n");
         fprintf(of,"        (viewType MASKLAYOUT)\n");

         /* Print port definitions */
         fprintf(of,"        (interface\n");
         for (k=0; k<cp->numTerminals; k++)
            printEdifCellTerminal(cp,k);
         fprintf(of,"        )\n"); /* end-of-interface */

         /* Print port locations */
         fprintf(of,"        (contents\n");
         calcCellBox(cp,&xmin,&xmax,&ymin,&ymax);
         fprintf(of,"          (boundingBox (rectangle (pt %d %d) (pt %d %d)))\n"
            ,xmin,ymin,xmax,ymax);
         fprintf(of,"          (comment \"The above represents the DESIRED dimensions\")\n");
         for (k=0; k<cp->numTerminals; k++)
            printEdifDomainPins(cp,k);

         for (k=0; k<cp->numSubModules; k++) {
            inst_ptr ip = cp->subModules[k];
            fprintf(of,"          (instance %s (viewRef layout",ip->instName);
            fprintf(of," (cellRef %s",ip->reference->moduleName);
            fprintf(of," (libraryRef %s))))\n"
               ,(ip->reference->what == zzDOMAIN) ? domainLibName :cellLibName);
         }
         for (k=0; k<cp->numNets; k++)
            printEdifNet(cp,k);
         fprintf(of,"        )\n"); /* end-of-contents */

         fprintf(of,"      )\n");   /* end-of-view */
         fprintf(of,"    )\n");  /* end-of-cell */
      }
      fprintf(of,"  )\n"); /* end-of-library of domains*/

      /* Print edif design - a reference to the main cell, which must be
       * the last domain.
       */
      fprintf(of,"  (design %s (cellRef %s (libraryRef %s)))\n"
         ,cp->moduleName,cp->moduleName,domainLibName);
   }

   fprintf(of,")\n");   /* end-of-edif-file */
   if (iocount != 0)
      Message("PROGRAMMING ERROR: iocount discrepancy %d",iocount);
   (void) fclose(of);
}
/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set ts=3 sw=3:
 */
