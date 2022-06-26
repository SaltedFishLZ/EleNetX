/*###########################################################################
  
                                                         ###
                                                         ###
  
     #########        #########         ######## #    ######       ######## #
   ###      ###      ###      ##       ##       ##       ###      ##       ##
  ###        ###           ######      ######### #       ###      ######### #
  ###        ###     #######  ###         #######        ###         #######
  ###        ###    ###       ###      #        ##       ###      #        ##
   ###      ###     ###       ###      ##        #       ###      ##        #
     #########       ######### ####    # ########     #######     # ########
  
#############################################################################
  
    Copyright 1988,1989,1990 by the MCNC.
  
    This copy, serial #O0000000, of MCNC proprietary software, is made  
    available under license agreement.  Please heed all nondisclosure
    and internal use restrictions.  
  
###########################################################################*/
#ifdef MCNC
#include "portability.h"
#include "unixproto.h"
#include "oasisutils.h"
#include "niceprint.h"
#include "messages.h"
#else
#include "non_mcnc.h"
#define InitError(a) (a)
extern FILE *yyin;
#endif
#include "xlatr.h"

main(argc, argv)
int             argc;
char           *argv[];
{
    int res;
    char *pgmname = InitError(argv[0]);
    char *filename = NULL, *dot;
    extern void initLexState PROTOTYPE((void));
    extern int yydebug;
    int verbosity = 2;

    cellFun cf;
    netFun nf;

    if (strcmp(pgmname, "xlatr2yal") == 0) {
	cf = PrintYALcell;
	nf = WriteYALnet;
    } else {
	cf = PrintVPNRcell;
	nf = WriteVPNRnet;
    }

    if (argc != 2) {
	Message("One argument required to %s", pgmname);
	exit (1);
    }

#ifdef MCNC
    filename = stracc(strclone(argv[1]),".xxxxx");
#else
    filename = (char*) malloc(strlen(argv[1]) + 7);
    strcpy(filename, argv[1]);
    strcat(filename, ".xxxxx");
#endif
    dot = rindex(filename,'.');
    yydebug = 1;
    yydebug = 0;
 
    strcpy(dot,".lib");
    InitReading(filename, verbosity);
    initLexState(0);
    if (res=yyparse())
	exit(res);
    TimeReport("Library read");

    initLexState(2);
    strcpy(dot,".dpc");
    InitReading(filename, verbosity);
    if (res=yyparse())
	exit(res);
    TimeReport("Structure read");

    initLexState(1);
    strcpy(dot,".chn");
    InitReading(filename, verbosity);
    if (res=yyparse())
	exit(res);
    TimeReport("Channels read");

    strcpy(dot,".des");
    InitReading(filename, verbosity);
    if (res=yyparse())
	exit(res);
    TimeReport("Circuit read");

    strcpy(dot,".a.db");
    WriteDB(filename,0,0,0,cf);
    TimeReport("Database var. a, original pins, original nets");

    strcpy(dot,".c.db");
    WriteDB(filename,1,0,0,cf);
    TimeReport("Database var. c, routed pins, original nets");

    strcpy(dot,".b.db");
    WriteDB(filename,0,1,0,cf);
    TimeReport("Database var. b, original used pins, original nets");

    strcpy(dot,".d.db");
    WriteDB(filename,1,1,0,cf);
    TimeReport("Database var. d, routed pins, no unused pins, original nets");

    strcpy(dot,".a.net");
    WriteVPNR(filename,0,0,nf);
    TimeReport("Circuit var. a, original nets, unused pins hanging)");

    strcpy(dot,".b.net");
    WriteVPNR(filename,1,0,nf);
    TimeReport("Circuit var. b, original nets, no unused pins)");

    MergeNets();

    strcpy(dot,".e.db");
    WriteDB(filename,0,0,1,cf);
    TimeReport("Database var. e, original pins, merged nets");

    strcpy(dot,".g.db");
    WriteDB(filename,1,0,1,cf);
    TimeReport("Database var. g, routed pins, merged nets");

    strcpy(dot,".f.db");
    WriteDB(filename,0,1,1,cf);
    TimeReport("Database var. f, original used pins, merged nets");

    strcpy(dot,".h.db");
    WriteDB(filename,1,1,1,cf);
    TimeReport("Database var. h, routed pins, no unused pins, merged nets");

    strcpy(dot,".c.net");
    WriteVPNR(filename,0,1,nf);
    TimeReport("Circuit var. c, merged nets, unused pins hanging)");

    strcpy(dot,".d.net");
    WriteVPNR(filename,1,1,nf);
    TimeReport("Circuit var. d, merged nets, no unused pins)");

    exit (0);
    /*NOTREACHED*/
}

/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set se sw=4
 */
