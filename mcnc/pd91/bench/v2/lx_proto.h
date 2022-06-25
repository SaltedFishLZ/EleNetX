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

#ifdef ANSI_PROTO

int yywrap(void);
void yyerror(char*);
void SetInputFileName(char*);
char *index(char*,char);
char *calloc(int,int);
int   strlen(char*);
void  exit(int);
int   fprintf(FILE*,char*,...);

#else ANSI_PROTO

int yywrap();
void yyerror();
void SetInputFileName();
char *index();
char *calloc();
int   strlen();
void  exit();
int   fprintf();

#endif ANSI_PROTO
