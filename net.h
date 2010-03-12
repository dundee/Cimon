
typedef struct {
	unsigned download;
	unsigned upload;
} net_info_t;

void net_create_rrd(char * datadir);
void net_update_rrd(char * datadir);
void net_create_graph(char * datadir);

net_info_t net_get_values();