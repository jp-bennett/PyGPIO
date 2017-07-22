from distutils.core import setup, Extension
setup(name='gpio', version='1.0',  \
      ext_modules=[Extension('gpio', ['gpiomodule.c'])])
