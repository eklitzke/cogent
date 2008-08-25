import unittest

import cogent

class CogentTest(unittest.TestCase):

	def test_basic_get_and_fetch(self):
		cache = cogent.Cache(1000)

		val = 'cogent'
		cache.store('foo', val)
		assert cache.get('foo') is val

if __name__ == '__main__':
	unittest.main()
