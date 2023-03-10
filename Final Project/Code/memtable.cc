#include "memtable.h"

MemTable::MemTable()
{
    skiplist = new Skiplist<uint64_t, std::string>();
}


MemTable::~MemTable()
{
    delete skiplist;
}


void MemTable::put(uint64_t key, const std::string &s){
    //std::cout << "put :" << key << ", value" << '\n';
    this->skiplist->insertNode(key, s);
}



bool MemTable::del(uint64_t key){
    //std::cout << "del :" << key << '\n';
    if(this->skiplist->findNode(key) == NULL)
        return false;
    this->skiplist->deleteNode(key);
    return true;
}


std::string MemTable::get(uint64_t key){
    //std::cout << "get :" << key << '\n';
    auto tryFindNode = this->skiplist->findNode(key);
    if(tryFindNode != NULL){
        std::string result = tryFindNode->val;
        return result;
    }
    return "";
}


void MemTable::reset(){
    this->skiplist->clear();
}


void MemTable::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string> > &list){
    Node<uint64_t, std::string>* iter = this->skiplist->findNode(key1);
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