#ifndef _CIMON_LOG_H
#define	_CIMON_LOG_H

/*#define DEBUG_MODE 1*/

typedef enum {
	DEBUG,
	WARNING
} MESSAGES;

#if DEBUG_MODE
# define DEBUG(msg, ...)  (void) mlog (msg, __VA_ARGS__)
#else
# define DEBUG(msg, ...)  /* nope */
#endif

#define WARNING(msg, ...) mlog (msg, __VA_ARGS__)

int mlog (char *msg, ...);

#endif