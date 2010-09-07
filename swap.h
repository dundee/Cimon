
typedef struct {
	unsigned free;
	unsigned used;
	unsigned total;
} swap_info_t;

void swap_create_rrd(char * datadir);
void swap_update_rrd(char * datadir);
void swap_create_graph(char * datadir);

swap_info_t *swap_get_values();
