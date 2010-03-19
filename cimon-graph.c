#include <stdio.h>

#include "memory.h"
#include "cpu.h"
#include "net.h"

int main(int argc, char** argv)
{
	memory_create_graph(".");
	cpu_create_graph(".");
	net_create_graph(".");

	return 0;
}