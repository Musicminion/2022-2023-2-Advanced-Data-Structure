#include "sstable.h"

SStable::~SStable(){
    // 删除所有的指针
    bool policy[4] = {false, false ,false, false};
    refreshCachePolicy(policy);
}

/**
 * 初始化一个SStable
 * @param path 指定文件的路径
 * @param cachePolicy 指定缓存策略
 * @param ifReadFile 是否立即读文件
*/
SStable::SStable(std::string setPath, bool setCachePolicy[4], bool ifReadFile){
    // 初始化元数据
    this->path = setPath;
    for(int i = 0; i < 4; i++){
        this->cachePolicy[i] = setCachePolicy[i]; 
    }

    if(ifReadFile)
        readFile(path);
}

/**
 * 读取一个SStable文件
*/
int SStable::readFile(std::string setPath){
    // 更新一次路径
    this->path = setPath;

    // 判断是否读取Header 必须要读取header部分，否则后面的无法处理
    if(!cachePolicy[0])
        return -1;
    else
        getHeaderPtr();
    
    // 是否读取BF过滤器部分
    if(cachePolicy[1])
        getBloomFliterPtr();

    // 是否读取索引
    if(cachePolicy[2])
        getIndexPtr();
    
    // 是否读取Value,其实value里面也不存储数据，只是一个数据操作对象
    if(cachePolicy[3])
        getValuePtr();

    return 0;
}

/**
 * 获取到当前的SST文件的header指针
 * 注意：如果当前的缓存策略是不缓存header，但是通过调用了这个函数
 * 那么缓存策略的对应位置cachePolicy[0] 会被修改
*/
SSTheader * SStable::getHeaderPtr(){
    if(this->header != NULL)
        return this->header;
    // 创建header对象，
    this->header = new SSTheader(this->path, sstable_fileOffset_header);
    this->cachePolicy[0] = true;
    return this->header;
}


/**
 * 获取到当前的SST文件的BF过滤器指针
 * 同理和Header部分
*/
BloomFliter<uint64_t, sstable_bfSize > * SStable::getBloomFliterPtr(){
    if(this->bloomFliter != NULL)
        return this->bloomFliter;
    this->bloomFliter = new BloomFliter<uint64_t, sstable_bfSize >(this->path, sstable_fileOffset_bf);
    this->cachePolicy[1] = true;
    return this->bloomFliter;
}


/**
 * 获取索引的指针
*/
SSTindex * SStable::getIndexPtr(){
    if(this->index != NULL)
        return this->index;
    uint64_t kvNum = this->getHeaderPtr()->keyValNum;
    this->index = new SSTindex(this->path, sstable_fileOffset_key, kvNum);
    return this->index;
}

/**
 * 刷新缓存策略
 * @param 缓存策略 0-3代表header、BF、Index、value
*/
void SStable::refreshCachePolicy(bool setCachePolicy[4]){
    // Header
    if(setCachePolicy[0] == false)
        delete this->header;
    else
        getHeaderPtr();
    
    // BF
    if(setCachePolicy[1] == false)
        delete this->bloomFliter;
    else
        getBloomFliterPtr();
    
    // index
    if(setCachePolicy[2] == false)
        delete this->index;
    else
        getIndexPtr();

    // value
    if(setCachePolicy[3] == false)
        delete this->value;
    else
        getValuePtr();
}