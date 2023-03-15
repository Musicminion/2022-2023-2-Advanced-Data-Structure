#pragma once
#include "skiplist.h"
#include "config.h"

class MemTable
{
private:
    // 维护跳表
    Skiplist<uint64_t, std::string>* skiplist;
    // 维护转换到sstable的大小（单位Byte）
    size_t sstSpaceSize;


public:
    MemTable();
    ~MemTable();

    bool putCheck(uint64_t key, const std::string &s);

    void put(uint64_t key, const std::string &s);

    bool del(uint64_t key);

    std::string get(uint64_t key);

    void reset();

    void scan(uint64_t key1, uint64_t key2, std::map<uint64_t, std::string> &resultMap);

    // 拷贝所有的内容
    void copyAll(std::list<std::pair<uint64_t, std::string> > &list);

    void tranverse(){this->skiplist->tranverse();}
};

