from distutils.core import setup, Extension

PyLorcon2 = Extension('PyLorcon2',
                      sources = ['PyLorcon2.c'],
                      libraries = ['orcon2', 'pcap', 'nl'])

setup(name = 'PyLorcon2',
      version = '0.1',
      description = 'Python wrapper for the Lorcon2 library',
      license = 'GNU GPLv3',
      author = 'Andres Blanco (6e726d), Ezequiel Gutesman (gutes)',
      author_email = '6e726d@gmail.com, egutesman@coresecurity.com',
      url = 'http://code.google.com/p/pylorcon2',
      ext_modules = [PyLorcon2])
