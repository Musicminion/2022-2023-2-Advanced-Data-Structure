#pragma once

#include "skiplist.h"

class MemTable
{
private:
    Skiplist<uint64_t, std::string>* skiplist;

public:
    MemTable();
    ~MemTable();

    void put(uint64_t key, const std::string &s);

    bool del(uint64_t key);

    std::string get(uint64_t key);

    void reset();

    void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string> > &list);
};

