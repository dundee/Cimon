CC		=	gcc
CFLAGS	=	-Wall -pedantic -g -std=gnu99
LDFLAGS	=	-lrrd_th

#`pkg-config --libs --cflags gtk+-2.0 libglade-2.0`

default: cimond cimon-graph cimon-web

cimond: cimond.o memory.o cpu.o net.o log.o rrd.o

cimon-graph: cimon-graph.o memory.o cpu.o net.o log.o rrd.o

cimon-web: cimon-web.o memory.o cpu.o net.o log.o rrd.o

%.o : %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f cimond *.o