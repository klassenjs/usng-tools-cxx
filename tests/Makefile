CFLAGS=-pedantic -Wall -Os -I../include
CXXFLAGS=-std=c++14 -pedantic -Wall -Os -I../include
LDFLAGS=-L../lib
LDLIBS=-lproj

TARGETS=test-shared test-static testc test-parallel

.PHONY: all
all: $(TARGETS)

test-parallel: ../lib/libusng2.so test-parallel.cc
	$(CXX) $(CXXFLAGS) test-parallel.cc $(LDFLAGS) \
		-Wl,-rpath,\$$ORIGIN/../lib -lusng2 \
		$(LDLIBS) -lpthread -o $@

test-shared: ../lib/libusng2.so test.cc
	$(CXX) $(CXXFLAGS) test.cc $(LDFLAGS) \
		-Wl,-rpath,\$$ORIGIN/../lib -lusng2 \
		$(LDLIBS) -o $@

test-static: ../lib/libusng2.a test.cc
	$(CXX) $(CXXFLAGS) test.cc $(LDFLAGS) \
		-Wl,-Bstatic -lusng2 -Wl,-Bdynamic $(LDLIBS) -o $@

testc: testc.c ../lib/libusng2-c.a
	$(CC) $(CFLAGS) testc.c $(LDFLAGS) -Wl,-Bstatic -lusng2-c -lusng2 -Wl,-Bdynamic -lstdc++ -lm $(LDLIBS) -o $@

clean:
	$(RM) -f $(OBJS) $(TARGETS)

.PHONY: callgrind
callgrind: test
	valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes ./test
