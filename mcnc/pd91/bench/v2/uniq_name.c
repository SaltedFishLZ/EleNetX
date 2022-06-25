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
#include "unixproto.h"
#define HASH_STATS

/*####################### Local variables and definitions. ###################*/

/* Table of primes used for determining the hash table length.
 */
#define MAXPRIMES 12
static Size primes[MAXPRIMES] = { 101,307,1009,3001,10007,30011,100003,200003
                           ,300007,1000003,3000017,10000019};

/* Characters allowed to appear in names.
 */
static char allowedChars[63] =
   "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char letters[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* Data structure used in a hash table for linking the records in buckets.
 */
typedef struct HASH_REC *HashPtr;
typedef struct HASH_REC {
   char *name;       /* Pointer to the identifier string.              */
   HashPtr link;     /* Next record in the bucket.                  */
   voidptr data;     /* Data associated with the identifier.              */
} HashRecord;

#ifdef ANSI_PROTO
HashPtr HashName(char*,Boolean*);
#else
HashPtr HashName();
#endif ANSI_PROTO

/* Structure for my own memory management.
 */
#define  BANKSIZE 4080     /* With all overheads, MemBank uses 4kBytes   */
typedef struct MY_MEMORY *MemBankPtr;
typedef struct MY_MEMORY {
   MemBankPtr next;        /* Next memory bank.                   */
   Size avail;             /* Available, unused element of this array.    */
   union {
      char strings[BANKSIZE];
      HashRecord names[BANKSIZE/sizeof(HashRecord)];
   } vault;
} MemBank;

/* Data structure storing information about some particular hash table.
 */
typedef struct {
   HashPtr *hashTable;     /* Hash table itself.                       */
   int numPrime;        /* Index of the current hash table length.        */
   long numObjects;     /* Number of objects in this table.            */
   Size numToRehash;    /* Number of objects that will cause a rehash.    */
   MemBankPtr allStrings;  /* Storage used by this table for identifiers.    */
   MemBankPtr allNames; /* Storage used for hash records.              */
#ifdef HASH_STATS
   int *hashCongestion; /* Counts of items in buckets.                 */
   Size numQueries;     /* Number of queries issued to this table.        */
   Size numSearches;    /* Number of searches in this table.           */
   Size maxCongestion;     /* Maximal conestion in this table.            */
#endif HASH_STATS
} HashTable, *HashTablePtr;

/* All hash tables are stored in an array of hash tables. The first table in the
 * array is the main hash table.
 */
static HashTablePtr allHashTables = NULL;
static Size     numHashTables = 0;
static float   rehashCriterion = 2.0;

/* Current hash table itself and current variables used to gather statistics.
 * These variables are copied from/into the table upon setting the use of the
 * given table.
 */
static HashTablePtr currHashTable = NULL;
static HashPtr *currTable = NULL;
static int numPrime = 0;      /* Index of the current hash table length.    */
static Size numHashEntries;      /* Current hash table length.             */
static Size numObjects = 0;      /* Number of object in the table.           */
static MemBankPtr allStrings = NULL;
static MemBankPtr allNames = NULL;  

#ifdef ANSI_PROTO
   static HashPtr newHashRecord(MemBankPtr *);
   static void freeMemoryBanks(MemBankPtr);
   static void initHashTable(HashTablePtr,Boolean);
   static HashPtr findName1(char*,HashTablePtr);
   static HashPtr findName(char*);
   static char *cloneName(char*,MemBankPtr*);
   static HashPtr addName(char*);
   static void rehashTable(HashTablePtr);
#else ANSI_PROTO
   static void rehashTable();
   static HashPtr newHashRecord();
   static void freeMemoryBanks();
   static void initHashTable();
   static HashPtr findName1();
   static HashPtr findName();
   static char *cloneName();
   static HashPtr addName();
#endif ANSI_PROTO

/*####################### Local functions. ###################################*/

/*----------------------- rehash Table ---------------------------------------*/
/* This procedure rehashes an overly congested table.
 */
