CFLAGS=-g


ptest: ptest.o getmodule.o writemodule.o
	${CC} ${CFLAGS} -o ptest ptest.o getmodule.o writemodule.o 

ptest.o: ptest.c benchtypes.h
	${CC} -std=c89 ${CFLAGS} -c ptest.c

getmodule.o: getmodule.c benchtypes.h
	${CC} -std=c89 ${CFLAGS} -c getmodule.c

writemodule.o: writemodule.c benchtypes.h
	${CC} -std=c89 ${CFLAGS} -c writemodule.c

binaries=ptest

.PHONY: clean

clean:
	rm -f $(binaries) *.o
