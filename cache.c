#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "cache.h"

typedef struct {
	char *key;
	char *value;
	time_t created;
	unsigned int expiration;
} cache_record_t;

cache_record_t *cache[1024];
int cache_size = 0;

static int get_key_number(char *key)
{
	int i;
	int len;
	
	len = strlen(key);
	
	for (i = 0; i < cache_size; i++) {
		if (!strncmp(key, cache[i]->key, len)) {
			return i;
		}
	}
	
	return -1;
}

int cache_is_valid(char *key)
{
	int i;
	time_t now;
	
	i = get_key_number(key);
	if (i == -1) {
		DEBUG("Cache key '%s' not found\n", key);
		return 0;
	}
	
	now = time(NULL);
	
	if (now > cache[i]->created + cache[i]->expiration) { /* expired */
		DEBUG("Cache key '%s' found but expired (now is %d, cache is from %d and expires in %d)\n", key, now, cache[i]->created, cache[i]->expiration);
		return 0;
	} else {
		DEBUG("Cache key '%s' found and valid\n", key);
		return 1;
	}
}

char *cache_get_content(char *key)
{
	int i;
	i = get_key_number(key);
	
	return strdup(cache[i]->value);
}


void cache_set_content(char *key, char *value, int expiration)
{
	int i;
	
	i = get_key_number(key);
	if (i != -1) { /* already in cache */
		cache[i]->value      = strdup(value);
		cache[i]->created    = time(NULL);
		cache[i]->expiration = expiration;
	} else {
		i = cache_size++;
		
		cache[i] = (cache_record_t *) malloc(sizeof(cache_record_t));
		memset(cache[i], 0, sizeof(cache_record_t));
		
		cache[i]->key        = strdup(key);
		cache[i]->value      = strdup(value);
		cache[i]->created    = time(NULL);
		cache[i]->expiration = expiration;
	}
}
