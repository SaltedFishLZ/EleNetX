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
#ifndef _oasisutils_h_
#define _oasisutils_h_

/* This file contains assorted useful macros. Some macros are machine-dependent,
 * some are operating-system dependent, and some are compiler-dependent;  all of
 * these dependencies are captured by defining appropriate macros on the command
 * line,  e.g.,  "cc -Dunix ...". For now, there are only two macros of interest
 * to unix users:
 *	-Dunix		- should be set on any UNIX machine
 *	-DANSI_PROTO	- should be set for compilers that  accept extensions of
 *			  C that were introduced in ANSI standard (void pointers
 *			  and function prototypes). There is no need for this if
 *			  one is using 'gcc' for compilation.
 */

#define LIGHTSPEED_C		/* Assume we're on the Macintosh.	      */

#ifdef unix
#include <stdio.h>	/* No need if on Macintosh ...		      */
#include <ctype.h>	/* No need if on Macintosh ...		      */
#include <sys/types.h>
#undef LIGHTSPEED_C	/* Too bad - we're running on some clunker... */
#ifdef __GNUC__
#ifndef ANSI_PROTO
#define ANSI_PROTO	/* ... but at least a decent ANSI compiler.   */
#endif ANSI_PROTO
#endif __GNUC__
#endif unix

#ifdef LIGHTSPEED_C
#ifndef ANSI_PROTO
#define ANSI_PROTO
#endif ANSI_PROTO
#endif LIGHTSPEED_C

/* If we're running ANSI C, define a generic pointer that can be assigned to and
 * from any other pointer.  Otherwise, use char *.
 */

#ifndef VOIDPTR
#define VOIDPTR
#ifdef ANSI_PROTO
typedef void *voidptr;
#else ANSI_PROTO
typedef char *voidptr;
#endif ANSI_PROTO
#endif VOIDPTR

/* Define 'Boolean' type for unix machines. This allows for better type checking
 * to avoid assigning integers to varaibles that really are boolean.
 */

#ifdef unix
#undef FALSE
#undef TRUE
#undef false
#undef true
typedef enum { false = 0, true = 1, FALSE = 0, TRUE = 1 } Boolean;
#endif unix

/* Define 'Size' type for unix machines.  Variable of type 'Size' should contain
 * sufficiently many bytes to store an address on the given machine.  Lightspeed
 * defines 'Size' as 'long'; for unix machines, 'int' should normally suffice.
 */

#ifdef unix
typedef unsigned int Size;
#endif unix

/* Two tricky but useful macros; both are compiler-dependent.
 *	MKSTRING(a)	- makes a string containing whatever is the value of 'a'
 *	CONCAT(a,b)	- concatenates two things together into one thing.
 */

#ifdef unix
#ifdef __GNUC__
#define MKSTRING(A) #A
#define CONCAT(A,B) A##B
#else __GNUC__
#define MKSTRING(A) "A"
#define CONCAT(A,B) A/**/B
#endif __GNUC__
#else unix
#ifdef LIGHTSPEED_C
#define MKSTRING(A) #A
#define CONCAT(A,B) A##B
#endif LIGHTSPEED_C
#endif unix

/* These are macros useful in program development and debugging. Each of ASSERT,
 * ASSERT1, ASSERT2 and ASSERT3 macros evaluates boolean expression 'cond', and,
 * if it evaluates true, nothing happens.  If the expression evaluates false,  a
 * printout is made to standard error. Parameter A_text usually will be a format
 * string;  A_1, A_2, A_3 usually will be variables to print in this format.
 *
 * NOTE:  evaluating the parameters to the assert macros shold not have any side
 * effects (no parameters like "++i")!  The debugging macros may not be compiled
 * into the final code (see tips and suggestions below).
 *
 * Five additional macros are defined that do the following tests:
 *
 * ASSERT_ARRAY:  tests whether the value of index 'ind' is legal to reference a
 *		  given array which is defined as array_name[max] ('ind' must be
 *		  less than 'max' and greater than 0 to be legal).
 * ASSERT_EVEN:	  test whether pointer 'p' points to an even location.
 * ASSERT_POINTS: test whether pointer 'p' is non-NULL.
 * ASSERT_PTR:	  test both of the above.
 * ASSERT_NULL:	  test whether pointer 'p' is NULL.
 *
 * Suggestions:
 *
 * - Defining NO_ASSERT before "oasisutils.h" is included,  e.g., on the command
 *   line, removes all assert code without changing the source file - all assert
 *   macros evaluate to empty string.  This is useful for the final version of a
 *   program, or in files that are thoroughly tested and debugged.
 *
 * - defining DO_EXIT_IN_ASSERT causes exit to shell if any assertion test fails
 *   By default, the execution continues and usually quickly dies with something
 *   like segmentation violation or bus error.
 */

