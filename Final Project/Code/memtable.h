#pragma once

#include "skiplist.h"

template<typename K, typename V>
class MemTable
{
private:
    Skiplist<K, V>* skiplist;

public:
    MemTable();
    ~MemTable();

    void put(K key, const V &s);

    bool del(K key);

    V get(K key);

    void reset();

    void scan(K key1, K key2, std::list<std::pair<K, V> > &list);
};


template<typename K, typename V>
MemTable<K,V>::MemTable()
{
    skiplist = new Skiplist<uint64_t, std::string>();
}

template<typename K, typename V>
MemTable<K,V>::~MemTable()
{
    delete skiplist;
}
template<typename K, typename V>
void MemTable<K,V>::put(K key, const V &s){
    //std::cout << "put :" << key << ", value" << '\n';
    this->skiplist->insertNode(key, s);
}
template<typename K, typename V>
bool MemTable<K,V>::del(K key){
    //std::cout << "del :" << key << '\n';
    if(this->skiplist->findNode(key) == NULL)
        return false;
    this->skiplist->deleteNode(key);
    return true;
}
template<typename K, typename V>
V MemTable<K,V>::get(K key){
    //std::cout << "get :" << key << '\n';
    auto tryFindNode = this->skiplist->findNode(key);
    if(tryFindNode != NULL){
        std::string result = tryFindNode->val;
        return result;
    }
    return V();
}
template<typename K, typename V>
void MemTable<K,V>::reset(){
    this->skiplist->clear();
}

template<typename K, typename V>
void MemTable<K,V>::scan(K key1, K key2, std::list<std::pair<K, V> > &list){
    Node<K, V>* iter = this->skiplist->findNode(key1);
    if(iter->key == key1){
        list.push_back({iter->key,iter->val});
    }
    iter = iter->next[0];
    while (iter->type == nodeType_Data && iter->key <= key2)
    {
        list.push_back({iter->key,iter->val});
        iter = iter->next[0];
    }
}