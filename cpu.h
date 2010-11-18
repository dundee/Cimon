
typedef struct {
	float load;
	unsigned user;
	unsigned nice;
	unsigned system;
} cpu_info_t;

void cpu_create_rrd(char * datadir);
void cpu_update_rrd(char * datadir);
void cpu_create_graph(char * datadir);

cpu_info_t *cpu_get_values();
