
typedef struct {
	unsigned used;
	unsigned cached;
	unsigned buffers;
	unsigned free;
	unsigned total;
} memory_info_t;

void memory_create_rrd(char * datadir);
void memory_update_rrd(char * datadir);
void memory_create_graph(char * datadir);

memory_info_t *memory_get_values();