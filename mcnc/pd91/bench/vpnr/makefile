#*******************************************************************************
#    Copyright 1988 by the Microelectronics Center of North Carolina       
#    All rights reserved.
#
#    makefile template written by Krzysztof Kozminski. VERSION 1.
#
### Normally, all you should do is to edit the lines containing the word
### "UNDEFINED" and add specific instructions.
#	$Header: makefile,v 1.1 89/01/04 13:52:47 kk Locked $
###	$Locker: kk $
###	$Source: /dist/oasis/work/RCS/makefile,v $
#   Modifications to the makefile template done $Date: 89/01/04 13:52:47 $
#   by $Author: kk $
#   This file is a part of OASIS system.
#   $Revision: 1.1 $
#
#
#CoNtEnTs  Contains a central makefile to install all the stuff.
#
#   History of this release:
#	$Log:	makefile,v $
# Revision 1.1  89/01/04  13:52:47  kk
# Initial revision
# 
#
#*******************************************************************************

# Name of this makefile

MAKEFILE=makefile

# If this makefile calls 'make' in subdirectories, all the definitions
# below should be passed forward!

# Names of the standard directories to put the stuff in.

BINDIR	= ../bin
INCDIR	= include
LIBDIR	= lib
MANDIR	= man

# Destdir will be appended before each of the above.  This is the name of the
# directory that will contain all the stuff.

DESTDIR = /mcnc/pi/hill/modgen89/bench/src

# End of the definitions that should be passed forward.  They are put into a
# single definition below.  We also pass forward any flags, e.g., "-k" may be
# quite useful.

PASSDEFS= $(MFLAGS) 'BINDIR=$(BINDIR)' 'LIBDIR=$(LIBDIR)' 'MANDIR=$(MANDIR)'   \
	  'DESTDIR=$(DESTDIR)' 'INCDIR=$(INCDIR)'

FULLBIN	= $(DESTDIR)/$(BINDIR)
FULLLIB	= $(DESTDIR)/$(LIBDIR)
FULLMAN	= $(DESTDIR)/$(MANDIR)
FULLINC	= $(DESTDIR)/$(INCDIR)
ALLDIRS = $(FULLINC) $(FULLLIB) $(FULLBIN) $(FULLMAN)/man1

# Definitions for the C compiler: name, include directories (used in automatic
# creation of dependency lists), other flags (usually -g or -O), link editor
# flags (usually -s)

CC	= cc
IFLAGS	= -I$(FULLINC)
CFLAGS	= -O $(IFLAGS)
LDFLAGS = -s -L$(FULLLIB)

# Flags for yacc and lex.

LFLAGS = -v
YFLAGS = -d

#*******************************************************************************
#
# Now definitions of the stuff to be made

# Programs that live in subdirectories, and have makefiles of their own.
# NOTE: If "make" in subdirectory A is dependent on subdirectory B being
# processed earlier, B should precede A on this list.  Notably, if there are
# any libraries of object code, they should be put on the list of pre-installed
# directories.

PREINSTALL	= parser load
SUBDIR		= sample

# Programs that live in this directory:
#    SIMPLE   - have a single file source and no need for libraries.  They do
#		not need a separate line in the makefile
#    COMPLEX  - all others, requiring a separate line in this makefile
#    SHELLS   - shell scripts.  See also the target .SUFFIXES and the
#		instructions for processing .csh and .sh suffixes.

SIMPLE	=
COMPLEX	=
SHELLS	=

# Libraries.  These are libraries made here, NOT the ones on which the stuff
# made here depends.  See the template in the end of this makefile for those
# others.  LIBBINS are executables that should go into the library - those are
# these binaries that people should invoke ONLY via shell scripts.  LIBINC are
# headers that should be installed in $(FULLINC). It is assumed that hey are
# generated here based on some other files; note that 'make veryclean' will
# REMOVE all $(LIBINC) stuff.  LIBCPY are all files that should be copied into
# the library directory. e.g., templates and such. If the stuff here depends on
# headers from $(FULLINC), their installation should be provided by the
# appropriate sequence of SUBDIR's in parent directories.

LIBS	=
LIBBINS	=
LIBINC	=
LIBCPY	=

# Manuals (usually those to be put in part 1 of the manual, perhaps also others)

MAN1	=
MAN3	=
MAN5	=
MAN8	=

# All programs to be made. It is useful to have this defined here, as one
# can make programs selectively by calling, e.g., 'make PROGRAMS=snafu'
# to make an executable snafu.

