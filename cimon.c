#include <stdio.h>

#include "memory.h"
#include "cpu.h"
#include "net.h"

int main(int argc, char** argv)
{
	memory_update_rrd(".");
	memory_create_graph(".");
	
	cpu_update_rrd(".");
	cpu_create_graph(".");
	
	net_update_rrd(".");
	net_create_graph(".");
	
	
	return 0;
}
