import cogent
c = cogent.Client('192.168.1.101')
c.set('foo', 'bar')
print c.get('foo')
