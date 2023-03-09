#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <bitset>
#include "MurmurHash3.h"

// 缓存策略 1代表不缓存、2、代表值缓存index、3、代表缓存BF过滤器
const int Cache_None = 1;
const int Cache_Only_Index = 2;
const int Cache_Index_BF = 3;


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



template<typename K, typename V>
class SStable{
private:
    // ********  meta data  ******** 
    // 缓存策略，包括三种 1代表不缓存、2、代表值缓存index、3、代表缓存BF过滤器
    int cacheStrategies;

    // 对应的sst文件大小。单位是Byte，当某一个操作超过2*1024*1024，需要发起相关操作
    uint64_t fileSize;

    // SStable对应当前的元素的数量
    size_t elemNum;


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
    std::vector<K> keyVec;
    std::vector<V> valVec;


public:
    SStable();
    SStable(int setCacheStrategies);
    ~SStable();

    // File related
    void readFile(std::string path);
    void saveToFile(std::string path);
    void readMemTable();
    
    // timeStramp
    time_t getSavedTime();
    void setSavedTime();

    // 
    u_int64_t getMinKey();
    u_int64_t getMaxKey();


    size_t getElemNum();



};
