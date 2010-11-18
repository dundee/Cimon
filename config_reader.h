#ifndef CONFIG_READER_H
#define CONFIG_READER_H

typedef struct {
	char *data_dir;
	int graph_refresh_interval;
	int port;
	int start_webserver;
} configuration_t;

/**
 * reads configuration from file
 * @param char *file File in which is configuration stored (/etc/cimon.conf)
 * @return int 1 on success, 0 on failure
 */
int read_config(char *file);

/**
 * parses configuration from given stream
 * @param FILE *fp stream
 * @return int 1 on success, 0 on failure
 */
int parse_config(FILE *fp);

/**
 * returns configuration struct (data dir, graph refresh interval, port...)
 */
configuration_t *get_config();

#endif /* config-reader.h */
