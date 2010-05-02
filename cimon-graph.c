#include <stdio.h>
#include <unistd.h>

#include "log.h"
#include "config.h"
#include "memory.h"
#include "cpu.h"
#include "net.h"

int main(int argc, char** argv)
{
	while (1) {
		DEBUG("Creating graph for %s monitor\n", "memory");
		memory_create_graph(DATA_DIR);
		DEBUG("Creating graph for %s monitor\n", "cpu");
		cpu_create_graph(DATA_DIR);
		DEBUG("Creating graph for %s monitor\n", "network");
		net_create_graph(DATA_DIR);
		
		sleep(REFRESH_GRAPH_INTERVAL);
	}

	return 0;
}