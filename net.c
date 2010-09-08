#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "rrd.h"
#include "utils.h"
#include "net.h"

#define BUFF_SIZE 1024

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
	net_info_t *net;
	char * buff[1];
	char tmp[100];

	if (!rrd_db_exists(datadir, "net.rrd")) net_create_rrd(datadir);

	net = net_get_values();
	sprintf(tmp, "N:%d:%d", net->download, net->upload);
	buff[0] = strdup(tmp);

	DEBUG("net: %s\n", buff[0]);

	rrd_update_db(datadir, "net.rrd", 1, buff);
	free(buff[0]);
	free(net);
}

void net_create_graph(char * datadir)
{
	char *rrd_path;
	char download_def[BUFF_SIZE];
	char upload_def[BUFF_SIZE];
	
	char * params[] = {
		"load",
		"net.png",
		"--imgformat", "PNG",
		"--title", "Network stats",
		"--vertical-label", "bytes/sec",
		"--width", "700",
		"--height", "400",
		"--lower-limit", "0",

		"DEF:download=net.rrd:download:AVERAGE", /* 14 */
		"DEF:upload=net.rrd:upload:AVERAGE",     /* 15 */

		"LINE:download#0F0:Download",
		"GPRINT:download:MAX:Max download\\: %2.2lf bytes/sec",

		"LINE:upload#00F:Upload",
		"GPRINT:upload:MAX:Max upload\\: %2.2lf bytes/sec",
	};
	
	rrd_path = compose_filename(datadir, "net.rrd");
	
	params[14] = download_def;
	params[15] = upload_def;
	
	snprintf(params[14], BUFF_SIZE, "DEF:download=%s:download:AVERAGE", rrd_path);
	snprintf(params[15], BUFF_SIZE, "DEF:upload=%s:upload:AVERAGE", rrd_path);
	
	if (!rrd_db_exists(datadir, "net.rrd")) net_create_rrd(datadir);
	rrd_create_graph(datadir, "net.png", sizeof(params) / sizeof(char *), params);
	free(rrd_path);
}

net_info_t *net_get_values()
{
	FILE * fp;
	net_info_t *net;
	int i;
	unsigned var;
	
	net = (net_info_t *) malloc(sizeof(net_info_t));
	memset(net, 0, sizeof(net_info_t));
	
	fp = fopen("/proc/net/dev", "r");
	
	if (fp == NULL) {
		WARNING("File %s doesn't exist or is not readable.\n", "/proc/net/dev");
		exit(1);
	}

	if(fscanf(fp, "%*[^:]") == EOF) WARNING("Cannot parse %s.\n", "/proc/net/dev");
	if(fscanf(fp, ": %*[^:]") == EOF) WARNING("Cannot parse %s.\n", "/proc/net/dev");
	if(!fscanf(fp, ": %u", &net->download)) WARNING("Cannot parse %s.\n", "/proc/net/dev");

	for (i=0; i < 7; ++i) {
		if(!fscanf(fp, " %u ", &var)) WARNING("Cannot parse %s.\n", "/proc/net/dev");;
	}
	if(!fscanf(fp, "%u", &net->upload)) WARNING("Cannot parse %s.\n", "/proc/net/dev");

	if(fclose(fp)) WARNING("Cannot close %s.\n", "/proc/net/dev");
	
	DEBUG("net: download %f\n", net->download);
	DEBUG("net: upload %d\n", net->upload);
	
	return net;
}
