#ifndef _CIMON_CACHE_H
#define	_CIMON_CACHE_H

int cache_is_valid(char *key);
char *cache_get_content(char *key);
void cache_set_content(char *key, char *value, int expiration);

#endif
