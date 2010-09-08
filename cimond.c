#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "http_server.h"
#include "log.h"
#include "config.h"
#include "memory.h"
#include "cpu.h"
#include "net.h"
#include "swap.h"

#define STEP 60

pthread_t threads[6];

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

static void *graph_loop(void *arg)
{
	while (1) {
		DEBUG("Creating graph for %s monitor\n", "memory");
		memory_create_graph(DATA_DIR);
		DEBUG("Creating graph for %s monitor\n", "cpu");
		cpu_create_graph(DATA_DIR);
		DEBUG("Creating graph for %s monitor\n", "network");
		net_create_graph(DATA_DIR);
		DEBUG("Creating graph for %s monitor\n", "swap");
		swap_create_graph(DATA_DIR);
		
		sleep(REFRESH_GRAPH_INTERVAL);
	}
	return NULL;
}

static void *web_thread(void *arg)
{
	http_server_run(PORT);
	return NULL;
}

int main(int argc, char** argv)
{
	thread_params_t params[4];
	int i;
	
	params[0].func    = memory_update_rrd;
	params[0].datadir = DATA_DIR;
	params[1].func    = cpu_update_rrd;
	params[1].datadir = DATA_DIR;
	params[2].func    = net_update_rrd;
	params[2].datadir = DATA_DIR;
	params[3].func    = swap_update_rrd;
	params[3].datadir = DATA_DIR;
	
	DEBUG("Starting thread for %s monitor\n", "memory");
	if(pthread_create(&threads[0], NULL, update_loop, (void *) &params[0])) ERROR("Thread %d cannot be started\n", 0);
	DEBUG("Starting thread for %s monitor\n", "cpu");
	if(pthread_create(&threads[1], NULL, update_loop, (void *) &params[1])) ERROR("Thread %d cannot be started\n", 1);
	DEBUG("Starting thread for %s monitor\n", "network");
	if(pthread_create(&threads[2], NULL, update_loop, (void *) &params[2])) ERROR("Thread %d cannot be started\n", 2);
	DEBUG("Starting thread for %s monitor\n", "swap");
	if(pthread_create(&threads[2], NULL, update_loop, (void *) &params[3])) ERROR("Thread %d cannot be started\n", 3);
	DEBUG("Starting thread for %s refreshing\n", "graph");
	if(pthread_create(&threads[4], NULL, graph_loop, NULL)) ERROR("Thread %d cannot be started\n", 4);
	
	#ifdef START_WEBSERVER
	DEBUG("Starting thread for %s server\n", "web");
	if(pthread_create(&threads[5], NULL, web_thread, NULL)) ERROR("Thread %d cannot be started\n", 5);
	#endif
	
	for (i = 0; i < 6; i++) {
		pthread_join(threads[i], NULL);
	}
	
	return 0;
}
