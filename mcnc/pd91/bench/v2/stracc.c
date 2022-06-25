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
#include "oasisutils.h"
#include "unixproto.h"

char *strclone(s)
char *s;
{
	int len = strlen(s) + 1;
	char *ss;

	ASSERT_POINTS(s);
	NEW_ARRAY(ss,len);
	if (ss == NULL)
		return NULL;
	strcpy(ss,s);
	return ss;
}

char *stracc(s1,s2)
char *s1, *s2;
{
	Size len, len1, len2;

	ASSERT_DATA(s1);
	ASSERT_POINTS(s2);
	len1 = (s1 == NULL) ? 0 : strlen(s1);
	len2 = (s2 == NULL) ? 0 : strlen(s2);
	len = len1 + len2 + 1;

	if (s1 == NULL) {
		NEW_ARRAY(s1,len);
		if (s1 != NULL)
			*s1 = '\0';
	} else
		REALLOC_ARRAY(s1,len);
	if (s1 == NULL)
		return NULL;
	if (len2 > 0)
		strcat(s1,s2);
	ASSERT(strlen(s1) == (len-1),"bad concat length");
	return s1;
}
