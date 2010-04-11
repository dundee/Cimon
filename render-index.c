#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 1024 * 4

#include "log.h"
#include "memory.h"
#include "cpu.h"
#include "net.h"

char * render_index()
{
	char *buffer, *tmp;
	memory_info_t *memory;
	cpu_info_t *cpu;
	net_info_t *net;
	
	buffer = tmp = (char *) malloc(sizeof(char) * BUFF_SIZE);
	
	memory = memory_get_values();
	cpu    = cpu_get_values();
	net    = net_get_values();
	
	
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n\
<head>\n\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
<meta http-equiv=\"Content-Language\" content=\"en\" />\n\
<meta name=\"Generator\" content=\"Cimon web\" />\n\
<meta name=\"robots\" content=\"all, follow\" />\n\
<meta name=\"googlebot\" content=\"index,follow,snippet,archive\" />\n\
<meta name=\"author\" content=\"Daniel Milde\" />\n\
<title>Cimon - Load statistics</title>\n\
<script type=\"text/javascript\" src=\"http://code.jquery.com/jquery-1.4.2.min.js\"></script>\n\
<script type=\"text/javascript\">\n\
function showTab() {\n\
	var elem = $(this);\n\
	$('#menu li a').removeClass('selected');\n\
	elem.addClass('selected');\n\
	$('.tab').hide();\n\
	$(elem.attr('href')).show();\n\
	return false;\n\
}\n\
\n\
$(document).ready(function(){\n\
	$('#graphs').hide();\n\
	$('#menu li a').click(showTab);\n\
});\n\
</script>\n\
<style type=\"text/css\">\n\
body {\n\
	font: normal 0.9em arial, sans-serif;\n\
}\n\
table {\n\
	border-collapse: collapse;\n\
}\n\
table td {\n\
	border: 1px solid #333;\n\
	padding: 5px;\n\
}\n\
.info {\n\
	float: left;\n\
	width: 200px;\n\
}\n\
.tab {\n\
	clear: both;\n\
	background: #ddd;\n\
	border: 1px solid #666;\n\
	padding: 10px;\n\
	margin-top: 5px;\n\
}\n\
.cleaner {\n\
	clear: both;\n\
}\n\
#menu {\n\
	list-style-type: none;\n\
	margin: 0;\n\
	padding: 0;\n\
	margin-left: 20px;\n\
}\n\
#menu li {\n\
	float: left;\n\
	margin-right: 10px;\n\
	margin-bottom: 5px;\n\
}\n\
#menu li a {\n\
	padding: 5px;\n\
	border: 1px solid #666;\n\
	background: #50B0BC;\n\
	text-decoration: none;\n\
	color: #00f;\n\
}\n\
#menu li a.selected {\n\
	border-bottom: none;\n\
	background: #ddd;\n\
	position: relative;\n\
	top: 1px;\n\
}\n\
</style>\n\
</head>\n\
<body>");

	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "<h1>Cimon - Load statistics</h1>\n");
	
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "<ul id=\"menu\">\n\
	<li><a href=\"#actual\">Actual</a></li>\n\
	<li><a href=\"#graphs\">Graphs</a></li>\n\
</ul>\n");
	
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "<div id=\"actual\" class=\"tab\">\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t<div class=\"info\">\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t<h2>CPU</h2>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t<table>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Load:</td><td>%f</td></tr>\n", cpu->load);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>User:</td><td>%u</td></tr>\n", cpu->user);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Nice:</td><td>%u</td></tr>\n", cpu->nice);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>System:</td><td>%u</td></tr>\n", cpu->system);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t</table>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t</div>\n");
	
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t<div class=\"info\">\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t<h2>Memory</h2>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t<table>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Used:</td><td>%u MB</td></tr>\n", memory->used/1000);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Cached:</td><td>%u MB</td></tr>\n", memory->cached/1000);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Buffers:</td><td>%u MB</td></tr>\n", memory->buffers/1000);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Free:</td><td>%u MB</td></tr>\n", memory->free/1000);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Total:</td><td>%u MB</td></tr>\n", memory->total/1000);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t</table>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t</div>\n");
	
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t<div class=\"info\">\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t<h2>Network</h2>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t<table>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Download:</td><td>%u</td></tr>\n", net->download);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t\t<tr><td>Upload:</td><td>%u</td></tr>\n", net->upload);
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t\t</table>\n");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "\t</div>\n");
	
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "<div class=\"cleaner\"></div>");
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "</div>\n"); /* end tab actual */
	
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "<div id=\"graphs\" class=\"tab\">\n\
	<p><img src=\"./memory.png\" /></p>\n\
	<p><img src=\"./cpu.png\" /></p>\n\
	<p><img src=\"./net.png\" /></p>\n\
	<div class=\"cleaner\"></div>\n\
</div>\n");
	
	tmp += snprintf(tmp, BUFF_SIZE-(tmp-buffer), "</body></html>");
	
	DEBUG("Response for %s written to buffer\n", "index");
	
	free(memory);
	free(cpu);
	free(net);
	
	DEBUG("Structures for %s freed\n", "memory, cpu, net");
	
	return buffer;
}