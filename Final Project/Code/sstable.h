#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include "skiplist.h"
#include "MurmurHash3.h"


// 缓存策略 1代表不缓存、2、代表值缓存index、3、代表缓存BF过滤器
const int Cache_None = 1;
const int Cache_Only_Index = 2;
const int Cache_Index_BF = 3;

const int Read_All = 10;
const int ReadPart_Header = 20;
const int ReadPart_Index = 30;


// 最大只能是 2MB
const uint64_t maxTableSize = 2 * 1024 * 1024;

const uint64_t byte_size_header = 32;
const uint64_t byte_size_bloomfliter = 10240;

const uint64_t bit_size_header = byte_size_header * 8;
const uint64_t bit_size_bloomfliter = byte_size_bloomfliter * 8;

const uint64_t byte_size_key = 8;
const uint64_t bit_size_key = byte_size_key * 8;

const uint64_t byte_size_offset = 4;
const uint64_t bit_size_offset = byte_size_offset * 8;

const uint64_t byte_size_key_and_offset = byte_size_key + byte_size_offset;
const uint64_t bit_size_key_and_offset = byte_size_key_and_offset * 8;


// 第i个元素(0-base) key_i的位置
// [ byte_size_bloomfliter + byte_size_header + (key_i) * byte_size_key_and_offset, 
// byte_size_bloomfliter + byte_size_header + (key_i + 1) * byte_size_key_and_offset)


// 对象生命周期【场景一：创建对象并把sstable写入到文件】：
// 1.  创建对象（把时间戳、缓存策略一并写入。）
// 2.1 如果是创建空对象为了写入到文件，执行 insertElem 函数，往里面插入元素！
// 2.2 然后调用 saveToFile() 写入到文件里面。
// 2.3 写入完成之后，根据缓存策略，删除掉对应元素
//         Cache_None 清空：keyVec_key keyVec_offset valVec BF过滤器
//         Cache_Only_Index 清空：valVec BF过滤器
//         Cache_Index_BF 不需要清空，保留在内存。
// 3.0 进入正常业务流程（把自身的指针交给上层，提供服务）
// 3.1 GET：查找操作，根据缓存策略，查找元素
//         Cache_None 再次读取文件（读取策略Read_Full），读取完清空keyVec_key keyVec_offset valVec BF过滤器
//         Cache_Only_Index 在Index里面【二分】查找到Offset，然后再读取文件
//         Cache_Index_BF 通过BF判断是否存在，如果存在，然后在通过二分法查找


// 对象生命周期【场景二：kvstore初始化的时候读取文件】：
// 1.  创建对象，并写入缓存策略。
// 2.1 读取文件，调用readFile来读取文件【这里直接根据默认的情况读取】
// 2.2 读取文件的时候，根据缓存策略，选择不同的读取策略
//         Cache_None -> ReadPart_Header：读取meta信息（比如文件名称、elemNum、minKey、maxKey）
//         Cache_Only_Index -> ReadPart_Index：读取index信息（keyVec_key、keyVec_offset）
//         Cache_Index_BF -> Read_All：读取全部信息
// 3.0 进入正常业务流程（把自身的指针交给上层，提供服务）





template<typename K, typename V>
class SStable{
private:
    // ******** class meta data  ******** 这些都是类的元数据，
    // 缓存策略，包括三种 1代表不缓存、2、代表值缓存index、3、代表缓存BF过滤器
    int cacheStrategies;
    
    // 对应的sst文件路径，作为
    std::string filePath;

    // 对应的sst文件大小。单位是Byte，当某一个操作超过2*1024*1024，需要发起相关操作
    uint64_t fileSize;


    // ********  Header 字段 ********  
    // 保存的时间戳 8 Byte
    uint64_t savedTime;
    // 最大最小值 8 Byte + 8 Byte
    K minKey;
    K maxKey;
    // SStable对应当前的元素的数量
    uint64_t elemNum;

    // ********  bloomfliter 字段 ********
    std::bitset<bit_size_bloomfliter>* bloomfliter;

