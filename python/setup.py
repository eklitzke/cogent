#!/usr/bin/python

from distutils.core import setup, Extension
import os

__version__ = '0.1'

macros = [('MODULE_VERSION', '"%s"' % __version__)]

cogent_extension = Extension(
	name='cogent',
	sources=['cogent.c'],
	libraries=['glib-2.0'],
	include_dirs=["/usr/include/glib-2.0", "/usr/lib/glib-2.0/include", ".."],
	define_macros=macros,
	extra_objects=["../cache.o"]
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
