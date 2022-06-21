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

void calcCellBox(mp,xmin,xmax,ymin,ymax)
module_ptr mp;
int *xmin,*xmax,*ymin,*ymax;
{
   register prof_ptr *ppp, pp;
   register int i,k;
   Boolean foundFirst = false;
   if (mp->what == zzCELL)
      ppp = mp->entity.mCell->profiles;
   else if (mp->what == zzDOMAIN)
      ppp = mp->entity.mDom->profiles;
   else {
      Message("PROGRAMMING ERROR - calculating box of an unknown entity\n");
      exit (1);
   }
   for (k=0; k<4; k++)
      if (pp = ppp[k])
         for (i=0; i<pp->ptCount; i++) {
            int x = pp->profile[i].h;
            int y = pp->profile[i].v;
            if (foundFirst) {
               if (*xmin > x) *xmin = x; if (*xmax < x) *xmax = x;
               if (*ymin > y) *ymin = y; if (*ymax < y) *ymax = y;
            } else {
               *xmin = *xmax = x; *ymin = *ymax = y;
               foundFirst = true;
            }
         }
}
