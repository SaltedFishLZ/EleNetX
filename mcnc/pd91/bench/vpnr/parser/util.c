/*
 *   This code was developed by individuals connected with MCNC
 *   and Duke University.
 *
 *   Copyright 1988 by the Microelectronics Center of North Carolina       
 *   All rights reserved.
 *
 *   $Source: /mcnc/pi/hill/modgen89/bench/src/parser/RCS/util.c,v $
 *   $Date: 89/01/20 09:47:54 $
 *   $Author: kk $
 *   $Revision: 1.1 $
 *
 */

#include <stdio.h>       /* Standard I/O functions */

/* Conversion of an integer into a string */

itoa(n,s)
	char s[];
	int n;
{
	sprintf(s,"%d",n);
}

/* Much needed function to make a clone of a string */

char *strclone(str)
char *str;
{
  extern char *strcpy();
  char *copied = (char*) calloc ( (unsigned) (strlen(str)+1)
				, (unsigned) sizeof(char));
  if (copied == NULL)
	return NULL;
  strcpy(copied,str);
  return(copied);
}
