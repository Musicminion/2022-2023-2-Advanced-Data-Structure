#include <iostream>
#include <map>
#include "treap.h"

using namespace std;

map <int, int> dataAll;
Treap<int> treap;

void generateData(int size){
    srand(time(NULL));
    while(dataAll.size() < size){
        int data = rand();
        dataAll[data] = 1;
    }
}

static inline uint64_t rdtsc(){
    uint32_t low, high;
    asm volatile ("rdtsc" : "=a" (low), "=d" (high));
    return ((uint64_t)high << 32) | low;
}



void runInsertTest(){
    uint64_t start, end;
    start = rdtsc();
    for(auto iter = dataAll.begin(); iter != dataAll.end(); iter++){
        treap.insert(iter->first);
    }    
    end = rdtsc();
    cout << "[Insert Time]The program took cycles to run is: " << end - start << endl;
    cout << "[Insert] size" << dataAll.size() << endl;
}

void runDeleteTest(){
    uint64_t start, end;
    start = rdtsc();
    for(auto iter = dataAll.begin(); iter != dataAll.end(); iter++){
        treap.remove(iter->first);
    }    
    end = rdtsc();
    cout << "[Delete Time]The program took cycles to run is: " << end - start << endl;
    cout << "[Delete] size" << dataAll.size() << endl;
}


void runTest(){
    
    // 产生的不同的数据的大小
    int size = 1000;
    generateData(size);

    // 插入测试
    runInsertTest();

    // 删除测试
    runDeleteTest();
    
}

int main() {

    return 0;
}