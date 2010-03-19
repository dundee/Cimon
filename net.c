#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "rrd.h"
#include "net.h"

void net_create_rrd(char * datadir)
{
	const char * params[] = {
		"DS:download:COUNTER:180:0:U",
		"DS:upload:COUNTER:180:0:U",

		"RRA:AVERAGE:0.5:5:1440",
		"RRA:MAX:0.5:1440:1"
	};

	rrd_create_db(datadir, "net.rrd", sizeof(params) / sizeof(char *), params);
}

void net_update_rrd(char * datadir)
{
	net_info_t net;
	char * buff[1];
	char tmp[100];

	if (!rrd_db_exists(datadir, "net.rrd")) net_create_rrd(datadir);

	net = net_get_values();
	sprintf(tmp, "N:%d:%d", net.download, net.upload);
	buff[0] = strdup(tmp);

	DEBUG("net: %s\n", buff[0]);

	rrd_update_db(datadir, "net.rrd", 1, buff);
	free(buff[0]);
}

void net_create_graph(char * datadir)
{
	char * params[] = {
		"load",
		"net.png",
		"--imgformat", "PNG",
		"--title", "Network stats",
		"--vertical-label", "bytes/sec",
		"--width", "700",
		"--height", "400",
		"--lower-limit", "0",

		"DEF:download=net.rrd:download:AVERAGE",
		"DEF:upload=net.rrd:upload:AVERAGE",

		"LINE:download#0F0:Download",
		"GPRINT:download:MAX:Max download\\: %2.2lf bytes/sec",

		"LINE:upload#00F:Upload",
		"GPRINT:upload:MAX:Max upload\\: %2.2lf bytes/sec",
	};
	if (!rrd_db_exists(datadir, "net.rrd")) net_create_rrd(datadir);
	rrd_create_graph(sizeof(params) / sizeof(char *), params);
}

net_info_t net_get_values()
{
	FILE * fp;
	net_info_t net;
	int i;
	unsigned var;
	
	fp = fopen("/proc/net/dev", "r");

	fscanf(fp, "%*[^:]");
	fscanf(fp, ": %*[^:]");
	fscanf(fp, ": %u", &net.download);

	for (i=0; i < 7; ++i) {
		fscanf(fp, " %u ", &var);
	}
	fscanf(fp, "%u", &net.upload);

	fclose(fp);
	
	DEBUG("cpu: download %f\n", net.download);
	DEBUG("net: upload %d\n", net.upload);
	
	return net;
}