static void rehashTable(htp)
HashTablePtr htp;
{
   register HashPtr *oldTable;
   register HashPtr *newTable;
   register HashPtr *hpp;
   register HashPtr np;
   register int newPrime = htp->numPrime;
   register Size oldEntries, newEntries;
   register int i;
   register Size location;
#ifdef HASH_STATS
   register int *hc;
#endif HASH_STATS
   ASSERT_PTR(htp);
   
   if (newPrime >= MAXPRIMES)
      return;

   oldTable = htp->hashTable;
   newPrime = htp->numPrime;
   oldEntries = primes[newPrime];
   newPrime += 1;
   newEntries = primes[newPrime];

   NEW_ARRAY(newTable,newEntries);
   htp->hashTable = newTable;
   htp->numPrime = newPrime;
   htp->numToRehash = newEntries * rehashCriterion;
#ifdef HASH_STATS
   REALLOC_ARRAY(htp->hashCongestion,newEntries);
   htp->maxCongestion   = 0;
   hc = htp->hashCongestion;
#endif HASH_STATS
   
   for (hpp = htp->hashTable, i=newEntries; i > 0; i-- ) {
      *(hpp++) = NULL;
#ifdef HASH_STATS
      *(hc++) = 0;
#endif HASH_STATS
   }

   for (hpp = oldTable + oldEntries; hpp-- > oldTable; )
      for ( ; np= *hpp, np != NULL; ) {
         *hpp = np->link;
         location = HashString1(np->name,newEntries);
         np->link = newTable[location];
         newTable[location] = np;

#ifdef HASH_STATS
         location = htp->hashCongestion[location] += 1;
         if (htp->maxCongestion < location)
            htp->maxCongestion = location;
#endif HASH_STATS
         
      }
   FREE(oldTable);
   if (htp == currHashTable) {
      currTable = newTable;
      numPrime = newPrime;
      numHashEntries = newEntries;
   }
}

/*----------------------- init Hash Table ------------------------------------*/
/* This procedure set up an empty hash table.
 */

static void initHashTable(htp,doAlloc)
register HashTablePtr htp;
Boolean doAlloc;
{
   register HashPtr *hpp;
#ifdef HASH_STATS
   register int *hc;
#endif HASH_STATS 
   register int i = primes[0];               /* Start with 101 entries     */
   ASSERT_PTR(htp);

   if (doAlloc)
      NEW_ARRAY(htp->hashTable,i);
   else
      htp->hashTable = NULL;
   htp->numPrime = 0;
   htp->numToRehash = primes[0] * rehashCriterion;
   htp->numObjects   = 0;
   htp->allStrings   = NULL;
   htp->allNames  = NULL;
#ifdef HASH_STATS
   if (doAlloc)
      NEW_ARRAY(htp->hashCongestion,i);
   else
      htp->hashCongestion = NULL;
   hc = htp->hashCongestion;
   htp->numQueries      = 0;
   htp->numSearches  = 0;
   htp->maxCongestion   = 0;
#endif HASH_STATS 
   if (!doAlloc)
      return;
   for (hpp = htp->hashTable; i > 0; i-- ) {
      *(hpp++) = NULL;
#ifdef HASH_STATS
      if (hc != NULL)
         *(hc++) = 0;
#endif HASH_STATS
   }
}

/*----------------------- free Memory Banks ----------------------------------*/
/* This procedure deallocates space taken by a linked list of memory banks.
 */
static void freeMemoryBanks(mp)
register MemBankPtr mp;
{
   while (mp != NULL) {
      register MemBankPtr mpn = mp->next;
      FREE(mp);
      mp = mpn;
   }
}

/*----------------------- new Hash Record ------------------------------------*/
/* Assign a new hash record from the bank of available records.
 */
static HashPtr newHashRecord(allBanks)
MemBankPtr *allBanks;
{
   register MemBankPtr thisBank;
   register HashPtr hp;
   ASSERT_PTR(allBanks);

   thisBank = *allBanks;
   if ((thisBank == NULL) || (thisBank->avail <= 0)) {   /* Need a new bank.    */
      NEW_PTR(thisBank);
      thisBank->avail = BANKSIZE/sizeof(HashRecord);
      thisBank->next = *allBanks;
      *allBanks = thisBank;
   }
   thisBank->avail -= 1;
   hp = thisBank->vault.names + thisBank->avail;
   /* hp->link = NULL; hp->name = NULL; hp->data = NULL; */
   return(hp);
}

