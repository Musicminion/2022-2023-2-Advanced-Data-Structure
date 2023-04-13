#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <string>
#include <map>

using namespace std;

const string DATA_PATH = "./testData/";
const string FilePrefix = "data-";
const int testSize[] = {512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};

void generateData(size_t dataSize, string fileName){
    srand(time(NULL));

    vector<int> testData;
    map<int, bool> record;
    // 生成数据，保证数据不重复
    for(size_t i = 0; i < dataSize; i++){
        int randNum = rand();
        while(record.count(randNum) != 0){
            randNum = rand();
        }
        record[randNum] = true;
        testData.push_back(randNum);
    }

    // 乱序写入文件
    ofstream fout_OrderNone(fileName + "-OrderNone" , ios::out);
    for(size_t i = 0; i < dataSize; i++){
        fout_OrderNone << testData[i] << endl;
    }
    fout_OrderNone.close();

    // 把数据排序
    sort(testData.begin(), testData.end());

    // 顺序写入文件
    ofstream fout_OrderAscend(fileName + "-OrderAscend" , ios::out);
    for(size_t i = 0; i < dataSize; i++){
        fout_OrderAscend << testData[i] << endl;
    }
    fout_OrderAscend.close();
}

int main(){
    
    for(int i = 0; i < 8; i++){
        generateData(testSize[i], DATA_PATH + FilePrefix + to_string(testSize[i]));
    }
    return 0;
}