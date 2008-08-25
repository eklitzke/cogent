#include <glib.h>

#include "lru.h"

static void
lru__promote_link(lru_cache *cache, GList *link_)
{
	g_queue_unlink(cache->queue, link_);
	g_queue_push_head_link(cache->queue, link_);
}

inline static void
lru__remove_link(lru_cache *cache, GList *link_)
{
	cache_item *item = link_->data;
	cache->size -= sizeof(GList) + sizeof(cache_item) + item->size;

	g_slice_free1(item->size, item->data);
	g_slice_free(cache_item, link_->data);
	g_list_free_1(link_);
}

lru_cache *
lru_init(gsize max_size)
{
	lru_cache *cache = g_slice_new(lru_cache);
	cache->size = 0;
	cache->max_size = max_size;
	cache->queue = g_queue_new();

	return cache;
}

void
lru_free(lru_cache *cache)
{
	/* TODO: this is a slow hack */
	while (!g_queue_is_empty(cache->queue))
	{
		GList *link_ = g_queue_pop_head_link(cache->queue);
		lru__remove_link(cache, link_);
	}
}

void
lru_add_item(lru_cache *cache, gpointer data, gsize size)
{
	gsize size_increment = sizeof(GList) + sizeof(cache_item) + size;
	g_assert(size_increment < (cache->max_size / 5));
	cache->size += size_increment;

	/* Maybe free data by removing from the end of the cache */
	while (cache->size > cache->max_size)
	{
		GList *link_ = g_queue_pop_tail_link(cache->queue);
		lru__remove_link(cache, link_);
	}

	cache_item *item = g_slice_new(cache_item);
	item->data = data;
	item->size = size;

	g_queue_push_head(cache->queue, item);
}

#if 0
int main()
{
	lru_cache *cache = lru_init(1e5);
	while(1)
	{
		gpointer data = g_slice_alloc(10);
		lru_add_item(cache, data, 10);
	}

	g_assert_not_reached();
	return 0;
}
#endif
