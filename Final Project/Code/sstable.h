#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <bitset>
#include "MurmurHash3.h"

// 最大只能是 2MB
const uint64_t maxTableSize = 2 * 1024 * 1024;

const uint64_t byte_size_header = 32;
const uint64_t byte_size_bloomfliter = 10240;

const uint64_t bit_size_header = byte_size_header * 8;
const uint64_t bit_size_bloomfliter = byte_size_bloomfliter * 8;

const uint64_t byte_size_key = 8;
const uint64_t bit_size_key = byte_size_key * 8;

const uint64_t byte_size_offset = 4;
const uint64_t bit_size_offset = byte_size_offset * 8;

const uint64_t byte_size_key_and_offset = byte_size_key + byte_size_offset;
const uint64_t bit_size_key_and_offset = byte_size_key_and_offset * 8;


// 第i个元素(0-base) key_i的位置
// [ byte_size_bloomfliter + byte_size_header + (key_i) * byte_size_key_and_offset, 
// byte_size_bloomfliter + byte_size_header + (key_i + 1) * byte_size_key_and_offset)




class SStable{
private:
    // ********  meta data  ******** 
    // 对应的sst文件大小。单位是Byte，当某一个操作超过2*1024*1024，需要发起相关操作
    uint64_t fileSize;
    // 对应的文件的大小
    size_t size;

    // ********  Header 字段 ********  
    // 保存的时间戳 8 Byte
    uint64_t savedTime;
    // 最大最小值 8 Byte + 8 Byte
    uint64_t minKey;
    uint64_t maxKey;
    // 表的数据的个数，以多少个key来反映 8 Byte
    uint64_t dataSize;

    // ********  bloomfliter 字段 ********
    std::bitset<bit_size_bloomfliter> bloomfliter;

    // ********  data 字段 ********
    std::vector<int> key;
    std::vector<int> offset;
    std::vector<int> data;

public:
    SStable();
    ~SStable();

    // File related
    void readFile(std::string path);
    void saveToFile(std::string path);
    
    // getter function
    time_t getSavedTime();

    unsigned long long int getTableSize();

    unsigned long long int getMinKey();

    unsigned long long int getMaxKey();

    unsigned long long int getDataSize();


    void setSavedTime();

};
