#ifndef _LRU_H_
#define _LRU_H_

typedef struct
{
	GQueue *queue;
	gsize size;
	gsize max_size;
} lru_cache;

typedef struct
{
	gpointer data;
	gsize size;
} cache_item;

lru_cache* lru_init(gsize max_size);
void lru_promote_link(lru_cache *cache, GList *link_);
void lru_free(lru_cache *cache);
void lru_add_item(lru_cache *cache, cache_item *item);
void lru_add_data(lru_cache *cache, gpointer data, gsize size);

#endif /* _LRU_H_ */
