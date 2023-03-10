
LINK.o = $(LINK.cc)
CXXFLAGS = -std=c++14 -Wall

all: correctness persistence

correctness: memtable.o kvstore.o correctness.o 

persistence: memtable.o kvstore.o persistence.o

debug: debug.o

clean:
	-rm -f correctness persistence *.o debug
