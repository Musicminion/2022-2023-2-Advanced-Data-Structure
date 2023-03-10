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

    SSTheader(){};
    ~SSTheader(){};

    // 从某个文件开始读取Header，读取的大小固定的32Byte
    int readFile(std::string path, uint32_t offset);

    // 向某个文件里面写入Header，写入的大小为固定的32Byte
    int writeToFile(std::string path, uint32_t offset);
};


/**
 * 从文件读取Header数据
 * @param path 文件的路径
 * @param offset 要读取的偏移量，默认为9
 * @return 0代表正常返回，-1代表文件不存在 -2代表范围异常
 * 使用样例：readFile("./data", 0);
 */
int SSTheader::readFile(std::string path, uint32_t offset){
    std::ifstream inFile(path, std::ios::in|std::ios::binary);
    if (!inFile)
        return -1;
    
    // 文件指针移动到末尾
    inFile.seekg(0,std::ios::end);

    uint32_t fileLimit = inFile.tellg();

    // 判断是否超过限度
    if(offset > fileLimit){
        return -2;
        inFile.close();
    }
    
    // 通过检查，文件指针移动到偏移量
    inFile.seekg(offset,std::ios::beg);

    // 读取文件
    inFile.read((char*)&timeStamp, sizeof(timeStamp));
    inFile.read((char*)&keyValNum, sizeof(keyValNum));
    inFile.read((char*)&minKey, sizeof(minKey));
    inFile.read((char*)&maxKey, sizeof(maxKey));

    inFile.close();
    return 0;
}


/**
 * 把Header数据写入到文件里面
 * @param path 文件的路径
 * @param offset 要写入的偏移量，默认为9
 * @return 0代表正常返回，-1代表文件不存在 -2代表范围异常
 * 使用样例：readFile("./data", 0);
 */
int SSTheader::writeToFile(std::string path, uint32_t offset){
    std::fstream outFile(path, std::ios::out| std::ios::in | std::ios::binary);
    
    if (!outFile)
        return -1;

    // 文件指针移动到末尾
    outFile.seekg(0,std::ios::end);
    uint32_t fileLimit = outFile.tellg();
    std::cout << "file limit" << fileLimit << std::endl;
    // 判断是否超过限度
    if(offset > fileLimit){
        outFile.close();
        return -2;
    }
        
    // 通过检查，文件指针移动到偏移量
    outFile.seekp(offset,std::ios::beg);

    // 写入文件
    outFile.write((char*)&timeStamp, sizeof(timeStamp));
    outFile.write((char*)&keyValNum, sizeof(keyValNum));
    outFile.write((char*)&minKey, sizeof(minKey));
    outFile.write((char*)&maxKey, sizeof(maxKey));

    outFile.close();
    return 0;
}