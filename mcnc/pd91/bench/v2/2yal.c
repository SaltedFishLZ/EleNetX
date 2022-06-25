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

static void myFloatFormat(fp,v)
FILE *fp;
double v;
{
   long i, f;
   i = (long) v;
   f = (v - i + 0.0005)*1000;
   fprintf(fp," %ld",i);
   if (f > 0)
      if (f % 10)
         fprintf(fp,".%03ld",f);
      else if (f % 100)
         fprintf(fp,".%02ld",f/10);
      else
         fprintf(fp,".%ld",f/100);
}

static double findModuleScale(cp)
module_ptr cp;
{
   register att_ptr ap = cp->moduleAttribs;
   for (;ap!=NULL; ap=ap->next)
      if (strEQ(ap->attName,"scale"))
         break;
   if (ap == NULL) {
      WARN(WARN_CSCALE);
      if (doWarnings) {
         if (commentWarnings)
            fprintf(of," /* WARNING: no scale specified ->assumed 0.001 */");
         else
            Message("WARNING: unknown scaleof cell '%s' -> assumed 0.001"
               ,cp->moduleName);
      }
      return(0.001);
   } else
      return(ap->attValue.floatValue);
}

static char *findModuleType(cp)
module_ptr cp;
{
   register att_ptr ap = cp->moduleAttribs;
   static char parent[] = "PARENT";
   static char standard[] = "STANDARD";
   static char standardW[] = "STANDARD /* WARNING: guessed cell type */";

   for (;ap!=NULL; ap=ap->next)
      if (strEQ(ap->attName,"type"))
         break;
   if (ap == NULL) {
      if (cp->what == zzDOMAIN)
         return parent;
      WARN(WARN_CTYPE);
      if (doWarnings) {
         if (commentWarnings)
            return(standardW);
         else
            Message("WARNING: unknown type of cell '%s' -> assumed STANDARD"
               ,cp->moduleName);
      }
      return(standard);
   } else
      return(ap->attValue.stringValue);
}

static char *findTerminalType(tp)
term_ptr tp;
{
   register att_ptr ap = tp->termAttribs;
   register int i;
   static char bidirW[] = "B /* WARNING: guessed pin type */ ";
   static struct {
      char *vpnr;
      char *yal;
   } corresp[13] = {
      { "bus", "B" }
     ,{ "pi", "I" }
     ,{ "input", "I" }
     ,{ "reset", "I" }
     ,{ "clock", "I" }
     ,{ "po", "O" }
     ,{ "output", "O" }
     ,{ "feedthrough", "F" }
     ,{ "padinput", "PI" }
     ,{ "padoutput", "PO" }
     ,{ "padbidirectional", "PB" }
     ,{ "pio", "PB" }
     ,{ "vdd", "PWR" }
   };

   for (;ap!=NULL; ap=ap->next)
      if (strEQ(ap->attName,"pintype"))
         break;

   if (ap == NULL) {
      WARN(WARN_PTYPE);
      if (doWarnings && commentWarnings)
         return(bidirW);
      return(corresp[0].yal);
   }
   
   for (i=0; i<13; i++)
      if (strEQ(ap->attValue.stringValue,corresp[i].vpnr))
         return(corresp[i].yal);

   WARN(WARN_PUTYPE);
   if (doWarnings)
      if (commentWarnings)
         fprintf(of," /* WARNING: strange pin type below */\n");
      else
         Message("WARNING: strange type of pin '%s' in cell '%s'",
            tp->whichNet->netName,tp->whichModule->moduleName);
   return (ap->attValue.stringValue);
}

