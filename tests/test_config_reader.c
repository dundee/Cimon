#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

#include "../config_reader.h"

int test_config_reader_init_suite()
{
	return 0;
}

int test_config_reader_clean_suite()
{
	return 0;
}

void test_config_reader_PARSE_CONFIG()
{
	FILE *fp;
	configuration_t *config;
	char *buff;
	int res;
	
	buff = "neco";
	fp = fmemopen(buff, strlen(buff), "r");
	res = parse_config(fp);
	CU_ASSERT(res == 0);
	fclose(fp);
	
	buff = " ";
	fp = fmemopen(buff, strlen(buff), "r");
	res = parse_config(fp);
	CU_ASSERT(res == 1);
	fclose(fp);
	
	buff = "StartWebserver 0";
	fp = fmemopen(buff, strlen(buff), "r");
	res = parse_config(fp);
	config = get_config();
	CU_ASSERT(config->start_webserver == 0);
	fclose(fp);
	
	buff = "StartWebserver 0#comment";
	fp = fmemopen(buff, strlen(buff), "r");
	res = parse_config(fp);
	config = get_config();
	CU_ASSERT(config->start_webserver == 0);
	fclose(fp);
	
	buff = "DataDir /xx/yy";
	fp = fmemopen(buff, strlen(buff), "r");
	res = parse_config(fp);
	config = get_config();
	CU_ASSERT(strcmp(config->data_dir, "/xx/yy") == 0);
	fclose(fp);
	
	buff = "DataDir /xx/yy\n\
	Port 55\n\
\n\
	GraphRefreshInterval 300  \n\
	\n\
	StartWebserver 0";
	fp = fmemopen(buff, strlen(buff), "r");
	res = parse_config(fp);
	config = get_config();
	CU_ASSERT(strcmp(config->data_dir, "/xx/yy") == 0);
	CU_ASSERT(config->graph_refresh_interval == 300);
	CU_ASSERT(config->port == 55);
	CU_ASSERT(config->start_webserver == 0);
	fclose(fp);
	
	buff = "Port 10\n\
#Port 5";
	fp = fmemopen(buff, strlen(buff), "r");
	res = parse_config(fp);
	config = get_config();
	CU_ASSERT(config->port == 10);
	fclose(fp);
}
