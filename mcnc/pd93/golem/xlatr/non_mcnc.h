/* Uncomment include files that are present in your system */

/* #include <sys/stdtypes.h> */
#include <stddef.h>
/* #include <float.h> */
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#include <signal.h>

#define PROTOTYPE(a) ()
#define Message printf("\n");printf

#define InitReading(a,b)	{ yyin = fopen(a, "r");if (yyin == NULL) {\
				  printf("Cannot open %s\n",a); exit (1); }}
#define TimeReport(a)		printf(a);printf("\n");

#define PgmErrorReport 		Message
#define CurrentLineTxt
#define LEX_RETURN              return

