#pragma once
#include <string>
#include <iostream>
#include <fstream>

// SSTheader是SStable里面的header部分
class SSTheader
{
public:
    uint64_t timeStamp;  // 时间戳的序列号
    uint64_t keyValNum;  // 键值对的数量
    uint64_t minKey;     // 最小的键
    uint64_t maxKey;     // 最大的键

    // 从某个文件开始读取Header，读取的大小固定的32Byte
    int readFile(std::string path, uint32_t offset);

    // 向某个文件里面写入Header，写入的大小为固定的32Byte
    uint32_t writeToFile(std::string path, uint32_t offset);

    SSTheader(){}
    ~SSTheader(){}
};
