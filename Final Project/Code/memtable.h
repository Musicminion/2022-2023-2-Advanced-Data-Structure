#pragma once
#include "skiplist.h"
#include "utils.h"
#include "config.h"


class MemTable
{
private:
    // 维护跳表
    Skiplist<uint64_t, std::string>* skiplist;
    // 维护转换到sstable的大小（单位Byte）
    size_t sstSpaceSize;

    // 增加、删除的内联函数，不记录日志。恢复的时候用
    // 对外暴露的接口都会记录日志！
    void putKV(uint64_t key, const std::string &s);
    bool delKV(uint64_t key);

    // 日志操作
    void writeLog(std::string path, int operationID, uint64_t key, std::string val);
    void restoreFromLog(std::string path);

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

