OBJECTS = main.o y2v.o y2v_yacc.o
CFLAGS	= -g

yal2vpnr: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -ll
	mv a.out yal2vpnr

clean:
	rm -f $(OBJECTS) a.out yal2vpnr y2v.c y2v_yacc.c y2v_yacc.h lex.yy.c \
	y.tab.c y.tab.h

y2v.c: y2v.l
	lex -v y2v.l
	mv lex.yy.c y2v.c

y2v_yacc.c y2v_yacc.h: y2v_yacc.y
	yacc -d y2v_yacc.y
	mv y.tab.c y2v_yacc.c
	mv y.tab.h y2v_yacc.h

###
main.o: main.c
main.o: /usr/include/stdio.h
main.o: /usr/include/strings.h
main.o: y2v.h
y2v_yacc.o: y2v_yacc.c
y2v_yacc.o: /usr/include/stdio.h
y2v_yacc.o: /usr/include/strings.h
y2v_yacc.o: y2v.h
y2v.o: y2v.c
y2v.o: /usr/include/stdio.h
y2v.o: /usr/include/stdio.h
y2v.o: y2v_yacc.h
