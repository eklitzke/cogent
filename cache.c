#include <glib.h>
#include "cache.h"

void
cache_add_data(cogent_cache *cache, gpointer key, gpointer data, gsize size)
{
	lru_add_data(cache->cache, data, size);

	/* race condition! */
	GList *head_link = g_queue_peek_head_link(cache->cache->queue);

	/* not correct! */
	g_hash_table_replace(cache->tbl, key, head_link);
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
	cache_add_data(cache, "key", data_one, size_one);
	return 0;
}
