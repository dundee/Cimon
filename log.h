#ifndef _CIMON_LOG_H
#define	_CIMON_LOG_H

#include "config.h"

typedef enum {
	DEBUG,
	WARNING
} MESSAGES;

#ifdef DEBUG_MODE
# define DEBUG(msg, ...)  print_warning (msg, __VA_ARGS__)
#else
# define DEBUG(msg, ...)  /* nope */
#endif

#define WARNING(msg, ...) print_warning (msg, __VA_ARGS__)
#define ERROR(msg, ...)  print_error (msg, __VA_ARGS__)

void print_warning (char *msg, ...);
void print_error (char *msg, ...);

#endif