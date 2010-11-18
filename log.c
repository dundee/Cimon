#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

void print_warning (char *msg, ...) {
	va_list ap;
	
	va_start (ap, msg);
	vfprintf (stderr, msg, ap);
	va_end (ap);
	return;
}

void print_error (char *msg, ...) {
	va_list ap;
	
	va_start (ap, msg);
	vfprintf (stderr, msg, ap);
	va_end (ap);
	fprintf(stderr, "Fatal error, exiting...\n");
	exit(1);
}
