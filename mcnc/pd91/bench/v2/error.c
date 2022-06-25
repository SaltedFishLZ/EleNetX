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
#define _error_c_

#include <stdio.h>
#include "oasisutils.h"
#include "unixproto.h"
#include <sys/time.h>
#include <sys/resource.h>

#define MCNC

#ifdef ANSI_PROTO
extern int getrusage(int,struct rusage *);
#else ANSI_PROTO
extern int getrusage();
#endif ANSI_PROTO

static char *programName = "";

void Perror()
{
   perror(programName);
}

void InitError(pgmname)
char *pgmname;
{
   char *c = rindex(pgmname,'/');
   if (c != NULL)
      programName = c + 1;
   else
      programName = pgmname;
}

void PgmErrorReport(format,a,b,c,d,e,f,g,h,i,j,k)
char *format;
long a,b,c,d,e,f,g,h,i,j,k;
{
   static char *scream = "!!! PROGRAMMING ERROR !!!\n";
   fprintf(stderr,scream);
   if (programName)
      fprintf(stderr,"%s: ",programName);
   fprintf(stderr,format,a,b,c,d,e,f,g,h,i,j,k);
   fprintf(stderr,"\n");
   fprintf(stderr,scream);
}

void Message(format,a,b,c,d,e,f,g,h,i,j,k)
char *format;
long a,b,c,d,e,f,g,h,i,j,k;
{
   static Boolean wasNewline = true;
   if (wasNewline && programName)
      fprintf(stderr,"%s: ",programName);
   fprintf(stderr,format,a,b,c,d,e,f,g,h,i,j,k);

   if (wasNewline = (Boolean) (format[strlen(format) - 1] != ' '))
      fprintf(stderr,"\n");
}

#ifdef MCNC
/* A procedure for reporting resource utilization.  Lint complains about it
 * for no apparent reason and it writes over memory (getrusage call) where not
 * supposed to (if struct rusage r is declared as static).  It is probably safe
 * only if called upon exit.
 */

#define TIMEV(what) what.tv_sec,what.tv_usec/1000

void ReportUsage(txt,what)
char *txt;
int what;
{
    static struct timeval ru_utime = { 0, 0 };
    static struct timeval ru_stime = { 0, 0 };
    struct rusage r;
    static int initialized = 0;
    long stime,utime,stimemicro,utimemicro;

    if ((what & (RES_TIMES|RES_CORE|RES_PAGE)) == 0)
   return;

    (void) getrusage(RUSAGE_SELF,&r);
    stime = r.ru_stime.tv_sec - ru_stime.tv_sec;
    stimemicro = r.ru_stime.tv_usec - ru_stime.tv_usec;
    if (stimemicro < 0) {
   stimemicro += 1000000;
   stime -=1;
    }
    utime = r.ru_utime.tv_sec - ru_utime.tv_sec;
    utimemicro = r.ru_utime.tv_usec - ru_utime.tv_usec;
    if (utimemicro < 0) {
   utimemicro += 1000000;
   utime -=1;
    }

    if (what & RES_TIMES)
        if (initialized) 
       Message("time (%s): user +%D.%D -> %D.%D, system +%D.%D -> %D.%D"
      ,txt ? txt : "",utime,utimemicro/1000,TIMEV(r.ru_utime)
      ,stime,stimemicro/1000,TIMEV(r.ru_stime));
    else {
        initialized = 1;
        Message("time report initalization (%s): user %D.%D, system %D.%D"
            ,txt ? txt : "",TIMEV(r.ru_utime),TIMEV(r.ru_stime));
    }
    if (what & RES_CORE)
        Message("core usage (%s): %d kilobytes",txt?txt:"",r.ru_maxrss);
    if (what & RES_PAGE)
        Message("paging (%s): %d reclaims, %d faults, %d swaps"
            ,txt?txt:"",r.ru_minflt,r.ru_majflt,r.ru_nswap);
    ru_utime = r.ru_utime;
    ru_stime = r.ru_stime;
}
#endif MCNC
