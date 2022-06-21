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
#ifndef _unixproto_h_
#define _unixproto_h_

#ifdef ANSI_PROTO
#ifndef VOIDPTR
typedef void *voidptr;
#define VOIDPTR
#endif VOIDPTR
extern voidptr xrealloc(voidptr,int);
extern voidptr xcalloc(int);
extern voidptr xalloc(int);
extern voidptr calloc(int,int);
extern voidptr realloc(voidptr,int);
extern int fprintf(FILE*,char*,...);
extern int fscanf(FILE*,char*,...);
extern int printf(char*,...);
extern int strcmp(char*,char*);
extern int strlen(char*);
extern int scanf(char*,...);
extern int sscanf(char*,char*,...);
extern char *strcpy(char*,char*);
extern char *strcat(char*,char*);
extern int free(voidptr);
extern int fclose(FILE*);
extern int fflush(FILE*);
extern int exit(int);
extern char *rindex(char*,int);
extern char *index(char*,int);
extern long random(void);
extern int abort(void);
extern void perror(char*);
extern double atof(char*);
#else ANSI_PROTO
#ifndef VOIDPTR
typedef char *voidptr;
#define VOIDPTR
#endif VOIDPTR
extern char* xrealloc();
extern char* xcalloc();
extern char* xalloc();
extern char* calloc();
extern char* realloc();
extern int fprintf();
extern int fscanf();
extern int printf();
extern int strcmp();
extern int strlen();
extern char *strcpy();
extern char *strcat();
extern int scanf();
extern int sscanf();
extern int free();
extern int fclose();
extern int fflush();
extern int exit();
extern char *rindex();
extern char *index();
extern long random();
extern int abort();
extern void perror();
extern double atof();
#endif ANSI_PROTO

#endif _unixproto_h_
