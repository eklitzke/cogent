import unittest
import cogent

class CogentTest(unittest.TestCase):

	def _test_basic_get_and_fetch(self):
		cache = cogent.Cache(1000)

		val = 'cogent'
		cache.store('foo', val)

		hit, fetched = cache.get('foo')
		assert hit
		print fetched
		assert fetched is val
	
	def test_str_get_and_fetch(self):
		cache = cogent.Cache(4096)
		key = 'foo'
		val = 'cogent'

		cache.store_str(key, val)
		hit, cache_val = cache.get_str(key)
		assert hit
		assert cache_val == val
	
	def test_size(self):
		cache = cogent.Cache(4096)
		assert cache.size() == 0
		assert cache.max_size() == 4096

		cache.store_str('foo', 'val')
		assert cache.size() > 0
		assert cache.max_size() == 4096

if __name__ == '__main__':
	unittest.main()