    // ********  data 字段 ********
    std::vector<K> keyVec_key;
    std::vector<uint32_t> keyVec_offset;
    std::vector<V> valVec;

    // 文件操作相关
    void readFile_ReadAll(std::string path);
    void readFile_HeaderOnly(std::string path);
    void readFile_Index(std::string path);
    void readFile_offset(std::string path, uint32_t offset, size_t readSize);
    
    // BF操作接口
    int BFfind(K setKey);
    int BFinsert(K setKey);
    void clearBF();


    // 数据操作接口
    void clearKeyVec();
    void clearValVec();
    
public:
    SStable();
    SStable(int setCacheStrategies);
    SStable(int setCacheStrategies, uint64_t timeStamp);
    ~SStable();

    // 不提供读取方式，根据默认的缓存策略读取
    int readFile(std::string path);
    // 指定读取方式，强制按照指定的读取方式读取
    int readFile(std::string path, int type);

    
    int saveToFile(std::string path);
    
    // timeStramp
    uint64_t getSavedTime();
    void setSavedTime(uint64_t setTime);

    // 数据操作接口
    K getMinKey();
    K getMaxKey();
    uint64_t getElemNum();

    // 从memTable读取，然后往SStable写数据的时候，调用这个函数
    void insertElem(K newkey, V newVal);

    // 需要对SSTabel执行查找操作的时候，调用这个函数。
    V findElem(K newkey, V newVal);
    
};

template<typename K, typename V>
SStable<K, V>::SStable(){
    this->elemNum = 0;
    this->fileSize = byte_size_header + byte_size_bloomfliter;
}

/**
 * SS table的构造函数
 * @param setCacheStrategies 缓存的策略设置。
 * @param timeStamp sstable生成的时间戳
 */

template<typename K, typename V>
SStable<K, V>::SStable(int setCacheStrategies, uint64_t timeStamp){
    // 如果传递了时间戳和缓存策略，就把对应的设置好
    this->cacheStrategies = setCacheStrategies;
    this->savedTime = timeStamp;
    this->elemNum = 0;

    // 不管采用哪一种缓存策略，文件的大小都需要包括【header】【过滤器】
    this->fileSize = byte_size_header + byte_size_bloomfliter;
}

/**
 * sstable的析构函数
 * @return none
 */
template<typename K, typename V>
SStable<K, V>::~SStable(){}


/**
 * 获取SSTable的时间戳
 * @return sstable生成的时间戳
 */

template<typename K, typename V>
uint64_t SStable<K, V>::getSavedTime(){
    return this->savedTime;
}

/**
 * 获取SSTable的时间戳
 * @param setTime 要设置的时间戳
 */

template<typename K, typename V>
void SStable<K, V>::setSavedTime(uint64_t setTime){
    this->savedTime = setTime;
}

/**
 * 获取SSTable的最小Key
 * @return sstable的最小Key
 */

template<typename K, typename V>
K SStable<K, V>::getMinKey(){
    return this->minKey;
}

/**
 * 获取SSTable的最大Key
 * @return sstable的最大Key
 */

template<typename K, typename V>
K SStable<K, V>::getMaxKey(){
    return this->maxKey;
}


/**
 * 获取SSTable的元素个数
 * @return 元素的个数
 */

template<typename K, typename V>
uint64_t SStable<K, V>::getElemNum(){
    return this->elemNum;
}


/**
 * 把ssTable的内容写到文件中，持久化，一旦文件写入完成之后，根据缓存策略
 * 删除掉不需要保留的部分。
 * @param path 要写入的目标路径
 * @return 写入的结果是否成功
 */
