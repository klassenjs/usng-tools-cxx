#!/usr/bin/python

import sys, os

from distutils.core import setup, Extension
from distutils import sysconfig
from distutils.command.build_ext import build_ext
from distutils.ccompiler import get_default_compiler
from distutils.sysconfig import get_python_inc

include_dirs = ['../../include']
library_dirs = ['../../lib']
libraries = ['usng2', 'proj']

extra_link_args = []
extra_compile_args = ['-std=c++14']


if not os.path.exists('usngc_wrap.cxx') :
    swig_cmd = """swig -c++ -python -shadow -modern -templatereduce -fastdispatch -fvirtual -fastproxy -modernargs -castmode -dirvtable -fastinit -fastquery -noproxydel -nobuildnone -I../../include -o usngc_wrap.cxx ../usngc.i"""
    os.system(swig_cmd)


usngc_module = Extension('_USNGC',
    sources=['usngc_wrap.cxx'],
    include_dirs = include_dirs,
    library_dirs = library_dirs,
    libraries = libraries,
    extra_compile_args = extra_compile_args,
    extra_link_args = extra_link_args)


setup (
  name='USNGC',
  version='1.0',
  description='Convert to and from USNG coordinate representation',
  author='Jim Klassen',
  author_email='klassen.js@gmail.com',
  ext_modules = [usngc_module],
  py_modules = ['USNGC']
)
