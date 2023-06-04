# !/bin/bash


# g++ -o serial ./serial.cpp
# g++ -o data ./data.cpp
# g++ -o parallel ./parallel.cpp -pthread # 让我们说谢谢gpt


./serial < 2000_10000.txt
./serial < 2000_100000.txt
./serial < 2000_1000000.txt

./serial < 5000_10000.txt
./serial < 5000_100000.txt
./serial < 5000_1000000.txt

./serial < 10000_10000.txt
./serial < 10000_100000.txt
./serial < 10000_1000000.txt


# 并行
./parallel < 2000_10000.txt
./parallel < 2000_100000.txt
./parallel < 2000_1000000.txt

./parallel < 5000_10000.txt
./parallel < 5000_100000.txt
./parallel < 5000_1000000.txt

./parallel < 10000_10000.txt
./parallel < 10000_100000.txt
./parallel < 10000_1000000.txt


# 6 9
# 1 2 1
# 1 6 2
# 2 6 6
# 2 3 11
# 6 3 9
# 6 5 14
# 3 5 7
# 3 4 3
# 5 4 4
