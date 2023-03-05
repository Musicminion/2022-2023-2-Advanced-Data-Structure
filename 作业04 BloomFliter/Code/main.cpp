#include<iostream>
#include <functional>
#include <vector>
#include <string>
#include <map>
#include <time.h>
#include "tabulate/table.hpp"

using namespace std;
std::hash<string> hash_str;

// 三个参数，第一个是要hash的X，第二个是第i个Hash函数
// 根据PDF要求，hash_i = hash(x+i)
int myHashI(int x,  int times, int limit){
    int num = x + times;
    string num_str = to_string(num);
    return hash_str(num_str) % limit;
}


class BloomFilter{
private:
    int m;  // 哈希数组的大小
    int k;  // hash函数的个数
    int *data;
public:
    BloomFilter(int set_m, int set_k){
        m = set_m;
        k = set_k;
        data = new int[set_m];

        for(int i = 0; i < m; i++){
            data[i] = 0;
        }
    }
    ~BloomFilter(){
        if(data != NULL)
            delete[] data;
    }
    // 插入一个元素
    void insertNum(int num){
        for(int i = 0; i < k; i++){
            int hashed_val = myHashI(num, i, m);
            data[hashed_val] = 1;
            //cout << "set hashed val to 1 " << hashed_val << endl;
        }
    }
    // 返回是否存在某个元素
    bool findNum(int num){
        //cout << "try find! ############ "  << endl;
        for(int i = 0; i < k; i++){
            int hashed_val = myHashI(num, i, m);
            //cout << "try find! hashed val is : " << hashed_val << endl;
            if(data[hashed_val] == 0){
                return false;
            }
        }
        return true;
    }
};

// 不妨固定 m = 403200 (1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 = 40320, 然后我再乘以了100)
// m/n      2      3         4       5       6       7       8 
// n      202600  134400   100800   80640   67200   57600   50400 

double bloomTest(int m, int n, int k, int noneExitNum){
    BloomFilter bloomFilter(m, k);
    map<int,bool> preparedData;
    vector<int> noneExistNumData;
    for(int i = 0; i < n; i++){
        int randVal = rand();
        while(preparedData.count(randVal) == 1){
            randVal = rand();
        }
        preparedData[randVal] = 1;
        bloomFilter.insertNum(randVal);
    }

    for(int i = 0; i < noneExitNum; i++){
        int randVal = rand();
        while(preparedData.count(randVal) == 1){
            randVal = rand();
        }
        noneExistNumData.push_back(randVal);
    }

    int wrongNum = 0;
    for(int i = 0; i < noneExitNum; i++){
        if(bloomFilter.findNum(noneExistNumData[i]) == true){
            wrongNum++;
        }
    }

    // return wrongNum;
    return wrongNum * 1.0 / noneExitNum;
}


   
void runTest(){
    srand(time(NULL));
    int m = 403200;
    using Row_t = std::vector<variant<std::string, const char *, string_view, tabulate::Table>>;
    tabulate::Table resultTable;
    Row_t row_header;
    row_header.push_back("data");
    for(int k = 1; k <= 8; k++){
        string str = "k = " + to_string(k);
        row_header.push_back(str);
    }
    resultTable.add_row(row_header);

    for(int i = 2; i <=8; i++){
        Row_t row_data;
        string str = "m/n = " + to_string(i);
        row_data.push_back(str);
        for(int k = 1; k <= 8; k++){
            int n = m / i;
            // cout << bloomTest(m, n, k, 10000) << "\t";
            row_data.push_back(to_string(bloomTest(m, n, k, 10000)));
        }
        resultTable.add_row(row_data);
    }
    cout << resultTable << endl;
}


int main(){
    runTest();
    return 0;
}