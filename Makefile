CC		=	gcc
CFLAGS	=	-Wall -pedantic -g -std=gnu99
LDFLAGS	=	-lrrd_th
prefix	=	/usr
sbindir	=	/usr/sbin
srcdir	=	.
datadir	=	/var/lib/cimon

default: cimond cimon-graph cimon-web

cimond: $(srcdir)/cimond.o $(srcdir)/memory.o $(srcdir)/cpu.o $(srcdir)/net.o $(srcdir)/log.o $(srcdir)/rrd.o $(srcdir)/utils.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $?

cimon-graph: $(srcdir)/cimon-graph.o $(srcdir)/memory.o $(srcdir)/cpu.o $(srcdir)/net.o $(srcdir)/log.o $(srcdir)/rrd.o $(srcdir)/utils.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $?

cimon-web: $(srcdir)/cimon-web.o $(srcdir)/memory.o $(srcdir)/cpu.o $(srcdir)/net.o $(srcdir)/log.o $(srcdir)/rrd.o $(srcdir)/utils.o $(srcdir)/render-index.o $(srcdir)/cache.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $?

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f cimond cimon-graph cimon-web $(srcdir)/*.o
	
install:
	install -Dm744 cimond $(prefix)$(sbindir)/cimond
	install -Dm744 cimon-graph $(prefix)$(sbindir)/cimon-graph
	install -Dm744 cimon-web $(prefix)$(sbindir)/cimon-web
	install -d $(prefix)$(datadir)
