#pragma once

#include "sstheader.h"
#include "bloomfliter.h"
#include "bloomfliter.h"
#include "sstindex.h"
#include "sstvalue.h"
#include "config.h"

class SStable{
private:
    // meta元数据
    std::string path;       // 文件的存储路径
    bool cachePolicy[4];    // 各组成的缓存状态
    
    // SSTable由四部分组成，header、BF、索引、数据区域
    // 【警告】这些指针请不要直接调用，更不要修改，需要通过getXXXPtr访问
    // 因为getXXXPtr做了慎重的考虑封装而来，会保证缓存策略的一致性，防止内存泄漏
    SSTheader * header;
    BloomFliter<uint64_t, sstable_bfSize > * bloomFliter;
    SSTindex * index;
    SSTvalue * value;

    // 内部提供获取相关header指针、bf指针、index指针、value指针
    SSTheader * getHeaderPtr();
    BloomFliter<uint64_t, sstable_bfSize > * getBloomFliterPtr();
    SSTindex * getIndexPtr();
    SSTvalue * getValuePtr();


public:

    // 文件读写，根据缓存策略读取文件
    // 缓存策略 header必读！BF过滤器可选！index可选读，value永远不读。
    int readFile(std::string path);
    int writeToFile(std::string path);
    int deleteFile();


    void refreshCachePolicy(bool setCachePolicy[4]);
    
    // 初始化一个SStable, 指定参数, 用户可以选择是否读取文件
    SStable(std::string path, bool cachePolicy[4], bool ifReadFile);

    SStable(){}
    ~SStable(){}
};