template<typename K, typename V>
int SStable<K, V>::saveToFile(std::string path){
    this->filePath = path;
    std::ofstream outFile(path, ios::out | ios::binary);
    
    if(outFile == NULL)
        return -1;

    outFile.write(this->savedTime, sizeof(this->savedTime));
    outFile.write(this->elemNum , sizeof(this->elemNum));
    outFile.write(this->minKey , sizeof(this->minKey));
    outFile.write(this->maxKey , sizeof(this->maxKey));
    
    uint32_t offsetBase = byte_size_header + byte_size_bloomfliter + elemNum * (byte_size_key_and_offset);


    for(uint64_t i = 0; i < elemNum; i++){
        K keyWrite = this->keyVec_key[i];
        uint32_t offsetWrite = offsetBase + this->keyVec_offset[i];

        outFile.write(keyWrite, sizeof(keyWrite));
        outFile.write(offsetWrite, sizeof(offsetBase));
    }

    for(uint64_t i = 0; i < elemNum; i++){
        outFile.write(this->valVec[i], sizeof(this->valVec[i]));    
    }
    
    outFile.close();
    return 0;
}


/**
 * 从文件里面读取所有的内容，并把相关变量设置好。
 * 和写入的操作是对称的！
 * @param path 文件路径
 */
template<typename K, typename V>
void SStable<K, V>::readFile_ReadAll(std::string path){
    std::ifstream inFile(path,ios::in|ios::binary);
    if(outFile == NULL)
        return -1;
    
    // 确认文件可以打开，就可以设置filePath了
    this->filePath = path;
    // 读取Header部分
    inFile.read((char*)&(this->savedTime), sizeof(this->savedTime));
    inFile.read((char*)&(this->elemNum), sizeof(this->elemNum));
    inFile.read((char*)&(this->minKey), sizeof(this->minKey));
    inFile.read((char*)&(this->maxKey), sizeof(this->maxKey));

    uint32_t offsetBase = byte_size_header + byte_size_bloomfliter + elemNum * (byte_size_key_and_offset);

    // 读取key和偏移量的部分
    for(uint64_t i = 0; i < elemNum; i++){
        K keyRead;
        uint32_t offsetRead;

        inFile.read((char*)&(keyRead), sizeof(keyRead));
        inFile.read((char*)&(offsetRead), sizeof(offsetRead));

        fileSize += sizeof(keyRead); 
        fileSize += sizeof(offsetRead);

        // 减去偏移量，然后再阅读
        offsetRead = offsetRead - offsetBase;
        
        // 放到容器里面去
        this->keyVec_key.push_back(keyRead);
        this->keyVec_offset.push_back(offsetRead);
    }

    // 读取value的部分
    for(uint64_t i = 0; i < elemNum; i++){
        if(i + 1 < elemNum){
            V valueRead;
            uint32_t readSize =  keyVec_offset[i + 1] - keyVec_offset[i];
            inFile.read((char*)&(valueRead), readSize);

            fileSize += readSize;
            this->valVec.push_back(valueRead);
        }
        else{
            
        }
    }

    fileSize = fileSize + sizeof(keyVec_key) + sizeof(keyVec_offset) + sizeof(valVec);
}

template<typename K, typename V>
void SStable<K, V>::readFile_HeaderOnly(std::string path){

}


template<typename K, typename V>
void readFile_Index(std::string path){

}


/**
 * 从文件里面读取ss table，根据缓存策略读取
 * 【readFile】有两个重载，如果只给路径，会根据缓存策略，读文件
 *  Cache_None -> ReadPart_Header：读取meta信息（比如文件名称、elemNum、minKey、maxKey）
 *  Cache_Only_Index -> ReadPart_Index：读取index信息（keyVec_key、keyVec_offset）
 *  Cache_Index_BF -> Read_All：读取全部信息
 * @param path 文件路径
 * @return -1代表缓存策略配置异常，其他的根据读取情况返回
 */
template<typename K, typename V>
int SStable<K, V>::readFile(std::string path){
    int result;
    switch (this->cacheStrategies)
    {
        case Cache_None:
            result = this->readFile_HeaderOnly(path);
            break;
        case Cache_Only_Index:
            result = this->readFile_Index(path);
            break;
        case Cache_Index_BF:
            result = this->readFile_ReadAll(path);
            break;
        default:
            return -1;
    }
    return result;
}