#ifndef NO_ASSERT
#define ASSERT(cond,text)
#define ASSERT1(cond,text,var)
#define ASSERT2(cond,text,var1,var2)
#define ASSERT3(cond,text,var1,var2,var3)
#else NO_ASSERT
#ifdef DO_EXIT_IN_ASSERT
	#define EXIT_TO_SHELL	exit(1)
#else DO_EXIT_IN_ASSERT
	#define EXIT_TO_SHELL
#endif DO_EXIT_IN_ASSERT

#ifdef LIGHTSPEED_C
	#define ASSFAIL  DebugStr("\passert fail")
#else LIGHTSPEED_C
	#define ASSFAIL  DebugStr("assert fail")
#endif LIGHTSPEED_C

#define ASSERT(A_cond,A_text)	\
{if (!(A_cond)) { fprintf(stderr,A_text); ASSFAIL; }}
#define ASSERT1(A_cond,A_text,A_1)	\
{if (!(A_cond)) { fprintf(stderr,(A_text),(A_1)); ASSFAIL;}}
#define ASSERT2(A_cond,A_text,A_1,A_2)	\
{if (!(A_cond)) { fprintf(stderr,(A_text),(A_1),(A_2)); ASSFAIL;}}
#define ASSERT3(A_cond,A_text,A_1,A_2,A_3)	\
{if (!(A_cond)) { fprintf(stderr,(A_text),(A_1),(A_2),(A_3));ASSFAIL;}}

#endif NO_ASSERT

/* Note the tricky conversion of variable names into strings - just like in the
 * 'MKSTRING' macros.
 */

#ifdef unix
#define ASSERT_EVEN(p)\
	ASSERT1((((long)(p)) & 1) == 0,"bad pointer %s","p")
#define ASSERT_DATA(p)
	ASSERT1((((long)p)&0x70000000) == 0,"stack pointer %s","p");
#define ASSERT_POINTS(p)
	ASSERT1((p)!=NULL,"null pointer %s","p");
#define ASSERT_PTR(p) ASSERT_EVEN(p); ASSERT_POINTS(p); ASSERT_DATA(p)
#define ASSERT_ARRAY(arr,ind,max)	\
	ASSERT2(((ind)>=0)&&((ind)<(max))\
		,"index to %s out of bounds (%ld)","arr",(long)(ind))
#define ASSERT_NULL(p) \
	ASSERT1((p)==NULL,"non-null pointer %s","p")