static char *findPinLayer(pip,tp)
pin_ptr pip;
term_ptr tp;
{
   register att_ptr ap = pip->pinAttribs;
   static char none[] = "";
   static char metal2[] = "METAL2";
   static char metal2W[] = "METAL2 /* WARNING: guessed pin layer */ ";

   for (;ap!=NULL; ap=ap->next)
      if (strEQ(ap->attName,"layer"))
         return(ap->attValue.stringValue);

   if (!guessPins)
      return none;
   WARN(WARN_PLAYER);
   if (doWarnings)
      if (commentWarnings)
         return (metal2W);
      else
         Message("WARNING: layer of pin '%s' in cell '%s' defaulted to METAL2",
            tp->whichNet->netName,tp->whichModule->moduleName);
   return (metal2);
}

static double findPinWidth(pip,tp)
pin_ptr pip;
term_ptr tp;
{
   register att_ptr ap = pip->pinAttribs;

   for (;ap!=NULL; ap=ap->next)
      if (strEQ(ap->attName,"width"))
         switch (ap->attType) {
         case zzNUMBER:
            return(ap->attValue.floatValue);
         case zzINTEGER:
            return((double)(ap->attValue.intValue));
         case zzID:
         case zzQUOTESTR:
            return((double)atof(ap->attValue.stringValue));
         }

   switch (pip->shape & TERM_SHAPE_MASK) {
   case TERM_SQUARE:
      if ((pip->xmax - pip->xmin) != (pip->ymax - pip->ymin)) {
         Message("Cannot handle rectangular pins that are not squares.");
         Message("The offending pin is terminal '%s' in cell '%s'\n" 
               ,tp->whichNet->netName,tp->whichModule->moduleName);
         exit(1);
      }
   case TERM_HORIZ:
      return((double)(pip->xmax - pip->xmin));
   case TERM_VERT:
      return((double)(pip->ymax - pip->ymin));
   case TERM_POINT:
      return((double)0);
   default:
      Message("Cannot handle pin shape '%d' yet (cell %s, terminal %s)\n"  
            ,pip->shape,tp->whichModule->moduleName
            ,tp->whichNet->netName);
      exit(1);
   }
   return (0); /* to shut up lint */
   /* WARN(WARN_PLAYER);
   if (doWarnings)
      if (commentWarnings)
         return (metal2W);
      else
         Message("WARNING: layer of pin '%s' in cell '%s' defaulted to METAL2",
            tp->whichNet->netName,tp->whichModule->moduleName);
   return (metal2);  */
}

