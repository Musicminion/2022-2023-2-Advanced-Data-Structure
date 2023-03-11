#include "sstable.h"


SStable::SStable(std::string setPath, bool setCachePolicy[4]){
    this->path = setPath;
    this->refreshCachePolicy(setCachePolicy);
}

/**
 *  从给定的list读取数据，然后写入到目标文件
 *  @param setTimeStamp 时间戳
 *  @param list 所有的key-value数据
 *  @param setPath 设置的目标路径
 *  @param setCachePolicy 设置的缓存策略
 *  补充：函数很复杂，但是也是目前能想出的最优解，必须保证在构造函数结束后有文件在硬盘上
*/

SStable::SStable(
    uint64_t setTimeStamp,
    std::list <std::pair<uint64_t, std::string> > &list,
    std::string setPath, bool setCachePolicy[4]){

    this->path = setPath;
    // 先处理BF过滤器
    this->bloomFliter = new BloomFliter<uint64_t, sstable_bfSize>();
    this->index = new SSTindex();
    this->value = new SSTvalue();
    this->header = new SSTheader();

    // 变量记录最大最小值
    uint64_t curMinKey = list.begin()->first;
    uint64_t curMaxKey = list.begin()->first;

    // 初始化value偏移量起点
    uint32_t curValueOffset = sstable_fileOffset_key + list.size() * (sstable_keySize + sstable_keyOffset);
    
    for(auto iter = list.begin(); iter != list.end(); iter++){
        // 更新min、max
        curMinKey = std::min(curMinKey, iter->first);
        curMaxKey = std::max(curMaxKey, iter->first);

        // 插入到BF、index、value
        bloomFliter->insert(iter->first);
        index->insert(iter->first, curValueOffset);
        value->insert(iter->second);

        // 移动当前的偏移量
        curValueOffset += iter->second.size();
    }

    // 把value变量再移动回来，待会用
    curValueOffset = sstable_fileOffset_key + list.size() * (sstable_keySize + sstable_keyOffset);

    header->timeStamp = setTimeStamp;
    header->minKey = curMinKey;
    header->maxKey = curMaxKey;
    header->keyValNum = list.size();

    // 完成写入
    header->writeToFile(setPath, sstable_fileOffset_header);
    bloomFliter->writeToFile(setPath, sstable_fileOffset_bf);
    index->writeToFile(setPath, sstable_fileOffset_key);
    value->writeToFile(setPath, curValueOffset);

    this->refreshCachePolicy(setCachePolicy);
}



SStable::~SStable(){
    // 删除所有的指针
    bool policy[4] = {false, false ,false, false};
    refreshCachePolicy(policy);
}

/**
 * 刷新缓存策略
 * @param 缓存策略 0-3代表header、BF、Index、value
*/
void SStable::refreshCachePolicy(bool setCachePolicy[4]){
    // ************ Header ************
    if(setCachePolicy[0] == false){
        if(this->header != NULL){
            delete this->header;
            this->header = NULL;
        }
        this->cachePolicy[0] = false;
    }
    else
        getHeaderPtr();
    
    // ************ BF ************
    if(setCachePolicy[1] == false){
        if(this->bloomFliter != NULL){
            delete this->bloomFliter;
            this->bloomFliter = NULL;
        }
            
    }        
    else
        getBloomFliterPtr();
    // ************ index ************
    if(setCachePolicy[2] == false){
        if(this->index != NULL){
            delete this->index;
            this->index = NULL;
        }
    }
    else
        getIndexPtr();

    // ************ value ************
    if(setCachePolicy[3] == false){
        if(this->value != NULL){
            delete this->value;
            this->value = NULL;
        }
    }
    else
        getValuePtr();
}

/**
 * 获取到当前的SST文件的header指针
 * 注意：如果当前的缓存策略是不缓存header，但是通过调用了这个函数
 * 那么缓存策略的对应位置cachePolicy[0] 会被修改
*/
SSTheader * SStable::getHeaderPtr(){
    this->cachePolicy[0] = true;
    if(this->header != NULL)
        return this->header;
    this->header = new SSTheader(this->path, sstable_fileOffset_header);
    
    return this->header;  
}

/**
 * 获取到当前的SST文件的BF过滤器指针
 * 同理和Header部分
*/
BloomFliter<uint64_t, sstable_bfSize > * SStable::getBloomFliterPtr(){
    this->cachePolicy[1] = true;
    if(this->bloomFliter != NULL)
        return this->bloomFliter;
    this->bloomFliter = new BloomFliter<uint64_t, sstable_bfSize >(this->path, sstable_fileOffset_bf);
    
    return this->bloomFliter;
}


/**
 * 获取索引的指针
*/
SSTindex * SStable::getIndexPtr(){
    this->cachePolicy[2] = true;
    if(this->index != NULL)
        return this->index;
    uint64_t kvNum = this->getHeaderPtr()->keyValNum;
    this->index = new SSTindex(this->path, sstable_fileOffset_key, kvNum);
    
    return this->index;
}

/**
 * 获取value的操作指针
*/
SSTvalue * SStable::getValuePtr(){
    this->cachePolicy[3] = true;
    if(this->value != NULL)
        return this->value;
    this->value = new SSTvalue();
    return this->value;
}




void SStable::devTest(){
    std::cout << this->getHeaderPtr()->keyValNum << std::endl; 
    std::cout << this->getHeaderPtr()->maxKey << std::endl; 
    std::cout << this->getHeaderPtr()->minKey << std::endl; 
    std::cout << this->getHeaderPtr()->timeStamp << std::endl; 

    std::cout << this->getBloomFliterPtr()->find(667) << std::endl;
    std::cout << this->getBloomFliterPtr()->find(777) << std::endl;
    std::cout << this->getBloomFliterPtr()->find(55) << std::endl;
    std::cout << this->getBloomFliterPtr()->find(44) << std::endl;
    

    SSTindex * tmp = this->getIndexPtr();
    size_t num = tmp->getKeyNum();

    for (size_t i = 0; i < num; i++)
    {
        std::cout << tmp->getKey(i) << " " << tmp->getOffset(i) - sstable_fileOffset_key << "\n";
    }
    
    
}