/**
 * 从文件里面读取ss table，目前来说考虑下面两种读取方式
 * 1: 全部读取方式，也就是说把整个文件全部读取进来，构建
 *    [class meta][header][BF][key vector][data vector]
 * 
 * 2: 部分读取模式 - 数据已经被写入硬盘了，因为系统崩溃或者查找原因，重新读取。
 *    这时候根据缓存策略，又要细分三种情况，
 *    - ReadPart_Header   代表只读取元数据 [class meta][header]
 *                        例如在缓存策略为：不缓存任何信息的时候，初始化使用
 *    - ReadPart_Index    代表读取元数据、索引信息 [class meta][header][key vector]
 *                        例如在缓存策略为：只缓存Index的时候，初始化使用
 * @param path 文件路径
 * @param type 阅读方式，如上所述
 * @return 打开的结果是否成功
 */
template<typename K, typename V>
int SStable<K, V>::readFile(std::string path, int type){
    this->filePath = path;
    int result = 0;
    switch (type)
    {
        case Read_All:
            result = this->readFile_ReadAll(path);
            break;
        case ReadPart_Header:
            result = this->readFile_HeaderOnly(path);
            break;
        case ReadPart_Index:
            result = this->readFile_Index(path)
        default:
            return -1;
    }
    return result;
}





/**
 * 根据BF过滤器，查找是否已经写入
 * @param targetKey 要查找的目标Key
 * @return 是否存在在BF过滤器，存在返回 1 ，不存在返回 0，BF不存在返回-1
 */
template<typename K, typename V>
int SStable<K, V>::BFfind(K targetKey){
    // 根据缓存策略，bf过滤器不存在，返回不存在
    if(this->bloomfliter == NULL)
        return -1;
    // 计算四个hash值
    unsigned int hash[4] = {0};
    MurmurHash3_x64_128(&targetKey, sizeof(key), 1, hash);
    
    // 对于四个位置的BF过滤器检测
    int result = 1;
    for(int i = 0; i < 4; i++){
        if(this->bloomfliter->test(hash[i] == false)){
            result = false;
            break;
        }
    }
    // 返回结果
    return result;
}


/**
 * 根据BF过滤器，往过滤器里面插入元素
 * @param setKey 要查找的目标Key
 * @return 是否插入成功，如果
 */
template<typename K, typename V>
int SStable<K, V>::BFinsert(K setKey){
    // 如果没有创建，那就创建一个过滤器（位图）
    if(this->bloomfliter == NULL){
        this->bloomfliter = new std::bitset<bit_size_bloomfliter>();
    }

    // 如果已经创建，那就继续写入
    // 计算四个hash值
    unsigned int hash[4] = {0};
    MurmurHash3_x64_128(&setKey, sizeof(key), 1, hash);

    // 设置BF过滤器！
    for(int i = 0; i < 4; i++){
        this->bloomfliter->set(hash[i] , true);
    }

}

/**
 * 清除key相关的存储容器
 */

template<typename K, typename V>
void SStable<K, V>::clearKeyVec(){
    this->keyVec_key.clear();
    this->keyVec_offset.clear();
}


/**
 * 清除val相关的存储容器
 */

template<typename K, typename V>
void SStable<K, V>::clearValVec(){
    this->valVec.clear();
}

/**
 * 清除BF过滤器
 */

template<typename K, typename V>
void SStable<K, V>::clearBF(){
    if(this->bloomfliter != NULL){
        delete bloomfliter;
    }
}


/**
 * 提供的插入元素的统一接口。
 * @param newkey 插入的key
 * @param newVal 插入的value
 * 注意：插入者需要保证插入的顺序是有序的
 */
template<typename K, typename V>
void SStable<K, V>::insertElem(K newkey, V newVal){
    if(elemNum == 0){
        this->minKey = newkey;
        this->maxKey = newkey;
    }

    // 简单的插入元素
    uint32_t beforeInsertSize = sizeof(valVec);
    this->keyVec_key.push_back(newkey);
    this->valVec.push_back(newVal);
    this->keyVec_offset.push_back(beforeInsertSize);

    this->elemNum++;

    // 更新最小最大key
    minKey = min(minKey, newkey);
    maxKey = max(maxKey, newkey);
}


