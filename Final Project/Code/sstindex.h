#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

// SST 的索引区域
class SSTindex
{
private:
    size_t keyNum;
    std::vector<uint64_t> keyVec;
    std::vector<uint32_t> offsetVec;

public:
    // 增、查函数
    size_t getKeyNum();
    uint64_t getKey(size_t index);
    uint32_t getOffset(size_t index);

    void insert(uint64_t newKey, uint32_t valOffset);

    // 文件相关
    int readFile(std::string path, uint32_t offset,size_t readKeyNum);

    uint32_t writeToFile(std::string path, uint32_t offset);

    SSTindex(){ keyNum = 0;}
    ~SSTindex(){}

    SSTindex(std::string path, uint32_t offset,size_t readKeyNum);
};

