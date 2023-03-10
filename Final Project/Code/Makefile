
LINK.o = $(LINK.cc)
CXXFLAGS = -std=c++14 -Wall


sstUtil = sstheader.o sstindex.o sstvalue.o

all: correctness persistence

correctness: kvstore.o correctness.o memtable.o $(sstUtil)

persistence: kvstore.o persistence.o memtable.o $(sstUtil)

sstheader.o: sstheader.cc

sstindex.o: sstindex.cc 

sstvalue.o: sstvalue.cc



debug: debug.o

clean:
	-rm -f correctness persistence *.o debug
