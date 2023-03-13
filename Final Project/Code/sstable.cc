#include "sstable.h"
#include "utils.h"


SStable::SStable(std::string setPath, bool setCachePolicy[4]){
    this->path = setPath;
    // 初始化的事情必须把这个指针指向NULL，因为后面判断会用到！
    this->bloomFliter = NULL;
    this->index = NULL;
    this->value = NULL;
    this->header = NULL;

    // 刷新缓存策略本质上会读文件
    this->refreshCachePolicy(setCachePolicy);
    
    // 再亲自去读取一遍获取大小
    std::ifstream inFile(setPath, std::ios::in|std::ios::binary);
    
    if (inFile){
         // 文件指针移动到末尾
        inFile.seekg(0,std::ios::end);
        this->fileSize = inFile.tellg();
        inFile.close();
    }
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
    uint64_t curMinKey = UINT64_MAX;
    uint64_t curMaxKey = 0;

    // 初始化value偏移量起点
    uint32_t curValueOffset = sstable_fileOffset_key + list.size() * (sstable_keySize + sstable_keyOffsetSize);

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
    curValueOffset = sstable_fileOffset_key + list.size() * (sstable_keySize + sstable_keyOffsetSize);

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


    // 再亲自去读取一遍获取文件大小！
    std::ifstream inFile(setPath, std::ios::in|std::ios::binary);
    
    if (inFile){
         // 文件指针移动到末尾
        inFile.seekg(0,std::ios::end);
        this->fileSize = inFile.tellg();
        inFile.close();
    }
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
    if(this->header != NULL){
        return this->header;
    }
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
    uint64_t kvNum = (this->getHeaderPtr())->keyValNum;
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

/**
 * 清空所有的数据、并且删除文件！危险操作！
*/
 void SStable::clear(){
    // 删除文件
    utils::rmfile(this->path.c_str());
    // // 删除所有指针
    // bool clearPolicy[4] = {false, false ,false, false};
    // refreshCachePolicy(clearPolicy);
 }

/**
 * 获取sst的kvstore时间戳
*/
uint64_t SStable::getSStableTimeStamp(){
    return this->getHeaderPtr()->timeStamp;
}

/**
 * 获取header里面的最小值 
 */
uint64_t SStable::getSStableMinKey(){
    return this->getHeaderPtr()->minKey;
}

/**
 * 获取header里面的最大值
 */
uint64_t SStable::getSStableMaxKey(){
    return this->getHeaderPtr()->maxKey;
}

/**
 * 获取header里面 key-val的数量
*/
uint64_t SStable::getSStableKeyValNum(){
    return this->getHeaderPtr()->keyValNum;
}


/**
 * 获取sst的第i个key
*/
uint64_t SStable::getSStableKey(size_t index){
    if(index >= this->getHeaderPtr()->keyValNum)
        return UINT64_MAX;
    return this->getIndexPtr()->getKey(index);
}

/**
 * 获取key对应的的偏移量
*/
uint32_t SStable::getSStableKeyOffset(size_t index){
    if(index >= this->getHeaderPtr()->keyValNum)
        return UINT32_MAX;
    return this->getIndexPtr()->getOffset(index);
}

/**
 * 获取value的值 index is 0-base
*/
std::string SStable::getSStableValue(size_t index){
    
    uint64_t KVnum = this->getHeaderPtr()->keyValNum;

    if(index >= KVnum)
        return sstable_outOfRange;
    
    // 恰好是最后一个元素
    uint32_t offset = this->getIndexPtr()->getOffset(index);
    if(index + 1 == KVnum){
        return this->getValuePtr()->getValFromFile(this->path, 
            offset, this->fileSize - offset);
    }
    // 如果不是最后一个元素，那就可以通过两个元素的偏移量之差来计算读取的长度

    uint32_t offsetNext = this->getIndexPtr()->getOffset(index + 1);
   
    
    return this->getValuePtr()->getValFromFile(this->path, 
        offset, offsetNext - offset);
}


/**
 * 检查一个key是否存在在sstable里面
*/
bool SStable::checkIfExist(uint64_t targetKey){
    return true;
    if(targetKey > getHeaderPtr()->maxKey || targetKey < getHeaderPtr()->minKey)
        return false;
    if(this->cachePolicy[1] == true){
        return getBloomFliterPtr()->find(targetKey);
    }
    return true;
}

/**
 * 通过二分法查找某一个key是否存在
 * @param key 传递的搜索目标key
 * @return 不存在就返回UINT32_MAX,否则返回偏移量
*/
uint32_t SStable::getKeyIndexByKey(uint64_t key){
    return getIndexPtr()->getKeyIndexByKey(key);
}


/**
 * 开发使用的函数，输出部分调试信息
*/
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

