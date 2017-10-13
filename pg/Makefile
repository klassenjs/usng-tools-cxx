CXXFLAGS := -std=c++14 -pedantic -Wall -Os -fpic

PG_CPPFLAGS := -I../include
SHLIB_LINK := -L../lib -Wl,-Bstatic -lusng2 -Wl,-Bdynamic -lproj -lstdc++ -lm

EXTENSION = usng
MODULE_big = pg_usng
OBJS=pg_usng.o
DATA = usng--1.0.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

