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
#define NO_ALERT
#include "oasisutils.h"

/*------------------------------- hash.c -------------------------------------*/

/* This file contains two functions used to hash alphanumeric strings. The first
 * function is used for single hashing  (for hash tables that use linked lists).
 * The second function  is used for double hashing,  for hash tables implemented
 * as arrays of pointers to the hashed objects.
 */

#define SCRAMBLE  0x55555555  /* Any ugly bit pattern would do here */

/*
STRING is a pointer to a character string.
VALUE is a pointer to an integer used to return the hash value
TABLE_SIZE is the size of the hash table
*/

#define HASHVALUE(STRING,VALUE,TABLE_SIZE)                                \
   {                                                                      \
   Size ISIZE, CSIZE, STRADDR, LOOP4;                                     \
   char  *END, *CPTR;                                                     \
                                                                          \
   ISIZE = (Size) sizeof (Size);                                          \
   CSIZE = (Size) sizeof (char);                                          \
   END = STRING + strlen (STRING);                                        \
   for (CPTR = STRING, *VALUE = 0, STRADDR = (Size)(*STRING); CPTR < END; \
   *VALUE = (*VALUE+STRADDR)%TABLE_SIZE, STRADDR = (Size)(*CPTR))         \
      for (LOOP4 = 0; (++CPTR < END) && (LOOP4 < ISIZE); LOOP4++)         \
         STRADDR = (STRADDR << CSIZE) + (Size) (*CPTR);                   \
   }

/*------------------------------- HashString1 --------------------------------*/

/* This function computes hash value  of an alphanumeric string.   Seems to work
 * fine even for very similar identifiers.
#define VARIANT1
 */

Size HashString1(ident,htsize)
register char *ident;
Size htsize;      /* Size of the hash table (should be a prime) */
{
   register Size off;  
   register Size k;  
   register Size cn = 1;

   ASSERT_PTR(ident);
#ifdef VARIANT1
   for (off=0; *ident != EOS; ident++) {
      off *= 10;
      off += (*ident - '0');
   }
#else VARIANT1
   k = off = 1;
   for (; *ident != EOS; ident++, cn++) {
      off = ((off ^ SCRAMBLE)*((Size) *ident)*cn) ^ k;
      k   = k*((Size) *ident);
   }
#ifdef OBSOLETE
   for (; *ident != EOS; ident++, cn++) {
      off = (off ^ SCRAMBLE)*((Size) *ident)*cn;
      k   = k*((Size) *ident);
      off = off ^ k;
   }
#endif OBSOLETE
#endif VARIANT1
   off %= htsize;
   return (off < 0 ? off + htsize : off);
}


/*------------------------------- HashString2 --------------------------------*/

/* This function computes hash value  of an alphanumeric string.  Double hashing
 * and a sort-of-a-quadratic residue search are used to avoid clustering.  Seems
 * to work fine even for very similar identifiers. The 'first' and 'seconds' are
 * values that should be preserved between consecutiv calls to HashString2.  The
 * best way to do this is to define them as  static variables that are available
 * in all procedures that call HasString2().
 */

Size HashString2(ident,firsttry,htsize,first,second)
register char *ident;
Boolean firsttry;    /* TRUE for the first try, FALSE for >=2 try. */
register Size htsize;      /* Size of the hash table (should be a prime) */
register Size *first, *second;   /* Hash values.                  */
{
   register Size k;  
   register Size off;  

   ASSERT_PTR(ident);
   ASSERT_PTR(first);
   ASSERT_PTR(second);

   if (firsttry) {
      register Size cn = 1;
      k = off = 1;
      for (; *ident != EOS; ident++, cn++) {
         off = (off ^ SCRAMBLE)*((Size) *ident)*cn;
         k   = k*((Size) *ident);
         off = off ^ k;
      }
      *second = k;
   } else {
      k = *second;
      off = (*first + (k*k));
      *second = (k+1) % htsize;
   }
   return (*first = MODULO(off,htsize));
}
