#include "sstheader.h"
#include "bloomfliter.h"
#include <iostream>

using namespace std;

int main(){
    SSTheader myheader;
    myheader.timeStamp = 123;
    myheader.keyValNum = 100;
    myheader.minKey = 1;
    myheader.maxKey = 10;
    
    BloomFliter<int,1024> bf;
    bf.BFinsert(310);
    bf.BFinsert(2345);

    myheader.writeToFile("./mydata", 0);
    bf.BFwriteToFile("./mydata", 32);


    SSTheader myread;
    BloomFliter<int,1024> bfRead;

    myread.readFile("./mydata", 0);
    bfRead.BFreadFile("./mydata",32);
    
    cout << myread.timeStamp  << endl;
    cout << myread.keyValNum  << endl;
    cout << myread.minKey << endl;
    cout << myread.maxKey << endl;
   

    cout << bfRead.BFfind(310) << endl;
    cout << bfRead.BFfind(2347) << endl;
    
    myheader.keyValNum = 100;

    return 0;
}







    
    // cout << "offset is : " << myheader.writeToFile("./mydata", 128) << endl;
    
    // SSTheader myheaderRead;
    // cout << myheaderRead.readFile("./mydata", 0) << endl;
    // cout << myheaderRead.timeStamp << endl;
    // cout << myheaderRead.keyValNum << endl;
    // cout << myheaderRead.minKey << endl;
    // cout << myheaderRead.maxKey << endl;



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
