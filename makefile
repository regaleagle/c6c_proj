c5i: lex.yy.c y.tab.c c5i.c
	gcc -o c5i lex.yy.c y.tab.c c5i.c

c5c: lex.yy.c y.tab.c c5c.c
	gcc -o c5c lex.yy.c y.tab.c c5c.c

lex.yy.c: c5.l
	flex c5.l

y.tab.c: c5.y
	bison -y -d c5.y

sas: sas.c sas.tab.c
	gcc -o sas sas.c sas.tab.c

sas.c: sas.l
	flex -o sas.c sas.l

sas.tab.c: sas.y
	bison -d sas.y

nas: nas.c nas.tab.c
	gcc -o nas nas.c nas.tab.c

nas.c: nas.l
	flex -o nas.c nas.l

nas.tab.c: nas.y
	bison -d nas.y

cal: cal.c cal.tab.c
	gcc -o cal cal.c cal.tab.c

cal.c: cal.l
	flex -o cal.c cal.l

cal.tab.c: cal.y
	bison -d cal.y

calx: calx.c calx.tab.c
	gcc -o calx calx.c calx.tab.c

calx.c: calx.l
	flex -o calx.c calx.l

calx.tab.c: calx.y
	bison -d calx.y


clean:
	$(RM) lex.yy.c y.tab.c y.tab.h c5i c5c

cleansas:
	$(RM) sas.c sas.tab.c sas.tab.h sas

cleannas:
	$(RM) nas.c nas.tab.c nas.tab.h nas

cleancal:
	$(RM) cal.c cal.tab.c cal.tab.h cal calx.c calx.tab.c calx.tab.h calx
