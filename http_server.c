#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <microhttpd.h>

#include "config.h"
#include "log.h"
#include "utils.h"
#include "render-index.h"
#include "cache.h"
#include "http_server.h"

struct MHD_Daemon * d;

typedef struct {
	unsigned length;
	char * data;
} page_t;

static void response_page(page_t *page, char *page_name)
{
	char *response;

	if (cache_is_valid(page_name)) {
		response = cache_get_content(page_name);
	} else {
		if (!strcmp(page_name, "index")) {
			response = render_index(page_name);
		}
		cache_set_content(page_name, response, 60);
	}
	
	DEBUG("Trying to send page %s\n", page_name);
	
	page->data = response;
	page->length = strlen(response);
}

static void response_file(page_t *page, char *file)
{
	char *buff;
	unsigned length;
	char *filename;
	FILE *fp;
	
	filename = compose_filename(DATA_DIR, file);
	
	fp = fopen(filename, "r");
	if (fp == NULL) {
		WARNING("Cannot open file %s\n", filename);
		return;
	}
	
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	DEBUG("File length: %d\n", length);
	fseek(fp, 0, SEEK_SET);
	
	buff = (char *) malloc(length * sizeof(char));
	memset(buff, 0, length);
	
	fread(buff, 1, length, fp);
	DEBUG("File buffer length: %d\n", strlen(buff));
	if (ferror(fp)) {
		WARNING("Cannot read from file %s\n", file);
		fclose(fp);
		return;
	}
	
	fclose(fp);
	
	page->length = length;
	page->data = buff;
	free(filename);
}

static int handle_request(void * cls,
                   struct MHD_Connection * connection,
                   const char * url,
                   const char * method,
                   const char * version,
                   const char * upload_data,
                   size_t * upload_data_size,
                   void ** ptr)
{
	static int dummy;
	struct MHD_Response * response;
	int ret;
	int isfile = 0;
	page_t page;
	
	if (0 != strcmp(method, "GET")) return MHD_NO; /* unexpected method */
	if (&dummy != *ptr) { // first round only headers
		*ptr = &dummy;
		return MHD_YES;
	}
	
	if (0 != *upload_data_size) return MHD_NO; /* upload data in a GET!? */
	
	*ptr = NULL; /* clear context pointer */
	
	if (!strcmp(url, "/")) {
		DEBUG("Sending page: %s\n", url);
		response_page(&page, "index");
	} else if (!strcmp(url, "/memory.png")) {
		DEBUG("Sending file: %s\n", url);
		response_file(&page, "memory.png");
	} else if (!strcmp(url, "/cpu.png")) {
		DEBUG("Sending file: %s\n", url);
		response_file(&page, "cpu.png");
		isfile = 1;
	} else if (!strcmp(url, "/net.png")) {
		DEBUG("Sending file: %s\n", url);
		response_file(&page, "net.png");
		isfile = 1;
	} else if (!strcmp(url, "/swap.png")) {
		DEBUG("Sending file: %s\n", url);
		response_file(&page, "swap.png");
		isfile = 1;
	} else {
		DEBUG("Request unknown: %s\n", url);
		return MHD_NO;
	}
	
	
	response = MHD_create_response_from_data(page.length,
	                                         (void*) page.data,
	                                         MHD_NO,
	                                         MHD_NO);
	
	if (isfile) MHD_add_response_header (response, "Content-type", "image/png");
	
	ret = MHD_queue_response(connection,
	                         MHD_HTTP_OK,
	                         response);
	MHD_destroy_response(response);
	return ret;
}

int http_server_run(int port)
{
	d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
	                     port,
	                     NULL,
	                     NULL,
	                     &handle_request,
	                     NULL,
	                     MHD_OPTION_END);
	if (d == NULL) {
		WARNING("Cannot start HTTP server on port %d", port);
		return 1;
	}
	DEBUG("Started HTTP server on port %d\n", port);
	while (1) {
		sleep (60);
	}
	MHD_stop_daemon(d);
	return 0;
}
