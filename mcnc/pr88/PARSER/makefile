CFLAGS=-g 

ptest: ptest.o getmodule.o writemodule.o 
	cc ${CFLAGS} -o ptest ptest.o getmodule.o writemodule.o 

ptest.o: ptest.c benchtypes.h
	cc ${CFLAGS} -c ptest.c

getmodule.o: getmodule.c benchtypes.h
	cc ${CFLAGS} -c getmodule.c

writemodule.o: writemodule.c benchtypes.h
	cc ${CFLAGS} -c writemodule.c