/*----------------------- clone Name -----------------------------------------*/
/* Put a copy of the 'name' string in local storage. Algorithm:
 *   - If 'name' is too long to fit in a bank, make a separate clone and return.
 *   - Otherwise,  find the first available memory bank that will accomodate the
 *     name. If no bank can be found, allocate a new one.
 *   - copy the string into the bank vault.
 */
static char *cloneName(name,allBanks)
register char *name;
register MemBankPtr *allBanks;
{
   register MemBankPtr thisBank;
   register Size nameLength = strlen(name);
   ASSERT_PTR(name);
   ASSERT_PTR(allBanks);

   if (nameLength >= BANKSIZE) {    /* Need to make an unique bank.       */
      thisBank = MessPtr(sizeof(*thisBank) - BANKSIZE + nameLength + 1);
      thisBank->avail = 0;
      thisBank->next = *allBanks;
      *allBanks = thisBank;
   } else {
      thisBank = *allBanks;
      while (thisBank != NULL)
         if ((thisBank->avail + nameLength) < BANKSIZE)
            break;
         else
            thisBank = thisBank->next;
      if (thisBank == NULL) {
         NEW_PTR(thisBank);
         thisBank->avail = 0;
         thisBank->next = *allBanks;
         *allBanks = thisBank;
      }
   }
   strcpy(thisBank->vault.strings + thisBank->avail,name);
   name = thisBank->vault.strings + thisBank->avail;
   thisBank->avail += nameLength + 1;
   return name;
}

/*----------------------- find Name ------------------------------------------*/
/* Returns a pointer to the HashRecord containing 'name', or NULL if there is no
 * such HashRecord.
 */
static HashPtr findName(name)
char* name;
{
   register Size location;
   register HashPtr hp;
   ASSERT_PTR(name);
      
   if (currTable == NULL)
      return NULL;
   location = HashString1(name,numHashEntries);

   hp = currTable[location];
   
#ifdef HASH_STATS
   currHashTable->numQueries += 1;
   currHashTable->numSearches += 1;
#endif HASH_STATS
   while ( (hp != NULL) && strNE(hp->name,name)) {
      hp = hp->link;
#ifdef HASH_STATS
      currHashTable->numSearches += 1;
#endif HASH_STATS
   }
   return hp;
}

/*----------------------- find Name1 -----------------------------------------*/
/* Returns a pointer to the HashRecord containing 'name', or NULL if there is no
 * such HashRecord. Just like findName, except operates on arbitrary table.
 */
static HashPtr findName1(name,thisHashTable)
char* name;
register HashTablePtr thisHashTable;
{
   register Size location;
   register HashPtr hp;
   ASSERT_PTR(name);
      
   if (thisHashTable == NULL)
      return NULL;
   location = HashString1(name,primes[thisHashTable->numPrime]);

   hp = thisHashTable->hashTable[location];
   
#ifdef HASH_STATS
   thisHashTable->numQueries += 1;
   thisHashTable->numSearches += 1;
#endif HASH_STATS
   while ( (hp != NULL) && strNE(hp->name,name)) {
      hp = hp->link;
#ifdef HASH_STATS
      thisHashTable->numSearches += 1;
#endif HASH_STATS
   }
   return hp;
}

/*----------------------- add Name -------------------------------------------*/
/* Add a new name record to the hash table #1 (if one exists) and to the current
 * hash table (if different from #1).
 */