PROGRAMS	= $(SUBDIR) $(LIBS) $(SIMPLE) $(COMPLEX) $(SHELLS)
MANUALS		= $(MAN1) $(MAN3) $(MAN5) $(MAN8)

# All the stuff that is archived under RCS.

HEADERS		=
SOURCES		=
SHELLSRCS	=
ARCHIVED	= $(SOURCES) $(HEADERS) $(MANUALS) $(SHELLSRCS)

# Stuff that should be removed when making clean

JUNK		= *.o core a.out make.log $(LIBS) $(SIMPLE) $(COMPLEX)	       \
		  $(LIBBINS) $(SHELLS)

#*******************************************************************************
#
# Standard targets (each is decribed in more detail later on):
#   all		- all programs to be made.
#   .DEFAULT	- What to do if there are no instructions how to make a target.
#   ci		- Checks all what is necessary into RCS archive.
#   co		- Checks everything out from RCS archive.
#   clean	- Deletes object files, executables, core dumps and other junk.
#   veryclean	- As above, also makes checkin to save disk space.
#   install	- Installs everything in desired places.
#   dirs	- Makes directories in which to install the stuff
#   makefiles	- Update default parameters in the hierarchy of makefiles.
#   depend	- Generate the dependency lists for all C source programs.
#*******************************************************************************
# All targets that should be repeated in the subdirectories are listed here with
# an extension ".sub".  NOTE: we need to define .SUFFIXES so that these targets
# are properly recognized.  Suffixes include also suffixes for C-shell and for
# Bourne shell scripts and script templates.

SUB_TARGETS= ci.sub co.sub clean.sub makefiles.sub
.SUFFIXES: .sub .sh .SH .csh .CSH

#*******************************************************************************
# It is assumed here that all info required by the shell scripts are locations
# of the library and binary directories.  See shell templates for more info.

.CSH.csh:
	-tmp=/tmp/sh.$$$$;						       \
	sed -e "s,--LIBDIR--,$(FULLLIB)," < $*.CSH 			       \
	| sed -e "s,--BINDIR--,$(FULLBIN),"	 			       \
		> $$tmp;						       \
	mv $$tmp $*.csh; exit 0

.SH.sh:
	-tmp=/tmp/sh.$$$$;						       \
	sed -e "s,--LIBDIR--,$(FULLLIB)," < $*.SH 			       \
	| sed -e "s,--BINDIR--,$(FULLBIN),"	 			       \
		> $$tmp;						       \
	mv $$tmp $*.sh; exit 0

#*******************************************************************************
# All the stuff to be made.

all:	preinstall all.sub $(PROGRAMS) $(LIBCPY) $(LIBS) $(LIBBINS)

PREDIR	=

preinstall:
	@-dummy=/tmp;							       \
	predir=`pwd`;							       \
	pre=`pwd`/.preinstall.$$$$;					       \
	touch $$pre;							       \
	if [ "a$(PREINSTALL)" != "a" ]; then				       \
		dummy= ;						       \
		echo pre-installing libraries and headers before real make;    \
		for i in $(PREINSTALL) $$dummy; do			       \
			(cd $$i;echo doing `pwd`;			       \
			if make $(PASSDEFS) PREDIR=$$predir .preinstall;       \
			then exit 0;					       \
			else						       \
				echo failed preinstalling `pwd`;	       \
				/bin/rm -rf $$pre;			       \
			fi);						       \
		done;							       \
		if [ -f $$pre ]; then					       \
			mv $$pre .preinstall;				       \
			touch .preinstall;				       \
			for i in $(PREINSTALL) $$dummy; do		       \
				touch -c $$i/.preinstall 2>/dev/null;	       \
			done;						       \
		else exit 0;						       \
		fi;							       \
	else								       \
		/bin/rm -rf $$pre;					       \
		exit 0;							       \
	fi; exit 0;

.preinstall: $(PREDIR)/.preinstall
	@ if make $(PASSDEFS) install ; then touch .preinstall; else exit 0; fi

#*******************************************************************************
# Default target which will cause an attempt to checkout any missing files from
# an RCS archive.

.DEFAULT:
	@-if [ -f RCS/$<,v ] ;then co -q $<; else echo "Can not find $<"; fi
	@test -f $<

#*******************************************************************************
# RCS-related targets: checkout and checkin of everything.  Also makes sure that
# the archive directory exists. 

co: co.sub $(ARCHIVED) FRC
ci: ci.sub RCS	; @-ci $(ARCHIVED) ; ci -l makefile
RCS:	;mkdir RCS

