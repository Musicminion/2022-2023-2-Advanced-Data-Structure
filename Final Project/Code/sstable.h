#pragma once

#include "sstheader.h"
#include "bloomfliter.h"
#include "bloomfliter.h"
#include "sstindex.h"
#include "sstvalue.h"
#include "config.h"
#include <list>
#include <map>

class SStable{
private:
    // meta元数据
    std::string path;           // 文件的存储路径
    uint32_t fileSize;          // 文件的大小（单位Byte）
    bool cachePolicy[4];        // 各组成的缓存状态

    // SSTable由四部分组成，header、BF、索引、数据区域
    // 【警告】这些指针请不要直接调用，更不要修改，需要通过getXXXPtr访问
    // 因为getXXXPtr做了慎重的考虑封装而来，会保证缓存策略的一致性，防止内存泄漏
    SSTheader * header = NULL;
    BloomFliter<uint64_t, sstable_bfSize > * bloomFliter = NULL;
    SSTindex * index  = NULL;
    SSTvalue * value  = NULL;

    // 内部提供获取相关header指针、bf指针、index指针、value指针
    // 通过函数获取指针，最大的好处就是保证不需要手动创建，哪怕当前没有header
    // 由于某个原因需要读取，getHeaderPtr会自动修改缓存策略
    SSTheader * getHeaderPtr();
    BloomFliter<uint64_t, sstable_bfSize >* getBloomFliterPtr();
    SSTindex * getIndexPtr();
    SSTvalue * getValuePtr();

    // 在某些操作可能胡已修改缓存策略前，保存当前的缓存策略
    void saveCachePolicy(bool (&saveTarget)[4]);

public:

    void refreshCachePolicy(bool setCachePolicy[4]);
    
    // 初始化一个SStable, 从文件里面读取，并设置常态化缓存策略
    SStable(std::string path, bool cachePolicy[4]);

    // 初始化一个SStable，从list读数据写入，完成之后立即落硬盘
    // 文件写完之后，按照缓存策略，删除不必要的元素。
    SStable(uint64_t setTimeStamp, 
        std::list <std::pair<uint64_t, std::string> > &list,
        std::string setPath, bool cachePolicy[4]);
    

    // 开发测试函数
    void devTest();

    // 清空sstable
    void clear();

    // 变量读取写在这里面
    uint64_t getSStableTimeStamp();
    uint64_t getSStableMinKey();
    uint64_t getSStableMaxKey();
    uint64_t getSStableKeyValNum();

    uint64_t getSStableKey(size_t index);
    uint32_t getSStableKeyOffset(size_t index);
    uint32_t getKeyIndexByKey(uint64_t key);
    uint32_t getKeyOrLargerIndexByKey(uint64_t key);
    std::string getSStableValue(size_t index);


    bool checkIfKeyExist(uint64_t targetKey);

    void scan(uint64_t key1, uint64_t key2, std::map<uint64_t, std::map<uint64_t, std::string> >  &scanMap);


    SStable();
    ~SStable();
};


