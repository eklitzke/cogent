#ifndef _CACHE_H_
#define _CACHE_H_

#include "lru.h"

typedef struct
{
	GHashTable *tbl;
	lru_cache *cache;
} cogent_cache;

#endif /* _CACHE_H_ */