#*******************************************************************************
# Cleanup all object files and other junk (e.g symbolic links to the library
# includes, core dumps, make logs ...

clean:	clean.sub
	-rm -f $(JUNK)
	@-sleep 1; touch -c .preinstall 2>/dev/null; exit 0;
veryclean: ci clean; -rm -f $(LIBINC)

#*******************************************************************************
# Subdirectories that need to be made for the stuff to be installed.

dirs:	$(ALLDIRS)
$(ALLDIRS):
	@-for i in $@ ; do						       \
		(case $$i in 						       \
			/*) cd / ;;					       \
		esac;							       \
		for j in `echo $$i | sed 's,/, ,g'`; do			       \
			if [ -d $$j ] ; then cd $$j;			       \
			else						       \
				/bin/mkdir $$j;				       \
				cd $$j;					       \
			fi;						       \
		done;							       \
	) done
	@for i in $@ ; do test -d $$i; done

#*******************************************************************************
# Install all the stuff.  The 'dummy' shell variable provides for a correct
# syntax if any of the lists to install are empty.

install: all dirs install.sub
	@-dummy=/tmp;							       \
	if [ "a$(SIMPLE)$(COMPLEX)" != "a" ]; then			       \
		dummy= ;						       \
		for i in $(SIMPLE) $(COMPLEX) $$dummy; do		       \
			install -s -c -m 0755 $$i $(FULLBIN);		       \
		done;							       \
	else								       \
		exit 0;							       \
	fi
	@-dummy=/tmp;							       \
	if [ "a$(SHELLS)" != "a" ]; then				       \
		dummy= ;						       \
		for i in $(SHELLS) $$dummy; do				       \
		    (if [ `basename $$i .csh` != `basename $$i` ] ;	       \
		    then						       \
			install -c -m 0755 $$i $(FULLBIN)/`basename $$i .csh`; \
		    else						       \
			install -c -m 0755 $$i $(FULLBIN)/`basename $$i .sh`;  \
		    fi; )						       \
		done;							       \
	else								       \
		exit 0;							       \
	fi
	@-dummy=/tmp;							       \
	if [ "a$(LIBBINS)" != "a" ]; then				       \
		dummy= ;						       \
		for i in $(LIBBINS) $$dummy; do				       \
			install -s -c -m 0755 $$i $(FULLLIB);		       \
		done;							       \
	else								       \
		exit 0;							       \
	fi
	@-dummy=/tmp;							       \
	if [ "a$(LIBS)" != "a" ]; then					       \
		dummy= ;						       \
		for i in $(LIBS) $$dummy; do				       \
		     (install -c -m 0644 $$i $(FULLLIB);ranlib $(FULLLIB)/$$i) \
		done;							       \
	else								       \
		exit 0;							       \
	fi
	@-dummy=/tmp;							       \
	if [ "a$(LIBINC)" != "a" ]; then				       \
		dummy= ;						       \
		for i in $(LIBINC) $$dummy; do				       \
			install -c -m 0644 $$i $(FULLINC);		       \
		done;							       \
	else								       \
		exit 0;							       \
	fi
	@-dummy=/tmp;							       \
	if [ "a$(LIBCPY)" != "a" ]; then				       \
		dummy= ;						       \
		for i in $(LIBCPY) $$dummy; do				       \
			install -c -m 0644 $$i $(FULLLIB);		       \
		done;							       \
	else								       \
		exit 0;							       \
	fi
	@-dummy=/tmp;							       \
	if [ "a$(MAN1)" != "a" ]; then					       \
		dummy= ;						       \
		for i in $(MAN1) $$dummy; do				       \
			install -c -m 0644 $$i $(FULLMAN)/man1;		       \
		done;							       \
	else								       \
		exit 0;							       \
	fi

#*******************************************************************************
# Update makefiles in the whole hierarchy

makefiles: makefiles.sub RCS
	@echo doing `pwd`; echo "Making RCS archive of the old $(MAKEFILE).";
	@-if echo "Before changing default definitions" | ci -q -l $(MAKEFILE);\
	then exit 0; else cp $(MAKEFILE) RCS/$(MAKEFILE).bak0.$$$$; fi
	@tmp=/tmp/sed.$$$$;						       \
	echo '/^BINDIR/s:=.*:= $(BINDIR):' > $$tmp;			       \
	echo '/^LIBDIR/s:=.*:= $(LIBDIR):' >> $$tmp;			       \
	echo '/^MANDIR/s:=.*:= $(MANDIR):' >> $$tmp;			       \
	echo '/^DESTDIR/s:=.*:= $(DESTDIR):' >> $$tmp;			       \
	echo '/^INCDIR/s:=.*:= $(INCDIR):' >> $$tmp;			       \
	echo w >>$$tmp; echo q >>$$tmp;					       \
	ed makefile < $$tmp; rm -f $$tmp;
	@echo "Making RCS archive of the new $(MAKEFILE).";
	@-if echo "After changing default definitions" | ci -q -l $(MAKEFILE);\
	then exit 0; else cp $(MAKEFILE) RCS/$(MAKEFILE).bak1.$$$$; fi

#*******************************************************************************
# This handles tasks that are to be repeated in each subdirectory.  The 'dummy'
# shell variable provides for a correct syntax if there are no subdirectories.

$(SUB_TARGETS):
	@-dummy=/tmp; trg=`basename $@ .sub`;				       \
	if [ "a$(SUBDIR)$(PREINSTALL)" != "a" ]; then			       \
		dummy= ;						       \
		for i in $(SUBDIR) $(PREINSTALL) $$dummy; do		       \
			(cd $$i;echo doing `pwd`;make $(PASSDEFS) $$trg);      \
		done;							       \
	else								       \
		exit 0;							       \
	fi

all.sub install.sub:
	@-dummy=/tmp; trg=`basename $@ .sub`;				       \
	if [ "a$(SUBDIR)" != "a" ]; then				       \
		dummy= ;						       \
		for i in $(SUBDIR) $$dummy; do				       \
			(cd $$i;echo doing `pwd`;make $(PASSDEFS) $$trg);      \
		done;							       \
	else								       \
		exit 0;							       \
	fi

# Dummy target to force some actions.

FRC:

# Need dev/null in case something is undefined

/dev/null $(SIMPLE):
	$(CC) $(CFLAGS) $@.c
	mv a.out $@

#*******************************************************************************
# Making dependency lists.  It is CRUCIAL that this makefile knows how to make
# the source and include files (e.g., should know how to check them out from the
# archive).  CFLAGS should contain appropriate -I options for include files
# residing in other directories, and these files MUST be already in place or
# "make depend" will fail.

forcedepend: FRC
	touch $(SOURCES)
	make depend

# DEPEND_BEGIN (do not change this line!)
#
#*******************************************************************************
# THIS WILL BE RE_DONE

.PRECIOUS: $(MAKEFILE)
depend: $(HEADERS) $(MAKEFILE)
$(MAKEFILE): $(SOURCES) $(DERIVED); @-					       \
	echo Re-creating dependencies in the makefile.  In case of failure;    \
	echo make sure that all non-standard include files are in place and;   \
	echo do \"make forcedepend\".; echo Archiving the old $(MAKEFILE)... ; \
	if echo "Before changing dependency lists" | ci -q -l $(MAKEFILE);     \
	then exit 0; else cp $(MAKEFILE) RCS/$(MAKEFILE).bak1.$$$$; fi
	@tmp=/tmp/dep.$$$$; tmp1=/tmp/dep1.$$$$; cat /dev/null > $$tmp;	       \
	for fi in $?; do						       \
	   bn=`basename $$fi .c`;					       \
	   (echo -n '/^# AUTOMATIC DEPENDENCIES/+2,/^# END OF DEPENDENCIES/-1';\
	    echo 'g/^'$$bn.o/d) >> $$tmp; done;				       \
	echo '/^# AUTOMATIC DEPENDENCIES/+2a' >> $$tmp;			       \
	for fi in $?; do						       \
	   bn=`basename $$fi .c`; echo $$fi > $$tmp1; cat /dev/null >$$tmp2;   \
	   echo Extracting dependencies for the file $$bn.o;		       \
	   cc -E $(CFLAGS) $$fi | grep '^#' | sed 's/.*[^"]*"\([^"]*\)".*/\1/' \
	      | sed 's/^\.\///' | sort -u > $$tmp1;			       \
	   for i in `cat $$tmp1`; do					       \
	      echo $$bn.o: $$i >> $$tmp; echo "   ..." depends on $$i; done    \
	   done;							       \
	(echo '.' ; echo 'g/.*\.o: /s,$(FULLINC),\$$(FULLINC),'; echo 'w';     \
	 echo 'q' ) >> $$tmp;						       \
	ed $(MAKEFILE) < $$tmp; rm -f $$tmp $$tmp1;
	@-echo "Making RCS archive of the new $(MAKEFILE)...";		       \
	if echo "After changing dependency lists" | ci -q -l $(MAKEFILE);      \
	then exit 0; else cp $(MAKEFILE) RCS/$(MAKEFILE).bak1.$$$$; fi
#
# DEPEND_END (do not change this line!)

# AUTOMATIC DEPENDENCIES. Do not change this line or ANYTHING until the line
# marked 'END OF DEPENDENCIES' below.  'make depend' won't work otherwise.


# END OF DEPENDENCIES. Do not change this line or ANYTHING above starting from
# the line marked 'AUTOMATIC DEPENDENCIES'.

# More specific makefile instructions and dependencies go here. 
# Put ALL your instructions here to facilitate automatic update of your
# makefile in case the makefile template gets expanded.

# BEGIN OF SPECIFICS. Do not change this line.

#vpnrhfix: $(FULLINC) $(FULLINC)/VPNR.h
#
#$(FULLINC)/VPNR.h: $(MAKEFILE)
#	@echo "#define VPNRSRC \"`pwd`\"" > $(FULLINC)/VPNR.h
#	@echo "#define VPNRBIN \"$(FULLBIN)\"" >>  $(FULLINC)/VPNR.h
#	@echo "#define VPNRLIB \"$(MAGICLIB)\"" >>  $(FULLINC)/VPNR.h

# END OF SPECIFICS. Do not change this line.

# FUTURE_BEGIN (do not change this line!)
#
### This stuff here is for the compatibility with the future versions of the
### makfile template.  Do not remove anything below.
#
# BETA_INSTALL_BEGIN (do not change this line!)
#
#*******************************************************************************
# Install beta-test version where necessary.
# THIS WILL BE RE_DONE
#rdirs:		; -for i in $(HOSTS); do rsh $$i mkdirr $(ALLDIRS); done
#rinstall: all rdirs rinstall.sub; @-					       \
#	for h in $(HOSTS); do						       \
#	   for i in $(BINS); do						       \
#	      rinstall -h $$h -s -c -m 0755 $$i $(FULLBIN);done;	       \
#	   for i in $(LIBS); do						       \
#	      rinstall -h $$h -c -m 0644 $$i $(FULLLIB); ranlib $(FULLLIB)/$$i;\
#	      done;							       \
#	   for i in $(MAN1); do						       \
#	     rinstall -h $$h -c -m 0644 $$i $(FULLMAN)/man1;done;	       \
#	done;
#
# BETA_INSTALL_END (do not change this line!)

# STABLE_INSTALL_BEGIN (do not change this line!)
#
#*******************************************************************************
# Notify the system administrator about the availability of a new version.
# THIS WILL BE RE_DONE
#
#archive: .,archiveupdate archive.sub
#.,archiveupdate: $(ARCHIVED) makefile; @-				       \
#	if [ \( $(SRCDIR) = $(ADIR) \) -a \( $(AHOST) = `hostname` \) ]; then \
#	echo Cannot execute archive - you are in the archive directory; exit 0;\
#	elif [ `pwd` = $(SRCDIR) ]; then curdir="";			       \
#	else curdir=`pwd|sed -e 's,$(SRCDIR)/,,'`; fi;			       \
#	rsh $(AHOST) mkdirr $(ADIR)/$$curdir/RCS;			       \
#	for i in $? ; do rinstall -h $(AHOST) -c -m 0644 $$i $(ADIR)/$$curdir; \
#	   done;							       \
#	rsh $(AHOST) cd $(ADIR)/$$curdir \; ci -q $?; touch .,archiveupdate
#
# STABLE_INSTALL_END (do not change this line!)
#
# FUTURE_END (do not change this line!)

#*******************************************************************************
# Examples of make instructions for various stuff
#
## library.a: necessary objects.o
##	ar ruv library.a necessary objects.o
#
# NOTE:
#	It is not necessary to supply instructions for making any objects
#	derived from C, yacc, or lex source code files (with extensions *.c
#	*.y and *.l).  It may be necessary, though, to put them in a dependency
#	list to provide the proper order of compilation.
#
# If some special flags are required for compiling a *.c file, here is a
# template:
#
## file.o: file.c maybe_some_other_stuff
##	cc -c $(CFLAGS) maybe_other_flags file.c
#
# NOTE: making executable files should ALWAYS follow the convention:
#
## program: all_objects
##	cc $(LDFLAGS) all_objects -L$(FULLLIB)
##	mv a.out program
#
# In the above template, bad link will NOT create a non-executable junk
# named 'program' that could be installed by mistake.
