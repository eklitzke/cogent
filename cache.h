#ifndef _CACHE_H_
#define _CACHE_H_

#include "lru.h"

typedef struct
{
	GHashTable *tbl;
	lru_cache *cache;
} cogent_cache;

void cache_store(cogent_cache *cache, gpointer key, gpointer data, gsize size);
cache_item * cache_fetch(cogent_cache *cache, gconstpointer key);
cogent_cache * cache_init(gsize size);

#endif /* _CACHE_H_ */
