#include "sstvalue.h"

/**
 * 把SST的value数据部分，写入到文件的对应位置。
 * @param path 文件的路径
 * @param offset 要写入的偏移量，由调用者指定，不能超过文件大小
 * @return 返回实际写入的偏移量
*/
uint32_t SSTvalue::writeToFile(std::string path, uint32_t offset){
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
    for (size_t i = 0; i < valVec.size(); i++)
    {
        outFile.write(valVec[i].c_str(), valVec[i].size());
    }
    outFile.close();
    return offset;
}




/**
 * 插入val元素
 *  @param newVal 新添加的val值
 * 插入完成后会增加元素的数量
 */
void SSTvalue::insert(std::string newVal){
    valVec.push_back(newVal);
    valNum++;
}


/**
 * 获取第i个key的val
 *  @param index 第i个
 *  @return 第i个key的val
 */
std::string SSTvalue::getVal(size_t index){
    if(index > valNum)
        return "~!Exceed Limit!~";
    return valVec[index];
}

/**
 * 通过文件读写，来获取某一段字符串
 * 由于sstvalue这个类不保存任何偏移量信息，调用者需要自行计算。
 * @param path 数据文件路径
 * @param offset 读取开始的偏移量，不能超过文件大小（调用者自己计算）
 * @param length 要读取字符串的长度（调用者自己计算）
 */
std::string SSTvalue::getValFromFile(std::string path, uint32_t offset, size_t length){
    std::ifstream inFile(path, std::ios::in|std::ios::binary);
    if (!inFile)
        return "~!NO File!~";

    // 文件指针移动到末尾
    inFile.seekg(0,std::ios::end);
    uint32_t fileLimit = inFile.tellg();

    // 判断是否超过限度 读取起点越界或者终点越界，都会返回-1
    if(offset > fileLimit || offset + length > fileLimit){
        inFile.close();
        return "~!Exceed Limit!~";
    }

    inFile.clear();
    // 通过检查，文件指针移动到偏移量
    inFile.seekg(offset,std::ios::beg);


    char* readStr = new char[length];
    inFile.read(readStr, length);
    
    // 拷贝到str然后删除释放空间
    std::string readResult(readStr);
    delete [] readStr;

    inFile.close();
    return readResult;
}

/**
 * 获取value的数量
 *  @return 当前存储的value的个数
 */
size_t SSTvalue::getValNum(){
    return valNum;
}