#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "rrd.h"
#include "utils.h"
#include "swap.h"

#define BUFF_SIZE 1024

void swap_create_rrd(char * datadir)
{
	const char * params[] = {
		"DS:free:GAUGE:180:0:U",
		"DS:used:GAUGE:180:0:U",
		"DS:total:GAUGE:180:0:U",
		"RRA:AVERAGE:0.5:1:2016", /* points */
		"RRA:AVERAGE:0.5:2016:1", /* real avg */
		"RRA:MAX:0.5:2016:1",
		"RRA:MIN:0.5:2016:1"
	};
	
	rrd_create_db(datadir, "swap.rrd", sizeof(params) / sizeof(char *), params);
}

void swap_update_rrd(char * datadir)
{
	swap_info_t *swap;
	char tmp[100];
	char * buff[1];
	
	if (!rrd_db_exists(datadir, "swap.rrd")) swap_create_rrd(datadir);
	
	swap = swap_get_values();

	sprintf(tmp, "N:%d:%d:%d", swap->free/1000, swap->used/1000, swap->total/1000);

	buff[0] = strdup(tmp);

	DEBUG("swap: %s\n", buff[0]);

	rrd_update_db(datadir, "swap.rrd", 1, buff);
	free(buff[0]);
	free(swap);
}

void swap_create_graph(char * datadir)
{
	char *rrd_path;
	char free_def[BUFF_SIZE];
	char used_def[BUFF_SIZE];
	char total_def[BUFF_SIZE];
	
	char * params[] = {
		"load",
		"./swap.png",
		"--imgformat", "PNG",
		"--title", "swap stats",
		"--vertical-label", "RAM",
		"--width", "700",
		"--height", "400",
		"--lower-limit", "0",

		"DEF:free=swap.rrd:free:AVERAGE",       /* 14 */
		"DEF:used=swap.rrd:used:AVERAGE",       /* 15 */
		"DEF:total=swap.rrd:total:AVERAGE",     /* 16 */

		"VDEF:freeavg=free,AVERAGE",
		"VDEF:freemax=free,MAXIMUM",
		"VDEF:freemin=free,MINIMUM",
		"VDEF:usedavg=used,AVERAGE",
		"VDEF:usedmax=used,MAXIMUM",
		"VDEF:usedmin=used,MINIMUM",
		"VDEF:totalavg=total,AVERAGE",
		"VDEF:totalmax=total,MAXIMUM",
		"VDEF:totalmin=total,MINIMUM",

		"COMMENT:             Average   Maximum   Minimum\\l",

		"AREA:used#F00:Used   :STACK",
		"GPRINT:usedavg:%7.2lfM",
		"GPRINT:usedmax:%7.2lfM",
		"GPRINT:usedmin:%7.2lfM\\l",

		"LINE2:total#000:Total  ",
		"GPRINT:totalavg:%7.2lfM",
		"GPRINT:totalmax:%7.2lfM",
		"GPRINT:totalmin:%7.2lfM"
	};
	
	rrd_path = compose_filename(datadir, "swap.rrd");
	
	params[14] = free_def;
	params[15] = used_def;
	params[16] = total_def;
	
	snprintf(params[14], BUFF_SIZE, "DEF:free=%s:free:AVERAGE", rrd_path);
	snprintf(params[15], BUFF_SIZE, "DEF:used=%s:used:AVERAGE", rrd_path);
	snprintf(params[16], BUFF_SIZE, "DEF:total=%s:total:AVERAGE", rrd_path);
	
	if (!rrd_db_exists(datadir, "swap.rrd")) swap_create_rrd(datadir);
	rrd_create_graph(datadir, "swap.png", sizeof(params) / sizeof(char *), params);
}

swap_info_t *swap_get_values()
{
	FILE *fp;
	swap_info_t *swap;
	char *buff, *tmp;
	
	buff = (char *) malloc(2048 * sizeof(char));
	
	swap = (swap_info_t *) malloc(sizeof(swap_info_t));
	memset(swap, 0, sizeof(swap_info_t));

	fp = fopen("/proc/meminfo", "r");

	if (fp == NULL) {
		WARNING("File %s doesn't exist or is not readable.\n", "/proc/meminfo");
		exit(1);
	}
	
	fread(buff, 1, 2048, fp);
	
	tmp = strstr(buff, "SwapTotal");
	
	do {
		if (tmp == NULL) { WARNING("Cannot parse %s.\n", "/proc/meminfo"); break; }
		if(sscanf(tmp, "SwapTotal: %u kB\n", &swap->total) == EOF) WARNING("Cannot parse %s.\n", "/proc/meminfo");
		tmp = strstr(buff, "SwapFree");
		if (tmp == NULL) { WARNING("Cannot parse %s.\n", "/proc/meminfo"); break; }
		if(sscanf(tmp, "SwapFree: %u kB\n", &swap->free) == EOF) WARNING("Cannot parse %s.\n", "/proc/meminfo");
		
		swap->used = swap->total - swap->free;
	} while (0);

	if(fclose(fp)) WARNING("Cannot close %s.\n", "/proc/meminfo");

	DEBUG("swap: total %d\n", swap->total);
	DEBUG("swap: used %d\n", swap->used);
	DEBUG("swap: free %d\n", swap->free);

	free(buff);

	return swap;
}
