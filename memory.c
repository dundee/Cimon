#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "rrd.h"
#include "utils.h"
#include "memory.h"

#define BUFF_SIZE 1024

void memory_create_rrd(char * datadir)
{
	const char * params[] = {
		"DS:used:GAUGE:180:0:U",
		"DS:cached:GAUGE:180:0:U",
		"DS:buffers:GAUGE:180:0:U",
		"DS:free:GAUGE:180:0:U",
		"DS:total:GAUGE:180:0:U",
		"RRA:AVERAGE:0.5:1:2016", /* points */
		"RRA:AVERAGE:0.5:2016:1", /* real avg */
		"RRA:MAX:0.5:2016:1",
		"RRA:MIN:0.5:2016:1"
	};
	
	rrd_create_db(datadir, "memory.rrd", sizeof(params) / sizeof(char *), params);
}

void memory_update_rrd(char * datadir)
{
	memory_info_t *memory;
	char tmp[100];
	char * buff[1];
	
	if (!rrd_db_exists(datadir, "memory.rrd")) memory_create_rrd(datadir);
	
	memory = memory_get_values();

	sprintf(tmp, "N:%d:%d:%d:%d:%d", memory->used/1000, memory->cached/1000, memory->buffers/1000, memory->free/1000, memory->total/1000);

	buff[0] = strdup(tmp);

	DEBUG("memory: %s\n", buff[0]);

	rrd_update_db(datadir, "memory.rrd", 1, buff);
	free(buff[0]);
	free(memory);
}

void memory_create_graph(char * datadir)
{
	char *rrd_path;
	char used_def[BUFF_SIZE];
	char cached_def[BUFF_SIZE];
	char buffers_def[BUFF_SIZE];
	char free_def[BUFF_SIZE];
	char total_def[BUFF_SIZE];
	
	char * params[] = {
		"load",
		"./memory.png",
		"--imgformat", "PNG",
		"--title", "Memory stats",
		"--vertical-label", "RAM",
		"--width", "700",
		"--height", "400",
		"--lower-limit", "0",

		"DEF:used=memory.rrd:used:AVERAGE",       /* 14 */
		"DEF:cached=memory.rrd:cached:AVERAGE",   /* 15 */
		"DEF:buffers=memory.rrd:buffers:AVERAGE", /* 16 */
		"DEF:free=memory.rrd:free:AVERAGE",       /* 17 */
		"DEF:total=memory.rrd:total:AVERAGE",     /* 18 */

		"VDEF:usedavg=used,AVERAGE",
		"VDEF:usedmax=used,MAXIMUM",
		"VDEF:usedmin=used,MINIMUM",
		"VDEF:cachedavg=cached,AVERAGE",
		"VDEF:cachedmax=cached,MAXIMUM",
		"VDEF:cachedmin=cached,MINIMUM",
		"VDEF:buffersavg=buffers,AVERAGE",
		"VDEF:buffersmax=buffers,MAXIMUM",
		"VDEF:buffersmin=buffers,MINIMUM",
		"VDEF:freeavg=free,AVERAGE",
		"VDEF:freemax=free,MAXIMUM",
		"VDEF:freemin=free,MINIMUM",
		"VDEF:totalavg=total,AVERAGE",
		"VDEF:totalmax=total,MAXIMUM",
		"VDEF:totalmin=total,MINIMUM",

		"COMMENT:             Average   Maximum   Minimum\\l",

		"AREA:used#F00:Used   ",
		"GPRINT:usedavg:%7.2lfM",
		"GPRINT:usedmax:%7.2lfM",
		"GPRINT:usedmin:%7.2lfM\\l",

		"AREA:cached#00F:Cached :STACK",
		"GPRINT:cachedavg:%7.2lfM",
		"GPRINT:cachedmax:%7.2lfM",
		"GPRINT:cachedmin:%7.2lfM\\l",

		"AREA:buffers#0F0:Buffers:STACK",
		"GPRINT:buffersavg:%7.2lfM",
		"GPRINT:buffersmax:%7.2lfM",
		"GPRINT:buffersmin:%7.2lfM\\l",

		"AREA:free#555:Free   :STACK",
		"GPRINT:freeavg:%7.2lfM",
		"GPRINT:freemax:%7.2lfM",
		"GPRINT:freemin:%7.2lfM\\l",

		"LINE2:total#000:Total  ",
		"GPRINT:totalavg:%7.2lfM",
		"GPRINT:totalmax:%7.2lfM",
		"GPRINT:totalmin:%7.2lfM"
	};
	
	rrd_path = compose_filename(datadir, "memory.rrd");
	
	params[14] = used_def;
	params[15] = cached_def;
	params[16] = buffers_def;
	params[17] = free_def;
	params[18] = total_def;
	
	snprintf(params[14], BUFF_SIZE, "DEF:used=%s:used:AVERAGE", rrd_path);
	snprintf(params[15], BUFF_SIZE, "DEF:cached=%s:cached:AVERAGE", rrd_path);
	snprintf(params[16], BUFF_SIZE, "DEF:buffers=%s:buffers:AVERAGE", rrd_path);
	snprintf(params[17], BUFF_SIZE, "DEF:free=%s:free:AVERAGE", rrd_path);
	snprintf(params[18], BUFF_SIZE, "DEF:total=%s:total:AVERAGE", rrd_path);
	
	if (!rrd_db_exists(datadir, "memory.rrd")) memory_create_rrd(datadir);
	rrd_create_graph(datadir, "memory.png", sizeof(params) / sizeof(char *), params);
}

memory_info_t *memory_get_values()
{
	FILE *fp;
	memory_info_t *memory;
	
	memory = (memory_info_t *) malloc(sizeof(memory_info_t));
	memset(memory, 0, sizeof(memory_info_t));

	fp = fopen("/proc/meminfo", "r");

	if (fp == NULL) {
		WARNING("File %s doesn't exist or is not readable.\n", "/proc/meminfo");
		exit(1);
	}

	if(!fscanf(fp, "MemTotal: %u kB\n", &memory->total)) WARNING("Cannot parse %s.\n", "/proc/meminfo");
	if(!fscanf(fp, "MemFree: %u kB\n", &memory->free)) WARNING("Cannot parse %s.\n", "/proc/meminfo");
	if(!fscanf(fp, "Buffers: %u kB\n", &memory->buffers)) WARNING("Cannot parse %s.\n", "/proc/meminfo");
	if(!fscanf(fp, "Cached: %u kB\n", &memory->cached)) WARNING("Cannot parse %s.\n", "/proc/meminfo");
	
	memory->used = memory->total - memory->free - memory->buffers - memory->cached;

	if(fclose(fp)) WARNING("Cannot close %s.\n", "/proc/meminfo");

	DEBUG("memory: total %d\n", memory->total);
	DEBUG("memory: free %d\n", memory->free);
	DEBUG("memory: buffers %d\n", memory->buffers);
	DEBUG("memory: cached %d\n", memory->cached);
	DEBUG("memory: total %d\n", memory->used);

	return memory;
}