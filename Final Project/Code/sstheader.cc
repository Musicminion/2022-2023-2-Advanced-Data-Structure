#include "sstheader.h"

/**
 * 从文件读取Header数据
 * @param path 文件的路径
 * @param offset 要读取的偏移量，默认为0
 * @return 0代表正常返回，-1代表文件不存在 -2代表范围异常
 * 使用样例：readFile("./data", 0);
 */
int SSTheader::readFile(std::string path, uint32_t offset){
    std::ifstream inFile(path, std::ios::in|std::ios::binary);
    if (!inFile)
        return -1;
    
    // 文件指针移动到末尾
    inFile.seekg(0,std::ios::end);

    uint32_t fileLimit = inFile.tellg();

    // 判断是否超过限度 读取起点越界或者终点越界，都会返回-1
    if(offset > fileLimit || offset + sizeof(SSTheader) > fileLimit){
        inFile.close();
        return -2;
    }
    
    inFile.clear();
    
    // 通过检查，文件指针移动到偏移量
    inFile.seekg(offset,std::ios::beg);

    // 读取文件
    inFile.read((char*)&timeStamp, sizeof(timeStamp));
    inFile.read((char*)&keyValNum, sizeof(keyValNum));
    inFile.read((char*)&minKey, sizeof(minKey));
    inFile.read((char*)&maxKey, sizeof(maxKey));

    inFile.close();
    return 0;
}


/**
 * 把Header数据写入到文件里面
 * @param path 文件的路径
 * @param offset 要写入的偏移量，默认为0
 * @return 返回实际写入的偏移量，出现异常返回-1
 * 
 * @example readFile("./data", 0);
 * 如果需要写入到特定的文件的位置，函数会有判断逻辑：当offset != 0 的时候，会判断文件是否存在
 * 如果文件不存在，会创建一个文件，并把写入的起点offset设置为0
 * 如果文件存在，会判断文件的大小，当offset < 文件大小的时候，才会覆盖性的写入，反之以追加模式写入文件
 * 调用者可以通过[返回值] 获取到实际写入的偏移量（Byte单位），-1(也就是一个最大的int32)说明是异常
 */
uint32_t SSTheader::writeToFile(std::string path, uint32_t offset){
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
    outFile.write((char*)&timeStamp, sizeof(timeStamp));
    outFile.write((char*)&keyValNum, sizeof(keyValNum));
    outFile.write((char*)&minKey, sizeof(minKey));
    outFile.write((char*)&maxKey, sizeof(maxKey));

    outFile.close();
    return offset;
}