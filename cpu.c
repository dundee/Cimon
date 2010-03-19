#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "rrd.h"
#include "cpu.h"

void cpu_create_rrd(char * datadir)
{
	const char * params[] = {
		"DS:load:GAUGE:180:0:U",
		"DS:user:COUNTER:180:0:U",
		"DS:nice:COUNTER:180:0:U",
		"DS:system:COUNTER:180:0:U", /*heartbeat:min:max*/

		"RRA:AVERAGE:0.5:1:1440",   /*instances*/
		"RRA:AVERAGE:0.5:1440:1",   /*real avg*/
		"RRA:MIN:0.5:1440:1",
		"RRA:MAX:0.5:1440:1"   /*xff:steps:rows*/
	};

	rrd_create_db(datadir, "cpu.rrd", sizeof(params) / sizeof(char *), params);
}

void cpu_update_rrd(char * datadir)
{
	cpu_info_t cpu;
	char * buff[1];
	char tmp[100];
	
	if (!rrd_db_exists(datadir, "cpu.rrd")) cpu_create_rrd(datadir);

	cpu = cpu_get_values();
	sprintf(tmp, "N:%f:%d:%d:%d", cpu.load, cpu.user, cpu.nice, cpu.system);
	buff[0] = strdup(tmp);

	DEBUG("cpu: %s\n", buff[0]);

	rrd_update_db(datadir, "cpu.rrd", 1, buff);
	free(buff[0]);
}

void cpu_create_graph(char * datadir)
{
	char * params[] = {
		"load",
		"cpu.png",
		"--imgformat", "PNG",
		"--alt-y-grid",
		"--lower-limit", "0",
		"--units-length", "5",
		"--title", "Load and CPU stats",
		"--vertical-label", "Load",
		"--width", "700",
		"--height", "400",
		"--color", "ARROW#000000",
		"--x-grid", "MINUTE:30:MINUTE:30:HOUR:1:0:%H",

		"DEF:load=cpu.rrd:load:AVERAGE",
		"DEF:user=cpu.rrd:user:AVERAGE",
		"DEF:nice=cpu.rrd:nice:AVERAGE",
		"DEF:sys=cpu.rrd:system:AVERAGE",

		"CDEF:cpu=user,nice,sys,+,+",
		"CDEF:reluser=load,user,100,/,*",
		"CDEF:relnice=load,nice,100,/,*",
		"CDEF:relsys=load,sys,100,/,*",
		"CDEF:idle=load,100,cpu,-,100,/,*",

		"HRULE:1#000000",

		"AREA:reluser#FF0000:CPU user",
		"AREA:relnice#00AAFF:CPU nice:STACK",
		"AREA:relsys#FFFF00:CPU system:STACK",
		"AREA:idle#00FF00:CPU idle:STACK",

		"LINE2:load#000888:Load average 5 min",

		"COMMENT:	\\j",
		"COMMENT:\\j",
		"COMMENT:	",

		"GPRINT:cpu:MIN:CPU usage minimum\\: %2.2lf%%",
		"GPRINT:cpu:MAX:CPU usage maximum\\: %2.2lf%%",
		"GPRINT:cpu:AVERAGE:CPU usage average\\: %2.2lf%%"
	};
	if (!rrd_db_exists(datadir, "cpu.rrd")) cpu_create_rrd(datadir);
	rrd_create_graph(sizeof(params) / sizeof(char *), params);
}

cpu_info_t cpu_get_values()
{
	FILE * fp;
	cpu_info_t cpu;
	
	fp = fopen("/proc/loadavg", "r");
	fscanf(fp, "%f", &cpu.load);
	fclose(fp);

	fp = fopen("/proc/stat", "r");
	fscanf(fp, "cpu %u", &cpu.user);
	fscanf(fp, "%u", &cpu.nice);
	fscanf(fp, "%u", &cpu.system);
	fclose(fp);
	
	DEBUG("cpu: load %f\n", cpu.load);
	DEBUG("cpu: user %d\n", cpu.user);
	DEBUG("cpu: nice %d\n", cpu.nice);
	DEBUG("cpu: system %d\n", cpu.system);
	
	
	return cpu;
}
