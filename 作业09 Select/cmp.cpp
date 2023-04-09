// 本程序需比较 Quick Select 与 Linear Select 的性能差异以及一些参数对算法的影响

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

vector<int> dataAll;

// 随机产生数据 policy 为 0 时，数据为随机数；为 1 时，数据需要升序排序
// 为2时，数据需要降序排序
void generateData(int policy, int length){
    srand(time(NULL));
    dataAll.clear();
    for(int i = 0; i < length; i++){
        dataAll.push_back(rand());
    }

    if(policy == 1){
        sort(dataAll.begin(), dataAll.end());
    }
    else if(policy == 2){
        sort(dataAll.begin(), dataAll.end());
        reverse(dataAll.begin(), dataAll.end());
    }
}

// 返回数组中，从array里面第k小的元素，left和right仅仅是为了递归调用
// left和right仅仅代表搜索的范围，每次搜索的第k小的元素，是在整个数组里面的第k小的元素
// 而不是在left和right之间的第k小的元素！！！（和后面的有一些区别）
// k是从0开始的！
int QuickSelect(vector<int> &array, int left, int right, int k){
    if(left == right){
        return array[left];
    }

    int pivot = array[left];
    int i = left + 1;
    int j = right;
    while(i <= j){
        while(i <= j && array[i] <= pivot){
            i++;
        }
        while(i <= j && array[j] > pivot){
            j--;
        }
        if(i < j){
            swap(array[i], array[j]);
        }
    }
    swap(array[left], array[j]);

    if(j == k){
        return array[j];
    }else if(j < k){
        return QuickSelect(array, j + 1, right, k);
    }else{
        return QuickSelect(array, left, j - 1, k);
    }
}

size_t Q  = 128;

size_t useTimes = 0;
// 返回数组中，从left到right区间内第k小的元素
// k是从0开始的！
int LinearSelect(vector<int> &array, int left, int right, int k){
    useTimes++;

    // 如果区间长度小于等于Q，直接快速排序
    if(right - left + 1 <= Q){
        sort(array.begin() + left, array.begin() + right + 1);
        // sort(&array[left], &array[right + 1]);
        return array[left + k];
    }

    // 将A均匀地划分为n/Q个子序列，各含Q个元素; 将返些中位数组成一个序列seqAll;
    vector<int> seqAll;

    for (size_t i = 0; i < array.size(); i+= Q)
    {
        size_t result = QuickSelect(array, i, min(i + Q - 1, array.size() - 1), Q / 2);
        seqAll.push_back(result);
    }

    // 递归地求出seqAll的中位数
    int pivot = LinearSelect(seqAll, 0, seqAll.size() - 1, seqAll.size() / 2);
    // 根据其相对亍M癿大小，将A中元素分为三个子集：L（小亍）、E（相等）和G（大亍）;
    vector<int> LGroup;
    vector<int> EGroup;
    vector<int> GGroup;

    for (size_t i = left; i <= right; i++)
    {
        if(array[i] < pivot){
            LGroup.push_back(array[i]);
        }else if(array[i] == pivot){
            EGroup.push_back(array[i]);
        }else{
            GGroup.push_back(array[i]);
        }
    }

    // 如果k小亍L，递归地在L中寻找第k小元素
    if (k < LGroup.size())
        return LinearSelect(LGroup, 0, LGroup.size() - 1, k);
    // 如果k大亍L和E，返回M
    else if (k < LGroup.size() + EGroup.size())
        return pivot;
    // 如果k大亍L和E，递归地在G中寻找第k - L.size() - E.size()小元素
    else
        return LinearSelect(GGroup, 0, GGroup.size() - 1, k - LGroup.size() - EGroup.size());
}


int correctTest(int length){
    vector<int> tmpData;
    vector<int> dataForVerify;
    srand(time(NULL));
    for(int i = 0; i < length; i++){
        int num = rand();
        tmpData.push_back(num);
        dataForVerify.push_back(num);
    }

    sort(dataForVerify.begin(), dataForVerify.end());

    for(int i = 0; i < length; i++){
        int result = LinearSelect(tmpData, 0, tmpData.size() - 1, i);
        if(result != dataForVerify[i]){
            cout << "LinearSelect Error!" << endl;
            return 1;
        }
        else if(i % 100 == 0)
            cout << "LinearSelect OK!" << endl;
    }

    for(int i = 0; i < length; i++){
        int result = QuickSelect(tmpData, 0, tmpData.size() - 1, i);
        if(result != dataForVerify[i]){
            cout << "QuickSelect Error!" << endl;
            return 1;
        }
        else if(i % 100 == 0)
            cout << "QuickSelect OK!" << endl;
    }
    return 0;
}

/**
 * @param length: 数据长度
 * @param ifOrder :0: 无序，1: 升序，2: 降序
*/
double performanceTest_LinearSelect(int length, int ifOrder){
    generateData(ifOrder, length);
    clock_t start, end;
    start = clock();
    for(int i = 0; i < length; i+= length / 64){
        int result = LinearSelect(dataAll, 0, dataAll.size() - 1, i);
    }
    end = clock();
    // cout << "LinearSelect: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
    return (double)(end - start) / CLOCKS_PER_SEC;
}

/**
 * @param length: 数据长度
 * @param ifOrder :0: 无序，1: 升序，2: 降序
*/
double performanceTest_QuickSelect(int length, int ifOrder){
    generateData(ifOrder, length);
    clock_t start, end;
    start = clock();
    for(int i = 0; i < length; i+= length / 64){
        int result = QuickSelect(dataAll, 0, dataAll.size() - 1, i);
    }
    end = clock();
    // cout << "QuickSelect: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
    return (double)(end - start) / CLOCKS_PER_SEC;
}



void runTest(){
    int lengthTest[] = {64, 1024, 4096 ,8096};
    cout << "linear" << endl;
    for (size_t i = 0; i < 4; i++)
    {   
        useTimes = 0;
        cout << performanceTest_LinearSelect(lengthTest[i], 0) << ",";
        cout << useTimes << ",";

        useTimes = 0;
        cout << performanceTest_LinearSelect(lengthTest[i], 1) << ",";
        cout << useTimes << ",";

        useTimes = 0;
        cout << performanceTest_LinearSelect(lengthTest[i], 2) << ",";
        cout << useTimes << "," << endl;
    }

    cout << "quick" << endl;

    for (size_t i = 0; i < 4; i++)
    {
        cout << performanceTest_QuickSelect(lengthTest[i], 0) << ",";
        cout << performanceTest_QuickSelect(lengthTest[i], 1) << ",";
        cout << performanceTest_QuickSelect(lengthTest[i], 2) << endl;
    }
}

void runQTest(){
    int lengthTest = 4096;
    int QTest[] = {64, 128, 256, 512, 1024};
    for (size_t i = 0; i < 5; i++)
    {
        Q = QTest[i];
        cout << Q << ",";
        cout << performanceTest_LinearSelect(lengthTest, 0) << "\n";
    }
    
}


int main(){
    runQTest();
    return 0;
}