#include "include/rbtree.h"
#include "include/skiplist.h"
#include "include/avl.h"

#include <fstream>
#include <string>
#include <time.h> 
#include <iostream>

using namespace std;
const std::string DATA_PATH = "./testData/";
const std::string FilePrefix = "data-";
const int testSize[] = {512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};


void runRBTreeSingleTest(int ID, bool ifOrderNone){
    rbtree::RedBlackTree rbTree;
    string fileName = DATA_PATH + FilePrefix + to_string(testSize[ID]);
    if(ifOrderNone)
        fileName += "-OrderNone";
    else
        fileName += "-OrderAscend";
    
    
    ifstream fin(fileName , ios::in);
    vector<int> testData;
    int num;
    while(fin >> num){
        testData.push_back(num);
    }
    fin.close();

    clock_t start,end;
    start = clock();
    for(int i = 0; i < testData.size(); i++){
        rbTree.insert(testData[i]);
    }
    end = clock();
    std::cout << "RBT, ";
    cout << fileName << ", ";
    cout << double(end-start) / CLOCKS_PER_SEC << ", ";
    // cout << "RBT insert Time = " << double(end-start) / CLOCKS_PER_SEC<<"s"<<endl;

    start = clock();
    for(int i = 0; i < testData.size(); i++){
        rbTree.search(testData[i]);
    }
    end = clock();
    cout << double(end-start) / CLOCKS_PER_SEC << ", ";
    // cout << "RBT search Time = " << double(end-start) / CLOCKS_PER_SEC<<"s"<<endl;

    cout << rbTree.getRotateTimes();
    cout << endl;
}



void runRBTreeTest(){
    
    for(int i = 0; i < 8; i++){
        runRBTreeSingleTest(i, true);
        runRBTreeSingleTest(i, false);
    }
}


void runSkipListSingleTest(int ID, bool ifOrderNone){
    skiplist::Skiplist<int,int> skipList;
    string fileName = DATA_PATH + FilePrefix + to_string(testSize[ID]);
    if(ifOrderNone)
        fileName += "-OrderNone";
    else
        fileName += "-OrderAscend";

    ifstream fin(fileName , ios::in);
    vector<int> testData;
    int num;
    while(fin >> num){
        testData.push_back(num);
    }
    fin.close();

    clock_t start,end;
    start = clock();
    for(int i = 0; i < testData.size(); i++){
        skipList.insert(testData[i], testData[i]);
    }
    end = clock();
    std::cout << "SkipList, ";
    cout << fileName << ", ";
    cout << double(end-start) / CLOCKS_PER_SEC << ", ";
    // cout << "RBT insert Time = " << double(end-start) / CLOCKS_PER_SEC<<"s"<<endl;

    start = clock();
    for(int i = 0; i < testData.size(); i++){
        skipList.find(testData[i]);
    }
    end = clock();
    cout << double(end-start) / CLOCKS_PER_SEC << ", ";
    // cout << "RBT search Time = " << double(end-start) / CLOCKS_PER_SEC<<"s"<<endl;
    cout << endl;
}




void runSkipListTest(){
    for(int i = 0; i < 8; i++){
        runSkipListSingleTest(i, true);
        runSkipListSingleTest(i, false);
    }
}


void runAVLSingleTest(int ID, bool ifOrderNone){
    avl::AVL<int> avlTree;
    string fileName = DATA_PATH + FilePrefix + to_string(testSize[ID]);
    if(ifOrderNone)
        fileName += "-OrderNone";
    else
        fileName += "-OrderAscend";
    
    ifstream fin(fileName , ios::in);
    vector<int> testData;
    int num;
    while(fin >> num){
        testData.push_back(num);
    }
    fin.close();

    clock_t start,end;
    start = clock();
    for(int i = 0; i < testData.size(); i++){
        avlTree.insert(testData[i]);
    }
    end = clock();

    std::cout << "AVL ,";
    cout << fileName << ", ";
    cout << double(end-start) / CLOCKS_PER_SEC << ", ";

    start = clock();
    for(int i = 0; i < testData.size(); i++){
        avlTree.search(testData[i]);
    }
    end = clock();
    cout << double(end-start) / CLOCKS_PER_SEC << ", ";

    cout << avlTree.getRotateTimes();
    cout << endl;

}


void runAVLTest(){
    for(int i = 0; i < 8; i++){
        runAVLSingleTest(i, true);
        runAVLSingleTest(i, false);
    }
}

void runGlobalTest(){
    std::cout << "dataStructure, file,Insert,Search" << std::endl;
    runRBTreeTest();
    runSkipListTest();
    runAVLTest();
}


int main()
{
    runGlobalTest();
    return 0;
}