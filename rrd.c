#include <stdio.h>
#include <time.h>
#include <rrd.h>
#include <stdlib.h>

#include "log.h"
#include "utils.h"

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
	
	if (rrd_test_error()) WARNING("RRD error: %s\n", rrd_get_error());
	
	free(filename);
	
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
	
	if (rrd_test_error()) WARNING("RRD error: %s\n", rrd_get_error());
	
	free(filename);
	
	return status;
}

int rrd_create_graph(char * dir, char * name, int argc, char * argv[])
{
	char *filename;
	
	filename = compose_filename(dir, name);
	argv[1] = filename;
	
	DEBUG("Updating graph: %s\n", argv[1]);
	rrd_graph_v(argc, argv);
	
	if (rrd_test_error()) WARNING("RRD error: %s\n", rrd_get_error());
	
	return 1;
}

int rrd_db_exists(char *dir, char *name)
{
    FILE * file;
    char *filename;
    
    filename = compose_filename(dir, name);
    
    if ((file = fopen(filename, "r")) != 0)
    {
        fclose(file);
        free(filename);
        return 1;
    }
    
    free(filename);
    
    return 0;
}