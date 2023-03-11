#include "sstindex.h"

/**
 * 创建SSTindex对象，并从文件里面读取
*/
SSTindex::SSTindex(std::string path, uint32_t offset,size_t readKeyNum){
    this->keyNum = 0;
    readFile(path,offset,readKeyNum);
}


/**
 * 从文件读取SST的索引部分，根据文件的目录、
 * @param path 要读取文件的路径
 * @param offset 要读取的文件的起点偏移量
 * @param readKeyNum 要读取的Key的数量
 * @return 读取结果 0代表正常返回，-1代表文件不存在 -2代表范围异常
 * 
 * @example [比如读取1个key，那就是一个 uint64_t + uint32_t 一共12Byte] 
 * 要读取一个key，请使用 readFile("./data", 32, 1);
 */
int SSTindex::readFile(std::string path, uint32_t offset,size_t readKeyNum){
    std::ifstream inFile(path, std::ios::in|std::ios::binary);
    if (!inFile)
        return -1;

    // 文件指针移动到末尾
    inFile.seekg(0,std::ios::end);
    uint32_t fileLimit = inFile.tellg();

    // 判断是否超过限度 读取起点越界或者终点越界，都会返回-1
    if(offset > fileLimit || offset + (sizeof(uint64_t) + sizeof(uint32_t)) * readKeyNum > fileLimit){
        inFile.close();
        return -2;
    }

    inFile.clear();
    // 通过检查，文件指针移动到偏移量
    inFile.seekg(offset,std::ios::beg);

    for(size_t i = 0; i < readKeyNum; i++){
        uint64_t keyRead;
        uint32_t offsetRead;

        inFile.read((char*)&keyRead, sizeof(keyRead));
        inFile.read((char*)&offsetRead, sizeof(offsetRead));

        keyVec.push_back(keyRead);
        offsetVec.push_back(offsetRead);
        keyNum++;
    }

    inFile.close();
    return 0;
}

/**
 * 把SST的索引部分，写入到文件的对应位置。
 * @param path 文件的路径
 * @param offset 要写入的偏移量，由调用者指定，不能超过文件大小
 * @return 返回实际写入的偏移量
*/
uint32_t SSTindex::writeToFile(std::string path, uint32_t offset){
    // 判断文件是否存在，用尝试读取的方式打开，如果打开成功就认为文件存在
    bool ifFileExist = false;
    uint32_t fileLimit = 0;
    std::ifstream inFile(path, std::ios::in|std::ios::binary);

    if (inFile){
        // 打开成功，获取文件大小
        inFile.seekg(0,std::ios::end);
        fileLimit = inFile.tellg();
        inFile.close();
        ifFileExist = true;
    }

    // 文件不存在
    if(!ifFileExist){
        offset = 0;
        // 创建文件
        std::fstream createFile(path, std::ios::out | std::ios::binary);
        createFile.close();
    }
    // 文件存在且用户设置的偏移量大于文件最大的范围，以追加形式写入文件
    else if(ifFileExist && (offset > fileLimit)){
        offset = fileLimit;
    }

    std::fstream outFile(path, std::ios::out | std::ios::in | std::ios::binary);
    
    if (!outFile)
        return -1;

    // 把文件指针移动到偏移量
    outFile.seekp(offset,std::ios::beg);

    // 开始写入文件
    for (size_t i = 0; i < keyVec.size(); i++)
    {
        outFile.write((char*)&(keyVec[i]), sizeof(uint64_t));
        outFile.write((char*)&(offsetVec[i]), sizeof(uint32_t));
    }
    
    outFile.close();
    return offset;
}


/**
 * 插入key元素，并设置他的value的偏移量，不会检查重复
 *  @param newKey 新添加的key值
 *  @param valOffset key值对应value在文件中的偏移量
 * 插入完成后会增加元素的数量
 */
void SSTindex::insert(uint64_t newKey, uint32_t valOffset){
    keyVec.push_back(newKey);
    offsetVec.push_back(valOffset);
    keyNum++;
}


/**
 * 获取索引里面的key个个数
 *  @return 索引里面的key的个数
 */
 size_t SSTindex::getKeyNum(){
    return this->keyNum;
 }

/**
 * 获取第i个key，越界返回-1
 *  @param index 第i个
 *  @return 第i个key
 */
uint64_t SSTindex::getKey(size_t index){
    if(index >= keyVec.size()){
        exit(-1);
        return -1;
    }
    return keyVec[index];
}


/**
 * 获取第i个key的val的偏移量，越界返回-1
 *  @param index 第i个
 *  @return 第i个key的val的偏移量
 */
uint32_t SSTindex::getOffset(size_t index){
    if(index > keyNum){
        exit(-1);
        return -1;
    }
    return offsetVec[index];
}