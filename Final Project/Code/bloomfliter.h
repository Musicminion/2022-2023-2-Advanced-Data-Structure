#pragma once
#include <bitset>
#include "MurmurHash3.h"

// K 是键的类型，Size是BF过滤器占用的空间（单位是bit）
template<typename K, size_t Size>
class BloomFliter{
private:
    // 用于存储的位图
    std::bitset<Size> bloomfliterData;

public:
    // 插入元素
    // void BFinsert(K key){
    //     this->bloomfliterData.set(0,1);
    // }
    void BFinsert(K key);
    // 检查元素是否存储
    bool BFfind(K key);

    // 构造函数类型
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
        this->bloomfliterData.set(hash[i] % Size , true);
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
        if(this->bloomfliterData.test(hash[i] % Size ) == false)
            return false;
    }
    return true;
}