#else unix
#define ASSERT_EVEN(p)\
	ASSERT1((((long)(p)) & 1) == 0,"bad pointer %s",#p)
#define ASSERT_DATA(p)
	ASSERT1((((long)p)&0x70000000) == 0,"stack pointer %s","p");
#define ASSERT_POINTS(p)
	ASSERT1((p)!=NULL,"null pointer %s",#p);
#define ASSERT_PTR(p) ASSERT_EVEN(p); ASSERT_POINTS(p); ASSERT_DATA(p)
#define ASSERT_ARRAY(arr,ind,max)	\
	ASSERT2(((ind)>=0)&&((ind)<(max))\
		,"index to %s out of bounds (%ld)",#arr,(long)(ind))
#define ASSERT_NULL(p) \
	ASSERT1((p)==NULL,"non-null pointer %s",#p)
#endif unix

#define FREE(p)	ASSERT_PTR(p); (void)free(p)

/* The definition of EXT is here for the purpose of defining global variables as
 * external in all modules except for the main module, and to be able to declare
 * these variables only in one file.  Here's how to do this:
 *	- use GLOBVAR and GLOBARR macros in the header  file that is included in
 *	  every file that uses the global variables;  the #include directive for
 *	  this file should be  preceded by including "oasisutils.h" or else this
 *	  file should include "oasisutils.h" itself.
 *	- define MAIN_PROGRAM_MODULE macro in exactle one source file,  *before*
 *	  the inclusion of "oasisutils.h".
 * The third argument of  'GLOBVAR'  is either empty or contains an initializer;
 * here are examples:
 *	GLOBVAR(int,i,);
 *	GLOBVAR(int,j,=45);
 * Here's how to initialize global arrays:
 *	GLOBARR(int,i_array,10)
 *	#ifdef MAIN_PROGRAM_MODULE
 *		= { 1,2,3,4,5,6,7,8,9,10 }
 *	#endif MAIN_PROGRAM_MODULE
 *	;
 * BE SURE TO PUT THE SEMICOLON just after the '#endif' !!
 */

#ifdef MAIN_PROGRAM_MODULE
#define EXT
#define GLOBVAR(t_,v_,i_)	t_ v_ i_
#else  MAIN_PROGRAM_MODULE
#define EXT			extern
#define GLOBVAR(t_,v_,i_)	extern t_ v_
#endif MAIN_PROGRAM_MODULE

#define GLOBARR(typ,v,dim)		EXT typ v[dim]

/* A couple of other useful macros - all self-explanatory.
 */

#define MAX2(a,b)		( ((a) > (b)) ? (a) : (b) )
#define MIN2(a,b)		( ((a) < (b)) ? (a) : (b) )
#define ABS(x)			(((x) >= 0) ? (x) : -(x))
#define ROUND(x)		((int) (((x) >= 0) ? (x) + .5 : (x) - .5 ))
#define BIT_MASK(i)		( 1L << (i) )

/* Swapping variables and structs */

#ifndef SWAP
#define SWAP(t,a,b)		{register t SWAP_x;SWAP_x=(a);a=(b);b=SWAP_x;}
#endif SWAP
#define SWAPSTRUCT(t,a,b)	{t SWAP_x;SWAP_x=(a);a=(b);b=SWAP_x;}

/* True modulo - guaranteeed >= 0 */

#define MODULO(a,b)		( ( ((a)%(b)) + (b)) % (b))

/* Character- and string-related definitions */

#define IS_PREFIX(pref,str)	(strncmp((pref),(str),strlen(pref)) == 0)
#define strNE(s1,s2)		(strcmp((s1),(s2)) != 0)
#define strEQ(s1,s2)		(strcmp((s1),(s2)) == 0)
#define strnNE(s1,s2,l)		(strncmp((s1),(s2),(l)) != 0)
#define strnEQ(s1,s2,l)		(strncmp((s1),(s2),(l)) == 0)
#define EOL			'\n'
#define EOS			'\0'
#define FORMFEED		'\014'
#define EMPTYSTR(s)		(*(s) == EOS)

/* This pointer should be out-of range on almost all machines. */

#define BADPTR			((voidptr) -1)

/* Finally, declarations of subroutines in the oasis utilities library.
 */

#ifdef ANSI_PROTO
Size HashString1(char *,Size);
Size HashString2(char *,Boolean,Size,Size *,Size *);
Boolean RegisterName(char*,char**);
Boolean IsUniqueName(char*);
char *MakeUniqueName(char*,char*);
Boolean NameData(char*,char**,voidptr,voidptr*);
Boolean SetNameData(char*,char**,voidptr);
voidptr GetNameData(char*);
int UseHashTable(int);
Size RehashTable(int);
void RehashCriterion(double);
voidptr MessPtr(Size);
void HashReport(void);
#ifndef _error_c_
void PgmErrorReport(char*,...);
void Message(char*,...);
#endif _error_c_
void InitError(char*);
void ReportUsage(char*,int);
void Perror(void);
char *stracc(char*,char*);
char *strclone(char*);
char *myGetWD(void);
Boolean isLink(char*);
Boolean isFile(char*);
Boolean isDir(char*);
u_short fileMode(char*);
char *ExpandTilde(char*);
int MySystem(char*,char*,int);
#else ANSI_PROTO
char *stracc();
char *strclone();
char *myGetWD();
Boolean isLink();
Boolean isFile();
Boolean isDir();
u_short fileMode();
Size HashString1();
Size HashString2();
Boolean RegisterName();
Boolean IsUniqueName();
char *MakeUniqueName();
Boolean NameData();
Boolean SetNameData();
voidptr GetNameData();
int UseHashTable();
Size RehashTable();
void RehashCriterion();
voidptr MessPtr();
void HashReport();
void Message();
void PgmErrorReport();
void InitError();
void ReportUsage();
void Perror();
char *ExpandTilde();
int MySystem();
#endif ANSI_PROTO

#define ERROR_LOC	fprintf(stderr,"\tFile %s, line %d\n",__FILE__,__LINE__)

#define NEW_PTR(a) a = MessPtr((Size) sizeof(*a))
#define NEW_ARRAY(a,n) a = MessPtr((n)*(Size) sizeof(*a))
#define REALLOC_ARRAY(a,n) a = realloc(a,(n)*sizeof(*a))

#define RES_TIMES	0x01
#define RES_CORE	0x02
#define RES_PAGE	0x04

#endif _oasisutils_h_
