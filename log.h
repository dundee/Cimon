#ifndef _CIMON_LOG_H
#define	_CIMON_LOG_H

typedef enum {
	DEBUG,
	WARNING
} MESSAGES;

#if DEBUG_MODE
# define DEBUG(msg, ...)  mlog (msg, __VA_ARGS__)
#else
# define DEBUG(msg, ...)  /* nope */
#endif

#define WARNING(msg, ...) mlog (msg, __VA_ARGS__)

#endif

void mlog (char *msg, ...);