static void printCellTerminal(cp,k,scale)
module_ptr cp;
int k;
double scale;
{
   register term_ptr tp = cp->terminals[k];
   register int l;
   double width;

   for (l=0; l<tp->numPins; l++) {
      register pin_ptr pip = tp->pins[revPins ? (tp->numPins - l - 1) : l];
      register att_ptr ap=pip->pinAttribs;
      double x = (pip->xmin + pip->xmax)/2;
      double y = (pip->ymin + pip->ymax)/2;
      for (ap=pip->pinAttribs; ap != NULL; ap=ap->next)
         if (strEQ(ap->attName,"desired")) {
            x = ap->attValue.pointValue.h;
            y = ap->attValue.pointValue.v;
            break;
         }
      fprintf(of,"    %s %s",tp->whichNet->netName,findTerminalType(tp));
      /* If a terminal that is allowed a range of positions has no desired
       * position, relax the allowed range to spane the entire chip side,
       * as YAL does not allow for a more dtailed specification.
       */
      if ((ap == NULL) && (pip->shape & TERM_INTERVAL)) {
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
         if (pip->shape & (TERM_ON_TOP|TERM_ON_BOT)) {
            if ((pip->xmin != 0) || (pip->xmax != 100)) {
               WARN(WARN_RELAX);
               if (doWarnings) {
                  if (commentWarnings)
                     fprintf(of," /* WARNING: pin position relaxed */");
                  else
                     Message("WARNING: relaxed pin interval <%d,%d> (cell %s, terminal %s)"
                     ,pip->xmin,pip->xmax,tp->whichModule->moduleName
                     ,tp->whichNet->netName);
               }
            }
         } else {
            if ((pip->ymin != 0) || (pip->ymax != 100)) {
               WARN(WARN_RELAX);
               if (doWarnings) {
                  if (commentWarnings)
                     fprintf(of," /* WARNING: pin position relaxed */");
                  else
                     Message("WARNING: relaxed pin interval <%d,%d> (cell %s, terminal %s)"
                     ,pip->ymin,pip->ymax,tp->whichModule->moduleName
                     ,tp->whichNet->netName);
               }
            }
         }
      } else {
         Boolean isParent = false;
         ap = cp->moduleAttribs;
         for (;ap!=NULL; ap=ap->next)
            if (strEQ(ap->attName,"type") &&
                strEQ(ap->attValue.stringValue,"PARENT")) {
               isParent = true;
               break;
            }

         if ( (do87 && !do87parent) || (isParent && do87parent)) {
            char *side;
            if ((pip->shape & TERM_LOC_MASK) == TERM_ON_TOP)
               side = "TOP";
            else if ((pip->shape & TERM_LOC_MASK) == TERM_ON_BOT)
               side = "BOTTOM";
            else if ((pip->shape & TERM_LOC_MASK) == TERM_ON_LEFT)
               { side = "LEFT"; x=y; }
            else if ((pip->shape & TERM_LOC_MASK) == TERM_ON_RIGHT)
               { side = "RIGHT"; x=y; }
            else {
               int xmin,xmax,ymin,ymax;
               calcCellBox(cp,&xmin,&xmax,&ymin,&ymax);
               if (y == ymin)          side = "BOTTOM";
               else if (y == ymax)     side = "TOP";
               else if (x == xmin)     { side = "LEFT"; x=y; }
               else if (x == xmax)     { side = "RIGHT"; x=y; }
               else {
                  Message("pin (%d-%d,%d-%d) seems not to be on the boundary of cell %s"
                        ,pip->xmin,pip->xmax,pip->ymin,pip->ymax
                        ,cp->moduleName);
                  Message("Argument '-7' must not be used for this circuit\n");
                  exit(1);
               }
            }
            fprintf(of," %s",side);
            myFloatFormat(of,scale*x);
            if (do87widths && !do87parent)
               myFloatFormat(of,scale*findPinWidth(pip,tp));
         } else {
            myFloatFormat(of,scale*x);
            myFloatFormat(of,scale*y);
            myFloatFormat(of,scale*findPinWidth(pip,tp));
         }
         if (!do87parent)
            fprintf(of," %s",findPinLayer(pip,tp));
         for (ap=pip->pinAttribs; ap != NULL; ap=ap->next)
            if (strEQ(ap->attName,"CURRENT") || strEQ(ap->attName,"VOLTAGE"))
               switch (ap->attType) {
               case zzNUMBER:
                  fprintf(of," %s %.3f",ap->attName,ap->attValue.floatValue);
                  break;
               case zzINTEGER:
                  fprintf(of," %s %d",ap->attName,ap->attValue.intValue);
                  break;
               case zzID:
               case zzQUOTESTR:
                  fprintf(of," %s %s",ap->attName,ap->attValue.stringValue);
                  break;
               }
      }
      fprintf(of,";\n");
   } 
}

static void printSubModule(cp,k)
module_ptr cp;
int k;
{
   int lineLen = 0;
   inst_ptr ip = cp->subModules[k];
   fprintf(of,"  %s %s",ip->instName,ip->reference->moduleName);
   lineLen = 3 + strlen(ip->instName) + strlen(ip->reference->moduleName);
   for (k=0; k <ip->reference->numTerminals; k++) {
      register io_ptr iop= ip->interface[k];
      if (iop != NULL) {
         int add = strlen(iop->net->netName) + 1;
         if ((lineLen + add) > 71) {
            lineLen = 3;
            fprintf(of,"\n   ");
         }
         fprintf(of," %s",iop->net->netName);
         lineLen += add;
      }
   }
   fprintf(of,";\n");
}

