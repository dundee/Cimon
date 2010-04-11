#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "log.h"
#include "config.h"
#include "memory.h"
#include "cpu.h"
#include "net.h"

#define STEP 60

pthread_t threads[3];

typedef struct {
	void (*func)(char *);
	char * datadir;
} thread_params_t;

static void *update_loop(void *arg)
{
	thread_params_t *params;
	
	params = (thread_params_t*) arg;
	
	while (1) {
		params->func(params->datadir);
		sleep(STEP);
	}
	return NULL;
}

int main(int argc, char** argv)
{
	thread_params_t params[3];
	int i;
	
	params[0].func    = memory_update_rrd;
	params[0].datadir = DATA_DIR;
	params[1].func    = cpu_update_rrd;
	params[1].datadir = DATA_DIR;
	params[2].func    = net_update_rrd;
	params[2].datadir = DATA_DIR;
	
	DEBUG("Starting thread for %s monitor\n", "memory");
	pthread_create(&threads[0], NULL, update_loop, (void *) &params[0]);
	DEBUG("Starting thread for %s monitor\n", "cpu");
	pthread_create(&threads[1], NULL, update_loop, (void *) &params[1]);
	DEBUG("Starting thread for %s monitor\n", "network");
	pthread_create(&threads[2], NULL, update_loop, (void *) &params[2]);
	
	for (i = 0; i < 3; i++) {
		pthread_join(threads[i], NULL);
	}
	
	return 0;
}