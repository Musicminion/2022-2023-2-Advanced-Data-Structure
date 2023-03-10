#pragma once
#include <bitset>
#include <string>
#include <iostream>
#include <fstream>
#include "MurmurHash3.h"

// K 是键的类型，Size是BF过滤器占用的空间（单位是Byte）
template<typename K, size_t Size>
class BloomFliter{
private:
    // 用于存储的位图
    std::bitset<Size * 8> bloomfliterData;

public:
    // 插入元素
    void BFinsert(K key);
    // 检查元素是否存储
    bool BFfind(K key);

    // 读取文件，从文件获取数据（path是路径，offset是文件偏移量，单位是Byte）
    int BFreadFile(std::string path, uint32_t offset);
    // 写入文件，把数据写入到文件（path是路径，offset是文件偏移量，单位是Byte）
    uint32_t BFwriteToFile(std::string path, uint32_t offset);
    // 构造函数 析构函数类型
    BloomFliter(){}
    ~BloomFliter(){}
};


/**
 * 往BF过滤器里面插入元素
 * @param Key 要插入的目标Key
 */
template<typename K, size_t Size>
void BloomFliter<K,Size>::BFinsert(K key){
    // 如果已经创建，那就继续写入
    // 计算四个hash值
    unsigned int hash[4] = {0};
    MurmurHash3_x64_128(&key, sizeof(key), 1, hash);
    this->bloomfliterData.set(0,1);
    // 设置BF过滤器！
    for(int i = 0; i < 4; i++){
        this->bloomfliterData.set(hash[i] % (Size * 8) , true);
    }
}


/**
 * 根据BF过滤器，查找是否已经写入
 * @param key 要查找的目标Key
 * @return 是否存在在BF过滤器，存在返回 1 ，不存在返回 0
 */
template<typename K, size_t Size>
bool BloomFliter<K,Size>::BFfind(K key){
    unsigned int hash[4] = {0};
    MurmurHash3_x64_128(&key, sizeof(key), 1, hash);
    for(int i = 0; i < 4; i++){
        if(this->bloomfliterData.test(hash[i] % (Size * 8) ) == false)
            return false;
    }
    return true;
}

/**
 * 把位图的数据写入到文件里面
 * @param path 文件的路径
 * @param offset 要读取的偏移量
 * @return 0代表正常返回，-1代表文件不存在 -2代表范围异常
 */
template<typename K, size_t Size>
int BloomFliter<K,Size>::BFreadFile(std::string path, uint32_t offset){
    std::ifstream inFile(path, std::ios::in|std::ios::binary);

    if (!inFile)
        return -1;

    // 文件指针移动到末尾
    inFile.seekg(0,std::ios::end);
    uint32_t fileLimit = inFile.tellg();

    // 判断是否超过限度
    if(offset > fileLimit || Size + offset > fileLimit){
        inFile.close();
        return -2;
    }
        
    inFile.clear();

    // 通过检查，文件指针移动到偏移量
    inFile.seekg(offset,std::ios::beg);

    inFile.read((char*)&bloomfliterData, Size);
    inFile.close();
    return 0;
}


/**
 * 把位图的数据写入到文件里面
 * @param path 文件的路径
 * @param offset 要写入的偏移量
 * @return 返回文件写入的偏移量（Byte）
 */
template<typename K, size_t Size>
uint32_t BloomFliter<K,Size>::BFwriteToFile(std::string path, uint32_t offset){
    // 判断文件是否存在，用尝试读取的方式打开，如果打开成功就认为文件存在
    bool ifFileExist = false;
    uint32_t fileLimit = 0;
    std::ifstream inFile(path, std::ios::in|std::ios::binary);

    if (inFile){
        // 打开成功，获取文件大小
        inFile.seekg(0,std::ios::end);
        fileLimit = inFile.tellg();
        inFile.close();
        ifFileExist = true;
    }

    // 文件不存在
    if(!ifFileExist){
        offset = 0;
        // 创建文件
        std::fstream createFile(path, std::ios::out | std::ios::binary);
        createFile.close();
    }
    // 文件存在且用户设置的偏移量大于文件最大的范围，以追加形式写入文件
    else if(ifFileExist && (offset > fileLimit)){
        offset = fileLimit;
    }

    std::ofstream outFile(path, std::ios::out | std::ios::in | std::ios::binary);
    
    if (!outFile)
        return -1;
    
    
    // 文件指针移动到偏移量
    outFile.seekp(offset,std::ios::beg);

    // 开始写入文件
    outFile.write((char*)&bloomfliterData, Size);
    outFile.close();

    return offset;
}
