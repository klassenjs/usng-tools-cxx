#!/usr/bin/ruby

print "Generating SWIG wrapper...\n"
system("swig -c++ -ruby -outcurrentdir -I../../include ../usngc.i")

require 'mkmf'
$CXXFLAGS="-std=c++14 -I../../include"
find_library("usng2", nil, "../../lib")
find_library("proj", "proj_create")
create_makefile('USNGC')
