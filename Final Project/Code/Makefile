
LINK.o = $(LINK.cc)
CXXFLAGS = -std=c++14 -Wall

sstUtil = sstheader.o sstindex.o sstvalue.o sstable.o

all: correctness persistence

correctness: kvstore.o correctness.o memtable.o $(sstUtil)

persistence: kvstore.o persistence.o memtable.o $(sstUtil)

performanceTest: performanceTest.o kvstore.o memtable.o $(sstUtil)

sstheader.o: sstheader.cc

sstindex.o: sstindex.cc 

sstvalue.o: sstvalue.cc

sstable.o: sstable.cc

performanceTest.o : performanceTest.cc

clean:
	-rm -f correctness persistence *.o performanceTest
	-rm -f ./data/level-*/*.sst

cleandata:
	-rm -f ./data/level-*/*.sst

