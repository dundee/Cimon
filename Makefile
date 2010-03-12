CC		=	gcc
CFLAGS	=	-Wall -pedantic -g
LDFLAGS	=	-lrrd_th

#`pkg-config --libs --cflags gtk+-2.0 libglade-2.0`

default: cimon

cimon: cimon.o memory.o cpu.o net.o log.o rrd.o

%.o : %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f cimon *.o