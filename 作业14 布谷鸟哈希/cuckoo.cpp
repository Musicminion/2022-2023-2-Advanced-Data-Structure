#include "cuckoo.h"

namespace cuckoo {

// ========================================

/**
 * 默认的构造函数，大小为50，后面空间不够了再扩展
 */ 
Cuckoo::Cuckoo() {
    this->cuckooSize = 50;
    // memset(T, 0, sizeof(KeyType) * this->cuckooSize);
    T.resize(this->cuckooSize, 0);
}


/**
 * 按照大小初始化
*/
Cuckoo::Cuckoo(long long int initSize) {
    this->cuckooSize = initSize;
    T.resize(this->cuckooSize, 0);
}


Cuckoo::~Cuckoo() {
    // TODO
}

// ========================================
int Cuckoo::hash1(const KeyType &key) {
    assert(this->cuckooSize != 0);
    int half_siz = this->cuckooSize / 2;
    return key % half_siz;
}

int Cuckoo::hash2(const KeyType &key) {
    assert(this->cuckooSize != 0);
    int half_siz = this->cuckooSize / 2;
    return key/half_siz%half_siz + half_siz;
}

KeyType Cuckoo::get1(const KeyType &key) {
    return (T[hash1(key)] == key)? key : 0;
}

KeyType Cuckoo::get2(const KeyType &key) {
    return (T[hash2(key)] == key)? key : 0;
}

KeyType Cuckoo::get(const KeyType &key){
    // 0 is reserved for null, invalid input
    if (key == 0) {
        printf("Invalid key \n");
        return 0;
    }

    KeyType res = get1(key);
    
    if (res == 0) {
        res = get2(key);
    }

    return res;
}

// ========================================

template <typename T>
inline void swap(T* a, T* b) {
    assert(a != NULL && b != NULL);
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

void Cuckoo::put(const KeyType &key){
    if (key == 0) {
        printf("Invalid key \n");
        return;
    }

    if (get(key) != 0) {
        printf("Key already exists put fail \n");
        return;
    }

    // basic way 
    if (T[hash1(key)] == 0) {
        T[hash1(key)] = key;
    } else if (T[hash2(key)] == 0) {
        T[hash2(key)] = key;
    } else {
        std::unique_lock<std::mutex> lck(mtx);

        // two place for one certain key has been occupied, need evict others
        KeyType evicted = key;

        // wich is 0 or 1, 0 means evict key in hash1, 1 means evict key in hash2
        int which = 0;

        // first evict key in hash1
        int idx = hash1(evicted);

        int pre_pos = -1;

        int whileTimes = 0;
        // evict key in hash1
        while(T[idx] != 0) {
            // printf("evicted key %d from %d to %d\n", evicted, pre_pos, idx); 
            // 加锁mutex
            // std::unique_lock<std::mutex> lck(mtx);
            swap(&evicted, &T[idx]);
            // 解锁mutex
            // lck.unlock();

            pre_pos = idx;
            which = 1 - which;
            idx = (which == 0)?hash1(evicted):hash2(evicted);

            whileTimes++;
            if (whileTimes > Limit) {
                printf("dead loop, rehash ing\n");
                // 加锁mutex
                // std::unique_lock<std::mutex> lck(mtx);
                rehash(evicted);
                // 解锁mutex
                // lck.unlock();
                return;
            }
        }
        // printf("evicted key %d from %d to %d\n", evicted, pre_pos, idx);
        // 加锁mutex
        // std::unique_lock<std::mutex> lck(mtx);
        T[idx] = evicted;
        // // 解锁mutex
        // lck.unlock();
    }
}


// ========================================
void Cuckoo::rehash(int evicted) {
    // TODO
    std::vector <KeyType> oldT;
    
    // 从T中拷贝数据到oldT
    oldT.assign(T.begin(), T.end());

    // 重新分配空间
    this->cuckooSize *= 4;

    // 清空T
    T.clear();

    // resizeT
    T.resize(this->cuckooSize, 0);

    // 重新插入数据
    for (int i = 0; i < oldT.size(); i++) {
        if (oldT[i] != 0) {
            put(oldT[i]);
        }
    }

    // 重新插入evicted
    put(evicted);
}


// reset
void Cuckoo::reset() {
    T.clear();

}

};