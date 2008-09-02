import cogent
c = cogent.Client()
c.set('foo', 'bar')
while True:
	r = c.get('foo')
	print 'result of get is %s' % r