static HashPtr addName(name)
char *name;
{
   register HashPtr hp, hp1;
   register Size location = HashString1(name,numHashEntries);
   register HashPtr *hTable = currTable;

   ASSERT_PTR(name);
   ASSERT_PTR(hTable);

/* Add to the current table (assured to be non-null)
 */
   hp = newHashRecord(&allNames);
   hp->name = cloneName(name,&allStrings);
   hp->link = hTable[location];
   hp->data = NULL;
   hTable[location] = hp;
   numObjects += 1;

#ifdef HASH_STATS
   location = currHashTable->hashCongestion[location] += 1;
   if (currHashTable->maxCongestion < location)
      currHashTable->maxCongestion = location;
#endif HASH_STATS
   if (numObjects > currHashTable->numToRehash)
      rehashTable(currHashTable);
   if ((currHashTable == allHashTables) || (allHashTables->numObjects < 0))
      return (hp);
   
   if (findName1(hp->name,allHashTables) != NULL)
      return (hp);
   hTable = allHashTables->hashTable;

   location = HashString1(name,primes[allHashTables->numPrime]);
   hp1 = newHashRecord(&allHashTables->allNames);
   hp1->name = cloneName(name,&allHashTables->allStrings);
   hp1->link = hTable[location];
   hp1->data = NULL;
   hTable[location] = hp1;
   allHashTables->numObjects += 1;

#ifdef HASH_STATS
   location = allHashTables->hashCongestion[location] += 1;
   if (allHashTables->maxCongestion < location)
      allHashTables->maxCongestion = location;
#endif HASH_STATS
   if (allHashTables->numObjects > allHashTables->numToRehash)
      rehashTable(allHashTables);

   return (hp);
}

/*####################### Global functions. ##################################*/

/*----------------------- Make Unique Name -----------------------------------*/
/* Create a name that is not present in the hash table yet.  Parameter 'format'
 * is a C-style formatting style for sprintf, and parameter 'firstChars' points
 * to an array containing admissible first characters. Altogether, there can be
 * 62**6 * strlen(firstChars) possible unique identifiers.
 */

char *MakeUniqueName(format,firstChars)
char *format;
char *firstChars;
{
   static Size nameNumber = 0;
   Size initialValue;
   static char *buffer = NULL;
   static Size bufLen = 256;
   int first;
   Size needBufLen = (format ? strlen(format) : 0) + 8;
   Size numFirst;
   Size numAllowed;
   char uniq[8];  /* The string that makes the name unique. */
   Size remainder;

   if (currTable == NULL)
      UseHashTable(1);
   if ((firstChars == NULL) || (firstChars[0] == EOS))
      firstChars = letters;
   if ((format == NULL) || (format[0] == EOS))
      format = "%s";
   
   numFirst = strlen(firstChars);
   numAllowed = strlen(allowedChars);

   if (buffer == NULL)
      buffer = MessPtr(bufLen);  /* Should be plenty enough... */
   if (needBufLen > bufLen) {    /* ... but make sure anyway.  */
      if (buffer != NULL)
         FREE(buffer);
      buffer = MessPtr(needBufLen);
      bufLen = needBufLen;
   }

   sprintf(buffer,format,"%s");
   if (strNE(buffer,format)) {      /* Incorrect format supplied. */
      PgmErrorReport("Incorrect format (%s) in MakeUniqueName",format);
      return NULL;
   }

   initialValue = nameNumber;
   do {
      int uniqLen = 1;
      first = nameNumber % numFirst;
      remainder = (nameNumber - first) / numFirst;
      uniq[0] = firstChars[first];
      uniq[1] = EOS;
      while ((remainder != 0) && (uniqLen < 7)) {
         first = remainder % numAllowed;
         remainder = ( remainder - first) / numAllowed;
         uniq[uniqLen] = allowedChars[first];
         uniqLen += 1;
         uniq[uniqLen] = EOS;
      }
      nameNumber += 1;
      sprintf(buffer,format,uniq);
      if (findName(buffer) == NULL) {
         HashPtr hp = addName(buffer);
         return (hp->name);
      }
   } while (initialValue != nameNumber);

   return NULL;   /* Should never get here ... */
}

/*----------------------- Is Unique Name -------------------------------------*/
/* Return 'true' if the 'name' is not present in the hash table yet.
 */

Boolean IsUniqueName(name)
char* name;
{
   return (Boolean) (findName(name) == NULL);
}

/*----------------------- Register Name --------------------------------------*/
/* Add 'name' to the list of unique names.   If it already is in the hash table,
 * 'false' is returned and nothing else happens.  Otherwise, if 'name' is indeed
 * new,  a HashRecord is created and placed in the hash table,  and the function
 * returns 'true'.
 */

