#! /usr/bin/env python

# System imports
from distutils.core import *
from distutils      import sysconfig

# Third-party modules - we depend on numpy for everything
import numpy

# _Series extension module
_peer = Extension("_peer",
                    ["peerPYTHON_wrap.cxx"],
                    include_dirs = [numpy.get_include(),"../include","./../External/alglib/src","./../External"],
                    library_dirs = ['lib','./'],
                  define_macros  = [('SWIG',None)],
                  libraries = ["peerlib"]
                    )

# Series setup
setup(name        = "peer",
      description = "Functions that work on series",
      author      = "Bill Spotz",
      py_modules  = ["peer"],
      ext_modules = [_peer]
      )
