import cogent
c = cogent.Client()
c.set('foo', 'bar')
r = c.get('foo')
print 'result of get is %s' % r
