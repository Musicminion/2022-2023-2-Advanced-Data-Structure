#include "cuckoo.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <random>
#include <time.h>
#include <filesystem>

long long int serializePUTTest(long long int capacity, long long int insertSize) {
    if (capacity == 0 || insertSize == 0) {
        std::cout << "Invalid input" << std::endl;
        return -1;
    }

    if (insertSize > capacity) {
        std::cout << "Insert size should be smaller than capacity" << std::endl;
        return -1;
    }


    cuckoo::Cuckoo cuckoo(capacity);
    std::map<long long int, long long int> insertMap;
    std::vector<long long int> insertList;

    // 随机生成long long int的key
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long int> dis(1, capacity);

    // 生成插入的key
    for (int i = 0; i < insertSize; i++) {
        long long int key = dis(gen);

        while(insertMap.count(key) != 0){
            key = dis(gen);
        }

        insertMap[key] = key;
        insertList.emplace_back(key);
    }
    
    
    // 开始计时
    auto start = std::chrono::steady_clock::now();
    
    // 插入
    for (auto key : insertList) {
        cuckoo.put(key);
    }

    // 结束计时
    auto end = std::chrono::steady_clock::now();

    // 输出纳秒级别的时间差
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}


long long int parallelPUTTest(int threadNum,long long int capacity, long long int insertSize){
    if (capacity == 0 || insertSize == 0) {
        std::cout << "Invalid input" << std::endl;
        return -1;
    }

    if (insertSize > capacity) {
        std::cout << "Insert size should be smaller than capacity" << std::endl;
        return -1;
    }

    cuckoo::Cuckoo cuckoo(capacity);
    std::map<long long int, long long int> insertMap;
    std::vector<long long int> insertList;

    // 随机生成long long int的key
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long int> dis(1, capacity);

    // 生成插入的key
    for (int i = 0; i < insertSize; i++) {
        long long int key = dis(gen);

        while(insertMap.count(key) != 0){
            key = dis(gen);
        }

        insertMap[key] = key;
        insertList.emplace_back(key);
    }

    // 开始计时
    auto start = std::chrono::steady_clock::now();

    // 插入
    std::vector<std::thread> threads;
    for (int i = 0; i < threadNum; i++) {
        threads.emplace_back(std::thread([&cuckoo, &insertList, i, threadNum](){
            for (int j = i; j < insertList.size(); j += threadNum) {
                cuckoo.put(insertList[j]);
            }
        }));
    }

    for (auto &thread : threads) {
        thread.join();
    }

    // 结束计时
    auto end = std::chrono::steady_clock::now();

    // 输出纳秒级别的时间差
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

long long int serializeGETTest(long long int capacity, long long int insertSize){
    if (capacity == 0 || insertSize == 0) {
        std::cout << "Invalid input" << std::endl;
        return -1;
    }

    if (insertSize > capacity) {
        std::cout << "Insert size should be smaller than capacity" << std::endl;
        return -1;
    }


    cuckoo::Cuckoo cuckoo(capacity);
    std::map<long long int, long long int> insertMap;
    std::vector<long long int> insertList;

    // 随机生成long long int的key
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long int> dis(1, capacity);

    // 生成插入的key
    for (int i = 0; i < insertSize; i++) {
        long long int key = dis(gen);

        while(insertMap.count(key) != 0){
            key = dis(gen);
        }

        insertMap[key] = key;
        insertList.emplace_back(key);
    }

    // 插入
    for (auto key : insertList) {
        cuckoo.put(key);
    }

    // 开始计时
    auto start = std::chrono::steady_clock::now();

    // 查找
    for (auto key : insertList) {
        cuckoo.get(key);
    }
    
    auto end = std::chrono::steady_clock::now();

    // 输出纳秒级别的时间差
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}


// 并行测试
long long int parallelGETTest(int threadNum,long long int capacity, long long int insertSize){
    if (capacity == 0 || insertSize == 0) {
        std::cout << "Invalid input" << std::endl;
        return -1;
    }

    if (insertSize > capacity) {
        std::cout << "Insert size should be smaller than capacity" << std::endl;
        return -1;
    }

    cuckoo::Cuckoo cuckoo(capacity);
    std::map<long long int, long long int> insertMap;
    std::vector<long long int> insertList;

    // 随机生成long long int的key
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long int> dis(1, capacity);

    // 生成插入的key
    for (int i = 0; i < insertSize; i++) {
        long long int key = dis(gen);

        while(insertMap.count(key) != 0){
            key = dis(gen);
        }

        insertMap[key] = key;
        insertList.emplace_back(key);
    }


    // 插入
    for (auto key : insertList) {
        cuckoo.put(key);
    }

    // 开始计时
    auto start = std::chrono::steady_clock::now();

    // 查找
    std::vector<std::thread> threads;
    for (int i = 0; i < threadNum; i++) {
        threads.emplace_back(std::thread([&cuckoo, &insertList, i, threadNum](){
            for (int j = i; j < insertList.size(); j += threadNum) {
                cuckoo.get(insertList[j]);
            }
        }));
    }

    for (auto &thread : threads) {
        thread.join();
    }

    // 结束计时
    auto end = std::chrono::steady_clock::now();

    // 输出纳秒级别的时间差
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}


int runtimes = 16;

long long int serializePUTTest_Wrap(long long int capacity, long long int insertSize){
    long long int sum = 0;
    for(int i = 0; i < runtimes; i++){
        sum += serializePUTTest(capacity, insertSize);
    }
    return sum / runtimes;
}

long long int parallelPUTTest_Wrap(int threadNum,long long int capacity, long long int insertSize){
    long long int sum = 0;
    for(int i = 0; i < runtimes; i++){
        sum += parallelPUTTest(threadNum, capacity, insertSize);
    }
    return sum / runtimes;

}

long long int serializeGETTest_Wrap(long long int capacity, long long int insertSize){
    long long int sum = 0;
    for(int i = 0; i < runtimes; i++){
        sum += serializeGETTest(capacity, insertSize);
    }
    return sum / runtimes;
}

long long int parallelGETTest_Wrap(int threadNum,long long int capacity, long long int insertSize){
    long long int sum = 0;
    for(int i = 0; i < runtimes; i++){
        sum += parallelGETTest(threadNum, capacity, insertSize);
    }
    return sum / runtimes;
}


int main(){
    int base = serializeGETTest_Wrap(100000, 10000);
    int a = parallelGETTest_Wrap(1, 100000, 10000);
    int b = parallelGETTest_Wrap(2, 100000, 10000);
    std::cout << base << std::endl;
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    

    // // 打开文件
    // std::ofstream outfile;
    // outfile.open("data-10000-GET.csv", std::ios::out | std::ios::trunc);
    // outfile << "threadNum,insertSize,time,rate" << std::endl;
    // long long int base = serializeGETTest_Wrap(100000, 10000);
    // for(int i = 1; i <= 32; i++){
    //     int result = parallelGETTest_Wrap(i, 100000, 10000);
    //     outfile << i << "," << 10000 << "," << result << "," << (double)result / base << std::endl;
    // }
    // outfile.close();


    // // 打开文件
    // outfile.open("data-50000-GET.csv", std::ios::out | std::ios::trunc);
    // outfile << "threadNum,insertSize,time,rate" << std::endl;
    // base = serializeGETTest_Wrap(100000, 50000);
    // for(int i = 1; i <= 32; i++){
    //     int result = parallelGETTest_Wrap(i, 100000, 50000);
    //     outfile << i << "," << 50000 << "," << result << "," << (double)result / base << std::endl;
    // }
    // outfile.close();


    // // 打开文件
    // outfile.open("data-10000-PUT.csv", std::ios::out | std::ios::trunc);
    // outfile << "threadNum,insertSize,time,rate" << std::endl;
    // base = serializePUTTest_Wrap(100000, 10000);
    // for(int i = 1; i <= 32; i++){
    //     int result = parallelPUTTest_Wrap(i, 100000, 10000);
    //     outfile << i << "," << 10000 << "," << result << "," << (double)result / base << std::endl;
    // }
    // outfile.close();


    // // 打开文件
    // outfile.open("data-50000-PUT.csv", std::ios::out | std::ios::trunc);
    // outfile << "threadNum,insertSize,time,rate" << std::endl;
    // base = serializePUTTest_Wrap(100000, 50000);
    // for(int i = 1; i <= 32; i++){
    //     int result = parallelPUTTest_Wrap(i, 100000, 50000);
    //     outfile << i << "," << 50000 << "," << result << "," << (double)result / base << std::endl;
    // }

    // outfile.close();
    return 0;
}