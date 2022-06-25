#!/bin/csh
#
#   This code was developed by individuals connected with MCNC
#   and Duke University.
#
#   Copyright 1988 by the Microelectronics Center of North Carolina       
#   All rights reserved.
#
#   $Source: /mcnc/pi/hill/modgen89/bench/src/load/RCS/load.CSH,v $
#   $Date: 89/01/20 09:48:04 $
#   $Author: kk $
#   $Revision: 1.1 $
#
# IMPORTANT: The template for the 'load' shell is called 'load.CSH'. Do
# not change 'load.csh', as any changes there WILL BE IGNORED !!!!  Any
# editing should be done using 'load.CSH'.
#

set tmp = /tmp/ed.$$
set libdir = --LIBDIR--

cp $libdir/parser.tmpl load.zz.c

$libdir/load > $tmp

if ($status != 0) goto err;

/bin/ed load.zz.c < $tmp
/bin/rm -f $tmp

exit 0

err:
/bin/rm -f $tmp load.zz.c
exit 1
