OBJECTS = 2edif.o 2twof.o 2yal.o lx.o main.o parser.o parser_fun.o	\
	  error.o uniq_name.o hash.o stracc.o 2generic.o
CFLAGS	= -g

v2: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -ll
	mv a.out v2

clean:
	rm -f $(OBJECTS) a.out v2 lx.c parser.c parser.h lex.yy.c \
	y.tab.c y.tab.h

lx.c: lx.l
	lex -v lx.l
	mv lex.yy.c lx.c

parser.c parser.h: parser.y
	yacc -d parser.y
	mv y.tab.c parser.c
	mv y.tab.h parser.h

###
2edif.o: /usr/include/ctype.h
2edif.o: /usr/include/stdio.h
2edif.o: /usr/include/stdio.h
2edif.o: /usr/include/sys/types.h
2edif.o: 2edif.c
2edif.o: custom.h
2edif.o: oasisutils.h
2edif.o: parser.h
2edif.o: parser_proto.h
2edif.o: unixproto.h
2generic.o: /usr/include/ctype.h
2generic.o: /usr/include/stdio.h
2generic.o: /usr/include/stdio.h
2generic.o: /usr/include/sys/types.h
2generic.o: 2generic.c
2generic.o: custom.h
2generic.o: oasisutils.h
2generic.o: parser.h
2generic.o: parser_proto.h
2generic.o: unixproto.h
2twof.o: /usr/include/ctype.h
2twof.o: /usr/include/stdio.h
2twof.o: /usr/include/stdio.h
2twof.o: /usr/include/sys/types.h
2twof.o: 2twof.c
2twof.o: custom.h
2twof.o: oasisutils.h
2twof.o: parser.h
2twof.o: parser_proto.h
2twof.o: unixproto.h
2yal.o: /usr/include/ctype.h
2yal.o: /usr/include/stdio.h
2yal.o: /usr/include/stdio.h
2yal.o: /usr/include/sys/types.h
2yal.o: 2yal.c
2yal.o: custom.h
2yal.o: oasisutils.h
2yal.o: parser.h
2yal.o: parser_proto.h
2yal.o: unixproto.h
error.o: /usr/include/ctype.h
error.o: /usr/include/stdio.h
error.o: /usr/include/stdio.h
error.o: /usr/include/sys/resource.h
error.o: /usr/include/sys/time.h
error.o: /usr/include/sys/types.h
error.o: error.c
error.o: oasisutils.h
error.o: unixproto.h
hash.o: /usr/include/ctype.h
hash.o: /usr/include/stdio.h
hash.o: /usr/include/sys/types.h
hash.o: hash.c
hash.o: oasisutils.h
lx.o: /usr/include/ctype.h
lx.o: /usr/include/stdio.h
lx.o: /usr/include/stdio.h
lx.o: /usr/include/sys/types.h
lx.o: lx.c
lx.o: oasisutils.h
lx.o: parser.h
lx.o: unixproto.h
main.o: /usr/include/ctype.h
main.o: /usr/include/stdio.h
main.o: /usr/include/stdio.h
main.o: /usr/include/sys/types.h
main.o: custom.h
main.o: main.c
main.o: oasisutils.h
main.o: parser.h
main.o: parser_proto.h
main.o: unixproto.h
parser.o: /usr/include/ctype.h
parser.o: /usr/include/ctype.h
parser.o: /usr/include/stdio.h
parser.o: /usr/include/stdio.h
parser.o: /usr/include/sys/types.h
parser.o: custom.h
parser.o: lint_fix.h
parser.o: oasisutils.h
parser.o: parser.c
parser.o: parser_proto.h
parser.o: unixproto.h
parser_fun.o: /usr/include/ctype.h
parser_fun.o: /usr/include/stdio.h
parser_fun.o: /usr/include/stdio.h
parser_fun.o: /usr/include/sys/types.h
parser_fun.o: custom.h
parser_fun.o: oasisutils.h
parser_fun.o: parser.h
parser_fun.o: parser_fun.c
parser_fun.o: parser_proto.h
parser_fun.o: unixproto.h
stracc.o: /usr/include/ctype.h
stracc.o: /usr/include/stdio.h
stracc.o: /usr/include/sys/types.h
stracc.o: oasisutils.h
stracc.o: stracc.c
stracc.o: unixproto.h
uniq_name.o: /usr/include/ctype.h
uniq_name.o: /usr/include/stdio.h
uniq_name.o: /usr/include/sys/types.h
uniq_name.o: oasisutils.h
uniq_name.o: uniq_name.c
uniq_name.o: unixproto.h
