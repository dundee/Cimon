#include <stdio.h>
#include <time.h>
#include <rrd.h>
#include <stdlib.h>

#include "log.h"
#include "utils.h"

int rrd_create_db(char *dir, char * name, int argc, const char *argv[])
{
	char *filename;
	time_t now;
	
	time(&now);
	
	filename = compose_filename(dir, name);
	if (rrd_create_r(filename, 60, now - 10, argc, argv)) {
		WARNING ("rrd_create (%s) returned error status.\n", filename);
		ERROR("RRD error: %s\n", rrd_get_error());
	} else DEBUG ("Successfully created RRD file \"%s\".\n", filename);
	
	free(filename);
	
	return 0;
}

int rrd_update_db(char * dir, char * name, int argc, char * argv[])
{
	char *filename;
	
	filename = compose_filename(dir, name);
	if(rrd_update_r(filename, NULL, argc, (const char **) argv)) {
		WARNING ("rrd_update (%s) returned error status.\n", filename);
		ERROR("RRD error: %s\n", rrd_get_error());
	} else DEBUG ("Successfully updated RRD file \"%s\".\n", filename);
	
	free(filename);
	
	return 0;
}

int rrd_create_graph(char * dir, char * name, int argc, char * argv[])
{
	rrd_info_t *info;
	char *filename;
	
	filename = compose_filename(dir, name);
	argv[1] = filename;
	
	DEBUG("Updating graph: %s\n", argv[1]);
	info = rrd_graph_v(argc, argv);
	rrd_info_free(info);
	
	if (rrd_test_error()) WARNING("RRD error: %s\n", rrd_get_error());
	
	free(filename);
	
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
