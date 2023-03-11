#pragma once

#include "sstheader.h"
#include "bloomfliter.h"
#include "bloomfliter.h"
#include "sstindex.h"
#include "sstvalue.h"
#include "config.h"
#include <list>


class SStable{
private:
    // meta元数据
    std::string path;           // 文件的存储路径
    bool cachePolicy[4];        // 各组成的缓存状态
    bool ifFileExist = false;    // 是否sst文件已经存在
    

    // SSTable由四部分组成，header、BF、索引、数据区域
    // 【警告】这些指针请不要直接调用，更不要修改，需要通过getXXXPtr访问
    // 因为getXXXPtr做了慎重的考虑封装而来，会保证缓存策略的一致性，防止内存泄漏
    SSTheader * header;
    BloomFliter<uint64_t, sstable_bfSize > * bloomFliter;
    SSTindex * index;
    SSTvalue * value;

    // 内部提供获取相关header指针、bf指针、index指针、value指针
    // 通过函数获取指针，最大的好处就是保证不需要手动创建，哪怕当前没有header
    // 由于某个原因需要读取，getHeaderPtr会自动修改缓存策略
    SSTheader * getHeaderPtr();
    BloomFliter<uint64_t, sstable_bfSize > * getBloomFliterPtr();
    SSTindex * getIndexPtr();
    SSTvalue * getValuePtr();

    


public:

    void refreshCachePolicy(bool setCachePolicy[4]);
    
    // 初始化一个SStable, 从文件里面读取，并设置常态化缓存策略
    SStable(std::string path, bool cachePolicy[4]);

    // 初始化一个SStable，从list读数据写入，完成之后立即落硬盘
    // 文件写完之后，按照缓存策略，删除不必要的元素。
    SStable(uint64_t setTimeStamp, 
        std::list <std::pair<uint64_t, std::string> > &list,
        std::string setPath, bool cachePolicy[4]);
    
    void devTest();

    SStable();
    ~SStable();
};












    // // 文件读写，本质是刷新缓存策略
    // // 缓存策略 header必读！BF过滤器可选！index可选读，value永远不读。
    // int readFile();
    // int readFile(std::string setPath);
    // int readFile(std::string setPath, bool setCachePolicy[4]);

    // int writeToFile();
    // int writeToFile(std::string setPath);
    // int writeToFile(std::string setPath, bool afterWriteCachePolicy[4]);
    
    // int deleteFile();