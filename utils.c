#include <string.h>
#include <stdio.h>

#include "log.h"

char * compose_filename(char *dir, char *name)
{
	char filename[FILENAME_MAX];
	
	strncpy(filename, dir, FILENAME_MAX - 1);
	filename[FILENAME_MAX - 1] = '\0';
	strncat(filename, "/", FILENAME_MAX - 2);
	strncat(filename, name, FILENAME_MAX - strlen(filename) - 1);
	
	DEBUG("Filename: %s\n", filename);
	return strdup(filename);
}