Boolean RegisterName(name,permName)
char* name;
char** permName;
{
   register HashPtr hp;
   register Boolean result;
      
   if (currTable == NULL)
      UseHashTable(1);

   hp = findName(name);
   result = (Boolean) (hp == NULL);
   if (result)
      hp = addName(name);
   if (permName != NULL)
      *permName = hp->name;
   return (result);
}

/*----------------------- Hash Name ------------------------------------------*/
/* Same as 'RegisterName',  except returns a pointer to the entire hash record,
 * and sets 'result' true iff a new alias has been created.
 */

HashPtr HashName(name,result)
char* name;
Boolean *result;
{
   register HashPtr hp;
      
   if (currTable == NULL)
      UseHashTable(1);

   hp = findName(name);
   *result = (Boolean) (hp == NULL);
   if (*result)
      hp = addName(name);
   return (hp);
}

/*----------------------- Set Name Data --------------------------------------*/
/* Same as 'RegisterName' except allows to associate data with the registered
 * name.
 */

Boolean SetNameData(name,permName,dataIn)
register char* name;
register char** permName;
register voidptr dataIn;
{
   register HashPtr hp;
   register Boolean result;
      
   if (currTable == NULL)
      UseHashTable(1);

   hp = findName(name);
   
   result = (Boolean) (hp == NULL);

   if (result)
      hp = addName(name);

   if (permName != NULL)
      *permName = hp->name;

   hp->data = dataIn;

   return (result);
}

/*----------------------- Get Name Data --------------------------------------*/
/* Returns data associated with the registered name.
 */

voidptr GetNameData(name)
register char* name;
{
   register HashPtr hp;
      
   if (currTable == NULL)
      UseHashTable(1);

   hp = findName(name);
   if (hp == NULL)
      return NULL;
   else
      return hp->data;
}

/*----------------------- Hash Report ----------------------------------------*/
/* Report the hashing effectiveness (for debugging purposes only).
 */
void HashReport()
{
#ifdef HASH_STATS
   double gotten, expected, overhead;

   if (currHashTable->numQueries > 0)
      gotten = (double)currHashTable->numSearches/(double)currHashTable->numQueries;
   else
      gotten = 0;
   expected = ((double)numObjects/numHashEntries)/2.0 + 1.0;
   overhead = gotten/expected;

   Message("Hash %ld objects: %ld slots, %ld queries, %ld trials"
      ,(long)numObjects,(long) numHashEntries
      ,(long)(currHashTable->numQueries)
      ,(long)(currHashTable->numSearches));
   Message("Hashing stats: Average/max congestion %.3g/%ld"
      ,(double)numObjects/(double)numHashEntries
      ,(long)(currHashTable->maxCongestion));
   Message("Overhead %.3g%%: %.3g trials/query, expected: %.3g"
      ,(overhead - 1.0)*100,gotten,expected);
#else HASH_STATS
   Message("Hashing statistics not available");
#endif HASH_STATS
}

/*------------------- Mess Ptr -----------------------------------------------*/
/* Allocate new memory and set the contents to all-1 bits to get bus errors
 * quickly upon using unintialized memory.
 */

voidptr MessPtr(size)
register Size size;
{
    extern voidptr malloc();
    register voidptr ptr = malloc(size);

    register char *p = ptr;
    
    if (ptr == NULL) {
      Message("out of memory");
      exit(1);
    }

    while (size-- > 0)
   *(p++) = 0xFF;
    return ptr;
}

/*----------------------- Use Hash Table -------------------------------------*/
/* Hash table management procedure.  Depending on the value of 'np', it does one
 * of the following:
 *  0       - allocates a new hash table,  makes it current, and returns its
 *            identifier (always > 0) to the user.
 *  negative   - deallocates an existing hash table.   Prevents creation of the
 *            main hash table if np == -1.
 *  positive   - makes the existing, initialized table current. If np == 1, the
 *            table need not to exist (it will be initialized if necessary),
 *            but it must not have been deallocated.
 * Deallocating a non-existent table or making it current,  except for the table
 * number 1, causes an an error message and returns (-1).  Otherwise, the number
 * of the current hash table  is returned.  Return result of zero indicates that
 * the current hash table has been deallocated.  
 */
