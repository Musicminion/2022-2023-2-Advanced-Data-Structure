#include <iostream>
#include <map>
#include <ctime>
#include "treap.h"

using namespace std;

map <int, int> dataAll;
Treap<int> treap;

void generateData(uint64_t size){
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
    // cout << "[Insert Time]The program took cycles to run is: " << end - start << endl;
    // cout << "[Insert] size" << dataAll.size() << endl;
    cout << end - start << ",";
}

void runDeleteTest(){
    uint64_t start, end;
    start = rdtsc();
    for(auto iter = dataAll.begin(); iter != dataAll.end(); iter++){
        treap.remove(iter->first);
    }    
    end = rdtsc();
    // cout << "[Delete Time]The program took cycles to run is: " << end - start << endl;
    // cout << "[Delete] size" << dataAll.size() << endl;
    cout << end - start << endl;
}

void runSearchTest(){
    uint64_t start, end;
    start = rdtsc();
    for(auto iter = dataAll.begin(); iter != dataAll.end(); iter++){
        treap.search(iter->first);
    }    
    end = rdtsc();
    // cout << "[Search Time]The program took cycles to run is: " << end - start << endl;
    // cout << "[Search] size" << dataAll.size() << endl;
    cout << end - start << ",";
}


void runTest(uint64_t dataNum){
    
    // 产生的不同的数据的大小
    uint64_t size = dataNum;
    generateData(size);

    // 插入测试
    runInsertTest();


    // 查询测试
    runSearchTest();

    // std::cout << "treap的高度是 " << treap.getHeight() << std::endl;
    std::cout << treap.getHeight() << ",";

    // 删除测试
    runDeleteTest();
    
    // 每次测试完清空数据
    treap.clear();
}

int main(int argc,char** argv) {
    if(argc != 2){
        cout << "Usage: ./test [dataSize]" << endl;
        return 0;
    }

    cout << "dataSize,insertTime,searchTime,height,deleteTime" << endl;
    // uint64_t dataSize[] = {128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
    // for(int i = 0; i < 10; i++){
    //     // cout << "=====================" << endl;
    //     // cout << "dataSize: " << dataSize[i] << endl;
    //     cout << dataSize[i] << ",";
    //     runTest(dataSize[i]);
    //     // cout << "=====================" << endl;
    // }
    
    cout << atoi(argv[1]) << ",";
    runTest(atoi(argv[1]));

    return 0;
}