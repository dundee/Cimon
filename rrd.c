#include <stdio.h>
#include <time.h>
#include <rrd.h>

#include "log.h"

static char * compose_filename(char *dir, char *name)
{
	char filename[FILENAME_MAX];
	
	strncpy(filename, dir, FILENAME_MAX - 1);
	filename[FILENAME_MAX - 1] = '\0';
	strncat(filename, "/", FILENAME_MAX - 2);
	strncat(filename, name, FILENAME_MAX - strlen(filename) - 1);
	
	return strdup(filename);
}

int rrd_create_db(char *dir, char * name, int argc, const char *argv[])
{
	char *filename;
	int status;
	time_t now;
	
	time(&now);
	
	filename = compose_filename(dir, name);
	status = rrd_create_r(filename, 60, now - 10, argc, argv);
	
	if (status != 0) WARNING ("rrd_create (%s) returned status %i.\n", filename, status);
	else DEBUG ("Successfully created RRD file \"%s\".\n", filename);
	
	return status;
}

int rrd_update_db(char * dir, char * name, int argc, char * argv[])
{
	char *filename;
	int status;
	
	filename = compose_filename(dir, name);
	status = rrd_update_r(filename, NULL, argc, (const char **) argv);
	
	if (status != 0) WARNING ("rrd_update (%s) returned status %i.\n", filename, status);
	else DEBUG ("Successfully updated RRD file \"%s\".\n", filename);
	
	return status;
}

int rrd_create_graph(int argc, char * argv[])
{
	rrd_graph_v(argc, argv);
	
	return 1;
}