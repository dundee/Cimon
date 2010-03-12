
typedef struct {
	unsigned used;
	unsigned cached;
	unsigned buffers;
	unsigned free;
	unsigned total;
} memory_info_t;

void memory_create_rrd(char * datadir);

memory_info_t memory_get_values();