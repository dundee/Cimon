#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "config.h"
#include "config_reader.h"
#include "http_server.h"
#include "log.h"
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
	configuration_t *config = (configuration_t *) arg;
	
	while (1) {
		DEBUG("Creating graph for %s monitor\n", "memory");
		memory_create_graph(config->data_dir);
		DEBUG("Creating graph for %s monitor\n", "cpu");
		cpu_create_graph(config->data_dir);
		DEBUG("Creating graph for %s monitor\n", "network");
		net_create_graph(config->data_dir);
		DEBUG("Creating graph for %s monitor\n", "swap");
		swap_create_graph(config->data_dir);
		
		sleep(config->graph_refresh_interval);
	}
	return NULL;
}

static void *web_thread(void *arg)
{
	configuration_t *config = (configuration_t *) arg;
	
	http_server_run(config->port);
	return NULL;
}

int main(int argc, char** argv)
{
	thread_params_t update_params[4];
	int i;
	configuration_t *config;
	
	if (!read_config(CONFIG_FILE)) {
		WARNING("Configuration file %s is broken\n", CONFIG_FILE);
		exit(1);
	}
	
	config = get_config();
	
	update_params[0].func    = memory_update_rrd;
	update_params[0].datadir = config->data_dir;
	update_params[1].func    = cpu_update_rrd;
	update_params[1].datadir = config->data_dir;
	update_params[2].func    = net_update_rrd;
	update_params[2].datadir = config->data_dir;
	update_params[3].func    = swap_update_rrd;
	update_params[3].datadir = config->data_dir;
	
	DEBUG("Starting thread for %s monitor\n", "memory");
	if(pthread_create(&threads[0], NULL, update_loop, (void *) &update_params[0])) ERROR("Thread %d cannot be started\n", 0);
	DEBUG("Starting thread for %s monitor\n", "cpu");
	if(pthread_create(&threads[1], NULL, update_loop, (void *) &update_params[1])) ERROR("Thread %d cannot be started\n", 1);
	DEBUG("Starting thread for %s monitor\n", "network");
	if(pthread_create(&threads[2], NULL, update_loop, (void *) &update_params[2])) ERROR("Thread %d cannot be started\n", 2);
	DEBUG("Starting thread for %s monitor\n", "swap");
	if(pthread_create(&threads[2], NULL, update_loop, (void *) &update_params[3])) ERROR("Thread %d cannot be started\n", 3);
	DEBUG("Starting thread for %s refreshing\n", "graph");
	if(pthread_create(&threads[4], NULL, graph_loop, (void *) config)) ERROR("Thread %d cannot be started\n", 4);
	
	if (config->start_webserver) {
		DEBUG("Starting thread for %s server\n", "web");
		if(pthread_create(&threads[5], NULL, web_thread, (void *) config)) ERROR("Thread %d cannot be started\n", 5);
	}
	
	for (i = 0; i < 6; i++) {
		pthread_join(threads[i], NULL);
	}
	
	return 0;
}
