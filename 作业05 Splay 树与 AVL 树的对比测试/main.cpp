#include "AVL.h"
#include "SplayTree.h"
#include <time.h>
#include <map>
using namespace std;

vector<int> dataAll;

void generateData(int n){
    map<int,bool> dataExit;
    for (size_t i = 0; i < n; i++)
    {
        int dataRand = rand();
        while(dataExit.count(dataRand) == 1){
            dataRand = rand();
        }
        dataExit[dataRand] = true;
        dataAll.push_back(rand());
    }
}


void runSingleTest(long int k, long int m){
    AVL<int> avl;
    SplayTree<int,int> spTree;

    // 插入数据
    for (size_t i = 0; i < dataAll.size(); i++)
    {
        avl.insert(dataAll[i]);
        spTree.insert(dataAll[i], dataAll[i]);
    }
    
    long int target = 0;
    clock_t start,end;
    start = clock();
    for (size_t i = 0; i < m; i++){
        // avl.search(dataAll[target]);
        avl.search(dataAll[dataAll.size() - 1 - target]);
        target = (target + 1) % k;
    }
    end = clock();
    cout << "k = " << k << " m = " << m << endl;
    cout << "AVL search Time = " << double(end-start) / CLOCKS_PER_SEC << "s"<<endl;

    target = 0;
    start = clock();
    for (size_t i = 0; i < m; i++){
        spTree.search(dataAll[dataAll.size() - 1 - target]);
        // spTree.search(dataAll[target]);
        target = (target + 1) % k;
    }
    end = clock();
    cout << "spTree search Time = " << double(end-start) / CLOCKS_PER_SEC << "s"<<endl;
    cout <<endl;
}

void runTest(){
    double rate[] = {0.00125,0.0025, 0.005, 0.01, 0.05 , 0.25, 0.45, 0.65, 0.95};
    long int n = 1024;
    long int m = n * 1024;

    generateData(n);

    for (size_t i = 0; i < 9; i++)
    {
        long int k = n * rate[i];
        runSingleTest(k , m);
    }
}


int main(){
    srand(time(NULL));
    runTest();

    return 0;
}