static void printModuleDimensions(mp,scale)
module_ptr mp;
double scale;
{
   register prof_ptr *ppp, pp;
   register int i;
   if (mp->what == zzCELL)
      ppp = mp->entity.mCell->profiles;
   else if (mp->what == zzDOMAIN)
      ppp = mp->entity.mDom->profiles;
   else {
      Message("PROGRAMMING ERROR - printing profile of an unknown entity\n");
      exit (1);
   }
   if (do87 && !do87parent) {
      int xmin,xmax,ymin,ymax;
      calcCellBox(mp,&xmin,&xmax,&ymin,&ymax);
      if ((xmin == 0) && (ymin == 0)) {
         fprintf(of,"  WIDTH");
         myFloatFormat(of,scale*xmax);
         fprintf(of,";\n");
         fprintf(of,"  HEIGHT");
         myFloatFormat(of,scale*ymax);
         fprintf(of,";\n");
         return;
      }
   }
   fprintf(of,"  DIMENSIONS");
   if (pp = ppp[BOTTOM]) {
      i = pp->ptCount-1;
      myFloatFormat(of,scale*(pp->profile[i].h));
      myFloatFormat(of,scale*(pp->profile[i].v));
   }
   if (pp = ppp[RIGHT])
      for (i=0; i<pp->ptCount; i++) {
         myFloatFormat(of,scale*(pp->profile[i].h));
         myFloatFormat(of,scale*(pp->profile[i].v));
      }
   if (pp = ppp[TOP])
      for (i=pp->ptCount; i-- >0;) {
         myFloatFormat(of,scale*(pp->profile[i].h));
         myFloatFormat(of,scale*(pp->profile[i].v));
      }
   if (pp = ppp[LEFT])
      for (i=pp->ptCount; i-- >0;) {
         myFloatFormat(of,scale*(pp->profile[i].h));
         myFloatFormat(of,scale*(pp->profile[i].v));
      }
   if (pp = ppp[BOTTOM])
      for (i=0; i<pp->ptCount-1; i++) {
         myFloatFormat(of,scale*(pp->profile[i].h));
         myFloatFormat(of,scale*(pp->profile[i].v));
      }
   fprintf(of,";\n");
}

void outputYal(fName)
char *fName;
{
   register int i;
   of = fopen(fName,"w");
   if (of == NULL) {
      Message("FATAL ERROR - could not open output file %s\n",fName);
      exit(1);
   }
   if (doLibrary)
      for (i=0; i<NumCells; i++) {
         register module_ptr cp = AllCells[i];
         register int k;
         double scale;

         fprintf(of,"MODULE %s;",cp->moduleName);
         scale = findModuleScale(cp);
         fprintf(of,"\n");

         fprintf(of,"  TYPE %s;\n",findModuleType(cp));
         printModuleDimensions(cp,scale);

         fprintf(of,"  IOLIST;\n");
         for (k=0; k<cp->numTerminals; k++)
            printCellTerminal(cp,k,scale);
         fprintf(of,"  ENDIOLIST;\nENDMODULE;\n");
      }
   if (doCircuit)
      for (i=0; i<NumDomains; i++) {
         register module_ptr dp = AllDomains[i];
         register int k;
         double scale;

         fprintf(of,"MODULE %s;",dp->moduleName);
         scale = findModuleScale(dp);
         fprintf(of,"\n");

         fprintf(of,"  TYPE %s;\n",findModuleType(dp));
         printModuleDimensions(dp,scale);

         fprintf(of,"  IOLIST;\n");
         for (k=0; k<dp->numTerminals; k++)
            printCellTerminal(dp,k,scale);
         fprintf(of,"  ENDIOLIST;\nNETWORK;\n");
         for (k=0; k<dp->numSubModules; k++)
            printSubModule(dp,k);
         fprintf(of,"  ENDNETWORK;\nENDMODULE;\n");
      }
   (void) fclose(of);
}
/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set ts=3 sw=3:
 */
