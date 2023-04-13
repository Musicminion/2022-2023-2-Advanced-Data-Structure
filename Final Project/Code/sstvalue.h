#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "config.h"

// SST的数据区域 
// 这里只提供读取某一个特定Value String的 API 因为没有需求要把sst文件的所有val读取
class SSTvalue{
private:
    size_t valNum;
    std::vector<std::string> valVec;

public:
    size_t getValNum();
    std::string getVal(size_t index);

    void insert(std::string newVal);

    uint32_t writeToFile(std::string path, uint32_t offset);
    std::string getValFromFile(std::string path, uint32_t offset, size_t length);

    SSTvalue(){ valNum = 0; }
    ~SSTvalue(){}
};

