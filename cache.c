#include <glib.h>
#include <stdio.h>
#include "cache.h"

void
cache_store(cogent_cache *cache, gpointer key, gpointer data, gsize size)
{
	lru_add_data(cache->cache, data, size);

	/* race condition! */
	GList *head_link = g_queue_peek_head_link(cache->cache->queue);

	/* not correct! */
	g_hash_table_replace(cache->tbl, key, head_link);
}

/* Fetch an item from the cache */
cache_item *
cache_fetch(cogent_cache *cache, gconstpointer key)
{
	GList *link = g_hash_table_lookup(cache->tbl, key);
	if (link != NULL)
	{
		lru_promote_link(cache->cache, link);
		return link->data;
	}
	return NULL;
}

cogent_cache *
cache_init(gsize size)
{
	cogent_cache *cache = g_slice_new(cogent_cache);
	cache->tbl = g_hash_table_new(g_str_hash, g_str_equal);
	cache->cache = lru_init(size);
	return cache;
}

int main (int argc, int argv)
{
	cogent_cache *cache = cache_init(1e5);

	gpointer data_one = g_strdup("foo");
	//gsize size_one = g_strlen(data_one);
	gsize size_one = 3;

	gpointer key = "key";
	cache_store(cache, "key", data_one, size_one);

	cache_item *item = cache_fetch(cache, "key");
	printf("got %s\n", item->data);
	return 0;
}
