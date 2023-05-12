#pragma once

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <vector>

// 一旦踢皮球的次数超过这个值，就认为出现了死循环，直接rehash
// 有哪个程序员会拒绝2的整数幂的诱惑呢？
const int Limit = 128;

namespace cuckoo{
    typedef int KeyType;
    class Cuckoo {
        protected:
            std::mutex mtx;
            long long int cuckooSize = 50;

            // 课程中的代码用的事写死的数组，这里用vector
            // 因为涉及到Rehash，所以不能用固定的数组
            // KeyType T[SIZE];
            std::vector<KeyType> T;
            
            // hash key by hash func1
            int hash1(const KeyType &key);
            // hash key by hash func2
            int hash2(const KeyType &key);
            // find key by hash func1 in T
            KeyType get1(const KeyType &key);
            // find key by hash func2 in T
            KeyType get2(const KeyType &key);

            void bt_evict(const KeyType &key, int which, int pre_pos);
        public:
            Cuckoo();
            Cuckoo(long long int initSize);
            ~Cuckoo();
            // insert key into T
            void put(const KeyType &key);

            // find key in T
            KeyType get(const KeyType &key);

            // rehash
            void rehash(int evicted);

            // reset
            void reset();

    };
};