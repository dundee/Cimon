#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "config_reader.h"

#define DEFAULT_DATA_DIR "/var/lib/cimon"
#define DEFAULT_PORT 8080
#define DEFAULT_GRAPH_REFRESH_INTERVAL 600
#define DEFAULT_START_WEBSERVER 1

#define BUFF_SIZE 1024

configuration_t *config;

void ltrim(char **s)
{
	int len;
	int i = 0;
	
	len = strlen(*s);
	while (i < len &&
	       ((*s)[i] < 33 || (*s)[i] > 126)) {
		i++; //trim whitespace
	}
	*s = (*s) + i;
}

void rtrim(char **s)
{
	int i;
	
	i = strlen(*s) - 1;
	while (i >= 0 && 
	       ((*s)[i] < 33 || (*s)[i] > 126)) {
		(*s)[i] = '\0';
		i--;
	}
}

void init_config(configuration_t *config)
{
	config->data_dir = NULL;
	config->graph_refresh_interval = -1;
	config->port = -1;
	config->start_webserver = -1;
}

int parse_config(FILE *fp)
{
	char *line, *tmp;
	
	if (!fp) {
		WARNING ("Wrong stream for %s\n", "parsing");
		exit(1);
	}
	
	line = (char *) malloc(sizeof(char) * BUFF_SIZE);
	memset(line, 0, BUFF_SIZE);
	
	config = (configuration_t*) malloc(sizeof(configuration_t));
	init_config(config);
	
	while (1) {
		line = fgets(line, BUFF_SIZE, fp);
		DEBUG("Config line: %s\n", line);
		if (line == NULL) break;
		if (line[0] == '#') continue; //whole line is comment
		line = strtok(line, "#"); // remove comments
		if (line == NULL) continue;
		DEBUG("Uncommented config line: %s\n", line);
		
		tmp = line;
		ltrim(&tmp);
		DEBUG("Left trimmed line: %s\n", tmp);
		rtrim(&tmp);
		
		if (strlen(tmp) == 0) continue; // emtpy line
		DEBUG("Trimmed line: %s\n", tmp);
		
		if (strncasecmp(tmp, "DataDir", strlen("DataDir")) == 0) {
			tmp += strlen("DataDir");
			ltrim(&tmp);
			config->data_dir = strdup(tmp);
			DEBUG("DataDir: '%s'\n", config->data_dir);
		} else if (strncasecmp(tmp, "GraphRefreshInterval", strlen("GraphRefreshInterval")) == 0) {
			tmp += strlen("GraphRefreshInterval");
			ltrim(&tmp);
			sscanf(tmp, "%d", &config->graph_refresh_interval);
			DEBUG("GraphRefreshInterval: %d\n", config->graph_refresh_interval);
		} else if (strncasecmp(tmp, "Port", strlen("Port")) == 0) {
			tmp += strlen("Port");
			ltrim(&tmp);
			sscanf(tmp, "%d", &config->port);
			DEBUG("Port: %d\n", config->port);
		} else if (strncasecmp(tmp, "StartWebserver", strlen("StartWebserver")) == 0) {
			tmp += strlen("StartWebserver");
			ltrim(&tmp);
			sscanf(tmp, "%d", &config->start_webserver);
			DEBUG("StartWebserver: %d\n", config->start_webserver);
		} else {
			WARNING("Wrong %s\n", "configuration");
			return 0;
		}
	}
	
	if (config->data_dir               == NULL) config->data_dir               = DEFAULT_DATA_DIR;
	if (config->port                   == -1  ) config->port                   = DEFAULT_PORT;
	if (config->graph_refresh_interval == -1  ) config->graph_refresh_interval = DEFAULT_GRAPH_REFRESH_INTERVAL;
	if (config->start_webserver        == -1  ) config->start_webserver        = DEFAULT_START_WEBSERVER;
	
	return 1;
}

int read_config(char *file)
{
	int res;
	FILE *fp;
	
	fp = fopen(file, "r");
	if (!fp) {
		WARNING("Configuration file %s doesn't exist.\n", file);
		exit(1);
	}
	
	res = parse_config(fp);
	
	fclose(fp);
	return res;
}

configuration_t *get_config()
{
	return config;
}