#include "sstheader.h"
#include <iostream>

using namespace std;

int main(){
    SSTheader myheader;
    myheader.timeStamp = 99;
    myheader.keyValNum = 1;
    myheader.minKey = 1;
    myheader.maxKey = 1;
    cout << myheader.writeToFile("./mydata", 32) << endl;

    // SSTheader myheaderRead;
    // cout << myheader.readFile("./mydata", 0);

    // cout << myheader.timeStamp << endl;
    // cout << myheader.keyValNum << endl;
    // cout << myheader.minKey << endl;
    // cout << myheader.maxKey << endl;
    return 0;
}











// #include "kvstore.h"

// using namespace std;

// int main(){
//     KVStore kvstore("./data");
    
//     kvstore.put(4,"123");
//     kvstore.put(5,"aaa");
//     kvstore.put(6,"v");
//     kvstore.put(7,"34");
//     kvstore.put(9,"me");
//     std::list<std::pair<uint64_t, std::string> > mylist;
//     kvstore.scan(7,300,mylist);


//     for(auto pt = mylist.begin(); pt != mylist.end(); pt++){
//         cout << pt->first << " " << pt->second  << endl;
//     }
//     return 0;
// }




















// // #include "kvstore.h"
// #include "bloomfliter.h"
// #include <iostream>

// int main(){
//     std::cout << "debug程序开始运行" << std::endl;
//     BloomFliter<int,50000> bf;
//     for(int i = 0; i < 100; i++){
//         bf.BFinsert(i);
//     }
//     std::cout << "插入完成" << std::endl;

//     for(int i = 0; i < 100; i++){
//         if(bf.BFfind(i))
//              std::cout << "插入成功" << std::endl;
//     }
//     for(int i = 100; i < 300; i++){
//         if(bf.BFfind(i))
//              std::cout << "missMatch" << std::endl;
//     }
//     std::cout << "测试完成" << std::endl;
//     return 0;
// }
