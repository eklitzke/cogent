#!/usr/bin/python

from distutils.core import setup, Extension
import commands
import os

__version__ = '0.1'

def process_cc_opts(output):
	opts = output.split(' ')
	return filter(None, (x[2:] for x in opts))

macros = [('MODULE_VERSION', '"%s"' % __version__)]

status, output = commands.getstatusoutput('pkg-config --cflags-only-I glib-2.0')
if status != 0:
	raise ValueException, 'glib-2.0 not found'
include_dirs = ['..'] + process_cc_opts(output)

#status, output = commands.getstatusoutput('pkg-config --libs-only-l glib-2.0')
#libraries = process_cc_opts(output)

status, output = commands.getstatusoutput('pkg-config --libs-only-L glib-2.0')
library_dirs = process_cc_opts(output)

cogent_extension = Extension(
	name='cogent',
	sources=['cogent.c'],
	libraries=['glib-2.0'],
	include_dirs=include_dirs,
	library_dirs=library_dirs,
	define_macros=macros,
	extra_objects=['../protocol.o']
)

setup(
	name			= 'cogent',
	version			= __version__,
	author			= 'Evan Klitzke',
	author_email	= 'evan@eklitzke.org',
	description		= 'Cogent in-process',
	platforms		= ['Platform Independent'], # FIXME: not quite true
	ext_modules		= [cogent_extension],
    #library_dirs=['/usr/local/lib'],
)
