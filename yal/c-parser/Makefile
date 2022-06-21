CFLAGS=-g
C_INCLUDE_PATH=./cJSON/


binaries=ptest yal2json

.PHONY: clean

# YAL to JSON obejct
yal2json: yal2json.o getmodule.o writemodule.o cJSON.o
	${CC} ${CFLAGS} -o yal2json yal2json.o getmodule.o cJSON.o

# YAL to JSON obejct
yal2json.o: yal2json.c benchtypes.h
	${CC} -std=c89 ${CFLAGS} -c yal2json.c

# cJSON object
cJSON.o: cJSON/cJSON.c cJSON/cJSON.h
	${CC} ${CFLAGS} -c cJSON/cJSON.c

# original parser test program
ptest: ptest.o getmodule.o writemodule.o
	${CC} ${CFLAGS} -o ptest ptest.o getmodule.o writemodule.o 

ptest.o: ptest.c benchtypes.h
	${CC} -std=c89 ${CFLAGS} -c ptest.c

# YAL parser object
getmodule.o: getmodule.c benchtypes.h
	${CC} -std=c89 ${CFLAGS} -c getmodule.c

# YAL writer object
writemodule.o: writemodule.c benchtypes.h
	${CC} -std=c89 ${CFLAGS} -c writemodule.c


clean:
	rm -f $(binaries) *.o
