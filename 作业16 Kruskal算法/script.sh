# !/bin/bash


# g++ -o serial ./serial.cpp
# g++ -o data ./data.cpp


./serial < 2000_10000.txt
./serial < 2000_100000.txt
./serial < 2000_1000000.txt

./serial < 5000_10000.txt
./serial < 5000_100000.txt
./serial < 5000_1000000.txt

./serial < 10000_10000.txt
./serial < 10000_100000.txt
./serial < 10000_1000000.txt

# 
