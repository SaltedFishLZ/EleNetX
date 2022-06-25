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
#define MAIN_PROGRAM_MODULE
#include <stdio.h>
#include "unixproto.h"
#include "oasisutils.h"
#include "custom.h"
#include "parser.h"
#include "parser_proto.h"

#define MCNC
#ifndef MCNC
#define ReportUsage(a,b)
#endif MCNC

static char *usage =
"\n   v2 [-L] [-C] [-R] [-W] [-O] [-G] [-r] [-7w] [-w#] fname.ext < fname.vpnr\
\nwhere 'ext' is one of the following:\
\n   yal - for YAL file\n   cel - for TIMBERWOLF file\n   edf - for EDIF file\
\n   sim - for transistor netlist file\
\nPresence of the -L argument causes the cell description NOT to be output\
\nPresence of the -C argument causes the circuit description NOT to be output\
\nPresence of the -G argument means: do not guess pin layers\
\nPresence of the -r argument reverses the order of pins for each terminal - this\
\n   is useful for verifying translation yal->vpnr->yal, as yal2vpnr reverses\
\n   this sequence on its own.\
\nYAL output options:\
\n   Presence of the -7 argument causes the pin locations to be specified in the\
\n   1987 version of YAL, using the specification 'RIGHT', 'LEFT', etc.\
\n   Argument -7p limits the above to the PARENT cells only\
\n   Argument -7w skips the printout of pin widths\
\nTimberwolf output options:\
\n   -w# - the number '#' is the timberwolf distance unit (in nanometers)\
\nOther arguments will cause warnings to be produced whenever a confusing\
\n(possibly erroneous) data in the VPNR file is found:\
\n   -R argument warns about redundantly defined nets.\
\n   -W argument warns about other unexpected situations\
\n   -O argument puts all the above warnings as comments in the output file\
";

static char* DO_YAL  = "YAL";
static char* DO_TWOF = "TIMBERWOLF";
static char* DO_EDIF = "EDIF";
static char* DO_SIM  = "SIM";
static char* DO_VPNR = "VPNR";

/* Pre-initialize a transistor module.  Its name is an empty string, thus
 * always different from all other modules.  Only one transistor module is
 * necessary.
 */

void InitXtor()
{
   register module_ptr cp;

   cp = (module_ptr) MessPtr(sizeof(*cp));

   UseHashTable(ModuleNames);
   SetNameData("",&cp->moduleName,(voidptr)cp);
   cp->moduleAttribs = NULL;
   cp->numSubModules = 0;
   cp->numTerminals = 0;
   cp->numNets = 0;
   cp->what = zzXTOR;
   (void) AddCellPort(cp,InitCellPort("gate"));
   (void) AddCellPort(cp,InitCellPort("source"));
   (void) AddCellPort(cp,InitCellPort("drain"));
}

int main(argc,argv)
int argc;
char *argv[];
{
   char *whatToDo = NULL;
   int twolfUnit = 1000;
   extern char *rindex();
   extern int yyparse();
   extern void outputYal();

   InitError(argv[0]);
   argc-- ; 
   argv++ ;

   while ((argc > 0) && (*argv[0] == '-')) {
      switch (argv[0][1]) {
      case 'L': doLibrary = false; break;
      case 'C': doCircuit = false; break;
      case 'G': guessPins = false; break;
      case 'r': revPins = true; break;
      case 'W': doWarnings = true ; break;
      case 'R': doRWarnings = true; break;
      case 'O': commentWarnings = true; break;
      case '7':
         do87 = true;
         if (argv[0][2] == 'w')
            do87widths = false;
         else if (argv[0][2] == 'p')
            do87parent = true;
         break;
      case 'w': {
         register char *readHere;
         if (strlen(argv[0]) == 2) {
            if (argc < 1) {
               Message("FATAL ERROR - unexpectded end of arguments after '%s'\n"
                  ,argv[0]);
               exit(1);
            }
            argc-- ; argv++ ;
            readHere = argv[0];
         } else
            readHere = argv[0] + 2;
         if (sscanf(readHere,"%d",&twolfUnit) != 1) {
            Message("FATAL ERROR - cannot read pin width from argument '%s'\n"
               ,argv[0]);
            exit(1);
         }
      } break;
      default:
         argc = 1;   /* Will break the loop */
      }
      argc-- ; 
      argv++ ;
   }
   if (argc == 1) {
      char *ext = (char*) rindex(argv[0],'.');
      if (ext != NULL)
         ext += 1;
      if (strEQ(ext,"yal"))      whatToDo = DO_YAL;
      else if (strEQ(ext,"cel")) whatToDo = DO_TWOF;
      else if (strEQ(ext,"edf")) whatToDo = DO_EDIF;
      else if (strEQ(ext,"vpnr"))whatToDo = DO_VPNR;
   }
   if (whatToDo != NULL) {
      struct WARN_INFO *wi = warningCount;
      Boolean diag = true;
      Message("Output file '%s' will be written in %s format",argv[0],whatToDo);
      if (whatToDo == DO_YAL) {
         if (!doLibrary)
            Message("The cell library will not be included in the output");
         if (!doCircuit)
            Message("The top-level netlist will not be included in the output");
      }
      ReportUsage("program startup",RES_TIMES);
      TmpNames = UseHashTable(0);
      AttNames = UseHashTable(0);
      AttValues = UseHashTable(0);
      ModuleNames = UseHashTable(0);
      InitXtor();
      yyparse();
      ReportUsage("data parsed",RES_TIMES|RES_CORE);
      if  (whatToDo == DO_YAL)
         outputYal(argv[0]);
      else if (whatToDo == DO_TWOF) {
			if (NumDomains > 0)
         	outputTimberwolf(argv[0],twolfUnit);
			else {
				Message("No parent cell in the input data\n");
				exit(1);
			}
      } else if (whatToDo == DO_EDIF)
         outputEdif(argv[0]);
      else if (whatToDo == DO_SIM)
         ;
      else if (whatToDo == DO_VPNR)
         ;

      ReportUsage("program finish",RES_TIMES|RES_CORE);
      for (wi = warningCount; wi->count != -1; wi++)
         if (wi->count > 0) {
            if (diag)
               Message("DATA CONSISTENCY DIAGNOSTICS:");
            diag = false;
            Message("%d %s warnings",wi->count,wi->info);
         }
      Message("translation completed succesfully...\n");
   } else {
      InitError("The proper usage is");
      Message(usage);
   }
   exit(0);
   return 0;   /* To shut up lint */
}
/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set ts=3 sw=3:
 */
