CXXFLAGS=-std=c++14 -pedantic -Wall -Os -fPIC -I../include

# Additional CXXFLAGS for clang
# CXXFLAGS+=-fsanitize=address #thread,memory

LDLIBS=-lproj

OBJS=proj4.o ll84.o utm.o ups.o usng2.o usng2-c.o
TARGETS=libusng2.so libusng2.a libusng2-c.so libusng2-c.a

.PHONY: all
all: $(TARGETS)

libusng2.so: $(OBJS)
	$(CXX) -fPIC -shared $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@


libusng2.a: $(OBJS)
	$(AR) rs $@ $(OBJS)

libusng2-c.so: libusng2.so usng2-c.o
	$(CXX) -fPIC -shared $(LDFLAGS) -L. -lusng2 $(LDLIBS) -o $@

libusng2-c.a: usng2-c.o
	$(AR) rs $@ usng2-c.o

usng2-c.o: ../include/usng2.h ../include/usng2-c.h

usng2.o: ../include/proj4.h ../include/ll84.h ../include/utm.h ../include/ups.h ../include/usng2.h

ups.o: ../include/ups.h ../include/ll84.h ../include/proj4.h

utm.o: ../include/utm.h ../include/ll84.h

ll84.o: ../include/ll84.h

clean:
	$(RM) -f $(OBJS) $(TARGETS)