int UseHashTable(np)
int np;
{
   register Size i;
   register HashTablePtr htp = allHashTables + np - 1;
   
   if ((currHashTable != NULL) && (currHashTable == htp) && (np > 0))
      return np;                       /* We're using it right now.  */

/* Whatever the current table was - put it back where it used to be.
 */
   if (currHashTable != NULL) {
      currHashTable->hashTable = currTable;
      currHashTable->numPrime = numPrime;
      currHashTable->numObjects = numObjects;
      currHashTable->allStrings = allStrings;
      currHashTable->allNames = allNames;
      currHashTable = NULL;
      currTable = NULL;
      allNames = allStrings = NULL;
      numPrime = numHashEntries = numObjects = 0;
   }

   if (allHashTables == NULL) {           /* Initialize main table.     */
      NEW_PTR(allHashTables);
      numHashTables = 1;
      initHashTable(allHashTables,false);
   }
   htp = allHashTables;
   if (np == 1) {                      /* Use the main table.        */
      if (htp->hashTable == NULL)
         initHashTable(htp,true);
   } else if (np == 0) {                  /* Assign a new table.        */
      if ( (htp->hashTable == NULL) && (htp->numObjects == 0) )
         initHashTable(htp,true);
      htp += 1;
      i = numHashTables - 1;
      for ( ; (i > 0) && (htp->hashTable != NULL); i--)
         htp++;
      if (i <= 0) {                    /* Need one more table.       */
         numHashTables += 1;
         REALLOC_ARRAY(allHashTables,numHashTables);
         htp = allHashTables + numHashTables - 1;
      }
      initHashTable(htp,true);
   } else if (np > 1) {                /* Make table current.        */
      htp = allHashTables + np - 1;
      if (np > numHashTables) {
         PgmErrorReport("Table number %d does not exist",(int) np);
         return (-1);
      } else if (htp->hashTable == NULL) {
         PgmErrorReport("Table number %d has been deallocated",(int) np);
         return (-1);
      }
   } else { /* if (np < 0) */          /* Get rid of a table.        */
      htp = allHashTables - np - 1;
      if (np == -1)
         htp->numObjects = -1;            /* Assure non-usage.      */
      if (htp->hashTable == NULL)            /* Was inactive already.     */
         return (0);
      FREE(htp->hashTable);
      freeMemoryBanks(htp->allStrings);      /* Deallocate strings.       */
      freeMemoryBanks(htp->allNames);        /* Deallocate hash records.     */
#ifdef HASH_STATS
      FREE(htp->hashCongestion);
#endif HASH_STATS
      initHashTable(htp,false);           /* Make it available.        */
      if (np == -1)
         htp->numObjects = -1;            /* Assure non-usage.      */
      return (0);
   }

/* Code for making the selected table current.
 */
   currHashTable = htp;
   currTable = htp->hashTable;
   numPrime = htp->numPrime;
   numHashEntries = primes[numPrime];
   numObjects = htp->numObjects;
   allStrings = htp->allStrings;
   allNames = htp->allNames;
   return (int) ((htp - allHashTables) + 1);
}

/*----------------------- Rehash Criterion -----------------------------------*/
/* Set the value of the maximal hash table congestion allowed to occur in a hash
 * table.  If the average congestion is exceeded, the table is rehashed.
 */
void RehashCriterion(newCriterion)
double newCriterion;
{
   if (newCriterion > 1)
      rehashCriterion = newCriterion;
}

/*----------------------- Rehash Table ---------------------------------------*/
/* Rehash the table into the next larger prime.  Return the table size
 * after rehashing.
 */
Size RehashTable(tableID)
int tableID;
{
   HashTablePtr htp = allHashTables + tableID;

   if ((tableID < 1) || (tableID > numHashTables) || (htp->hashTable == NULL))
      return (-1);
   
   rehashTable(htp);
   return primes[htp->numPrime];
}

/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set tabstop=4 sw=4:
 */
