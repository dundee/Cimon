#ifndef _CIMON_RRD_H
#define	_CIMON_RRD_H

int rrd_create_db(char *dir, char *name, int argc, const char *argv[]);
int rrd_update_db(char *dir, char *name, int argc, char *argv[]);
int rrd_create_graph(int argc, char *argv[]);
int rrd_db_exists(char *dir, char *name);

#endif