// 性能评测和随机化数据生成的代码

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <random>
#include <map>
#include <time.h>
#include <wait.h>


#include "kvstore.h"

// C++11支持的随机化引擎！
std::independent_bits_engine<std::default_random_engine, 64, uint64_t> e;

// 数据集合，复杂度更高，评测更有效！
// 如果添加空格字符，难度会增加！
const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()_+=-{}[]:;<>?/,.|`~";

// 最大的value字符串长度
const uint64_t maxValLength = 1024 * 32;

const uint64_t reSeedLength = 1000;

/**
 * 产生随机长度的字符串，随机长度为length
 * @param 随机产生的长度
*/
std::string randString(const int length, bool ifReseed){
    if(ifReseed == true){
        // 时间戳获取，精确到纳秒级别，所以即使在一个循环的反复调用不会有影响！
	    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::nanoseconds nsTime;
        nsTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());

        // 用纳秒作为时间种子！
        e.seed(nsTime.count());
    }

    // 开始随机！
    std::string str;
    char c;
    for(int i = 0; i < length; i++){
        c = charset[e() % (sizeof(charset) - 1)];
        str.push_back(c);
    }

    return str;
}

/**
 * 产生64位的随机数
 * @param limit 用来取模的最大限制，这样生成[0,limit - 1]的随机数
 * @param ifReseed 是否重新用时间作为随机数种子
 **/ 
uint64_t randUnInt64(uint64_t limit, bool ifReseed){
    if(ifReseed == true){
        // 时间戳获取，精确到纳秒级别，所以即使在一个循环的反复调用不会有影响！
	    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::nanoseconds nsTime;
        nsTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());

        // 用纳秒作为时间种子！
        e.seed(nsTime.count());
    }
    
    return e() % limit;
}


/**
 * 产生随机数据集合
 * @param ExistDataGourp 保证是存在的数据集合
 * @param NoExistDataGroup 代表的事不存在的数据集合（后续用来评测查找的时候应该不存在）
 * @param existDataSize 存在的数据集合的大小
 * @param noExistDataSize 不存在的数据截获的大小
*/
uint64_t generateData(
    std::map<uint64_t, std::string> &ExistDataGourp,
    std::map<uint64_t, std::string> &NoExistDataGroup,
    size_t existDataSize, 
    size_t noExistDataSize){

    //
    uint64_t valSize = 0;
    
    // 用来验证已经插入的数据集合
    std::map<uint64_t, bool> ExistKeyMap;
    uint64_t genKey = randUnInt64(UINT64_MAX, true);
    uint64_t genValLen = randUnInt64(maxValLength, true);
    std::string genVal = randString(genValLen, true);

    // 产生存在数据集合
    for(size_t i = 0; i < existDataSize; i++){
        // 如果已经存在，那就再生成随机数
        while(ExistKeyMap.count(genKey) != 0){
            genKey = randUnInt64(UINT64_MAX, false);
            genValLen = randUnInt64(maxValLength, false);
            genVal = randString(genValLen, false);
        }

        ExistKeyMap[genKey] = true;
        ExistDataGourp[genKey] = genVal;
        valSize += genVal.size();

        // 产生下一个随机数
        bool ifReseed = (i % reSeedLength  == 0);

        genKey = randUnInt64(UINT64_MAX, ifReseed);
        genValLen = randUnInt64(maxValLength, ifReseed);
        genVal = randString(genValLen, ifReseed);
    }

    // 产生不存在数据集合
    for(size_t i = 0; i < noExistDataSize; i++){

        // 如果已经存在，那就再生成随机数
        while(ExistKeyMap.count(genKey) != 0){
            genKey = randUnInt64(UINT64_MAX, false);
            genValLen = randUnInt64(maxValLength, false);
            genVal = randString(genValLen, false);
        }

        ExistKeyMap[genKey] = true;
        NoExistDataGroup[genKey] = genVal;

        // 产生下一个随机数，判断种子是否重置
        bool ifReseed = (i % reSeedLength  == 0);
        genKey = randUnInt64(UINT64_MAX, ifReseed);
        genValLen = randUnInt64(maxValLength, ifReseed);
        genVal = randString(genValLen, ifReseed);
    }

    return valSize;
}

/**
 * 运行插入性能测试，返回插入的事件
 * @return 返回插入的平均时间！
 * */ 
double runInsertPerformanceTest(std::map<uint64_t, std::string> &data, KVStore &kvstore){
    if(data.size() == 0)
        return -1;
    clock_t start,end;
    start = clock();
    for(auto iter = data.begin(); iter != data.end(); iter++){
        kvstore.put(iter->first, iter->second);
    }
    end = clock();
    // 计算起点和终止时间
    double insertTime = double(end-start) / CLOCKS_PER_SEC;
    // 返回插入的平均时间
    std::cout << "runInsertPerformanceTest: avg time is"  << insertTime / data.size() << "s \n";
    return insertTime / data.size();
}


/**
 * 运行查找性能测试，返回插入的事件
 * @return 返回插入的平均时间！
 */ 
double runGetPerformanceTest(std::map<uint64_t, std::string> &data, KVStore &kvstore){
    if(data.size() == 0)
        return -1;

    clock_t start,end;
    start = clock();
    for(auto iter = data.begin(); iter != data.end(); iter++){
        std::string getResult = kvstore.get(iter->first);

        if(getResult != iter->second){
            std::cout << "EXPECT: " << iter->second.substr(0,10) << "\n";
            std::cout << "REAL: " << getResult.substr(0,10) << "\n";
        }

    }
    end = clock();
    // 计算起点和终止时间
    double getTime = double(end-start) / CLOCKS_PER_SEC;

    std::cout << "runGetPerformanceTest: avg time is" <<  getTime / data.size() << "s \n";
    // 返回插入的平均时间
    return getTime / data.size();
}


/**
 * 运行删除性能测试，返回插入的事件
 * @return 返回插入的平均时间！
 */ 
double runDelPerformanceTest(std::map<uint64_t, std::string> &data, KVStore &kvstore){
    if(data.size() == 0)
        return -1;
    clock_t start,end;
    start = clock();
    for(auto iter = data.begin(); iter != data.end(); iter++){
        kvstore.del(iter->first);
    }
    end = clock();
    // 计算起点和终止时间
    double delTime = double(end-start) / CLOCKS_PER_SEC;

    std::cout << "runDelPerformanceTest: avg time is" <<  delTime / data.size() << "s \n";
    // 返回插入的平均时间
    return delTime / data.size();
}

/**
 * 运行性能测试，依次进行插入、查找、删除所有数据的操作，统计平均每个数据的查找时间。
 */ 
void runPerformanceTest(){
    std::map<uint64_t, std::string> ExistDataGourp;
    std::map<uint64_t, std::string> NoExistDataGroup;

    KVStore mystore("./data");
    mystore.reset();

    runInsertPerformanceTest(ExistDataGourp, mystore);
    runGetPerformanceTest(ExistDataGourp, mystore);
    runDelPerformanceTest(ExistDataGourp, mystore);
}



/**
 * 模拟崩溃的进行，先插入数据，然后子进程意外终止，然后父进程继续读写，
*/
void crashTest(){
    std::map<uint64_t, std::string> ExistDataGourp;
    std::map<uint64_t, std::string> NoExistDataGroup;
    

    // 运行性能测试！产生需要的数据集合，只需要产生 ExistDataGourp
    uint64_t dataSize = generateData(ExistDataGourp, NoExistDataGroup, 1024 * 8, 0);
    std::cout << "Data-val Size(Byte) is: " << dataSize << "\n";
    
    pid_t pid = fork();
    int status;

    
    if (pid == 0){
        /* child process */
        KVStore mystore("./data");
        mystore.reset();
        runInsertPerformanceTest(ExistDataGourp, mystore);

        // 主动让进程崩溃。模拟突然出错的情况
        int a = 1 / 0;
        // return;
    }
    else{  
        /* father process*/
        wait(&status);
    }   
    std::cout << "Construct Start\n";
    KVStore mystore("./data");
    std::cout << "GET/DEL Start\n";
    runGetPerformanceTest(ExistDataGourp, mystore);
    runDelPerformanceTest(ExistDataGourp, mystore);
}





int main(){
    crashTest();

    return 0;
}


