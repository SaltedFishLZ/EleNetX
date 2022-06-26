#OASIS      = /cad/oasis/new/src
#MCNCLIB    = ${OASIS}/oasisutil/OBJECTS.SUN4.SunOS.41/liboasis.a
# -lPW
#MCNCFLAGS  = -DMCNC -I${OASIS}/oasisutil/OBJECTS.SUN4.SunOS.41 \
#	     -DSUN4 -DSunOS -DOSV=41
# -A 2 -DmacII -DSYSV

SOURCES = main.c xlatr.c vpnr.c yal.c
YSOURCE = x_yacc.c
LSOURCE = x_lex.c
INCLUDES= non_mcnc.h typedef.h xlatr.h x_yacc.h

YOBJECT= ${YSOURCE:.c=.o}
LOBJECT= ${LSOURCE:.c=.o}
OBJECTS= ${YOBJECT} ${LOBJECT} ${SOURCES:.c=.o}

PROGRAM= xlatr

LIBS= $(MCNCLIB) -ll -ly

CFLAGS=  -O $(MCNCFLAGS)

CC= cc

VARIANTS = ${PROGRAM}2vpnr ${PROGRAM}2yal

all: ${PROGRAM} ${VARIANTS}

${VARIANTS}:
	ln -s ${PROGRAM} $@

${PROGRAM}: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} ${LIBS}
	mv a.out ${PROGRAM}

${OBJECTS}: ${INCLUDES}

${LSOURCE}: x_lex.l
	lex x_lex.l; mv lex.yy.c ${LSOURCE}

${YSOURCE} x_yacc.h : x_yacc.y
	yacc -d x_yacc.y; mv y.tab.c ${YSOURCE};  mv y.tab.h x_yacc.h

clean:
	rm -f ${OBJECTS} ${PROGRAM} ${LSOURCE} ${YSOURCE} a.out core \
		${VARIANTS} x_yacc.h
