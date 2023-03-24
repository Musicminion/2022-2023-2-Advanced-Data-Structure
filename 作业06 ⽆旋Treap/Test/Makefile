LINK.o = $(LINK.cc)
# 必须加上pg 便于性能分析
CXXFLAGS = -std=c++14 -Wall -pg -g

test: test.o

all: test


clean:
	-rm -f test *.o gmon.out result

analysis:
	./test 1024 >> result.csv
	gprof test gmon.out -b >> result-1024.txt
	./test 4096 >> result.csv
	gprof test gmon.out -b >> result-4096.txt
	./test 16384 >> result.csv
	gprof test gmon.out -b >> result-16384.txt
	./test 65536 >> result.csv
	gprof test gmon.out -b >> result-65536.txt