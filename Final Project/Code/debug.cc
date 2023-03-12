#include <ctime>
#include <string>
#include <chrono>
#include <sstream>
#include <iostream>

#include "kvstore.h"

using namespace std;

int main()
{

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::microseconds msTime;
    msTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());

    for(int i = 0; i <  100; i++){
        cout << msTime.count() << endl;
    }

    // std::list <std::pair<uint64_t, std::string> > list;

    // for (size_t i = 0; i < 10000; i++)
    // {
    //     string str = "aaaaa" + to_string(i);
    //     list.push_back({i*2,str});  
    // }

    // bool cachePolicy[4] = {true, true, true, false};
    // SStable mytable(32,list, "./data/level-0/test.sst", cachePolicy);

    // SStable mytable2("./data/level-0/test.sst", cachePolicy);

    // for(int i = 0; i < 1000; i++){
    //     cout << mytable2.getSStableKey(i) << endl;
    //     cout << mytable2.getSStableKeyOffset(i) << endl;
    //     cout << mytable2.getSStableValue(i) << endl;
    // }

    // cout << mytable2.getSStableKeyValNum()<< endl;
    // cout << mytable2.getSStableTimeStamp()<< endl;
    // cout << mytable2.getSStableMaxKey() << endl;
    // cout << mytable2.getSStableMinKey() << endl;
    

    return 0;
}



// #include "sstable.h"
// #include <iostream>
// #include <sstream>
// #include <chrono>
// #include <unistd.h>

// using namespace std;
// using namespace chrono;

// int main(){

//     for(int i = 0; i < 100; i++){
//         auto t1 = std::chrono::high_resolution_clock::now();
//         cout << std::chrono::duration_cast<std::chrono::nanoseconds>(t1.time_since_epoch()).count() << endl;
//     }
    

//     uint64_t value; 
//     std::istringstream iss("24272206472791"); 
//     iss >> value;
//     cout << value;

//     // bool policy[4] = {true,true,true,false};
//     // std::list <std::pair<uint64_t, std::string> > list;

//     // list.push_back({0, "adadad"});
//     // list.push_back({3345, "bcccbb"});
//     // list.push_back({23, "bbbcass"});
//     // list.push_back({43, "bbb"});
//     // list.push_back({667, "bbbf"});
//     // list.push_back({777, "bbsb"});
    
//     // SStable mysstable(1, list, "./mydata" , policy);


//     // SStable mytestTable("./mydata", policy);
//     // mytestTable.devTest();
    

//     return 0;    
// }


// #include <iostream>
// #include <string>
// #include "sstvalue.h"

// using namespace std;

// int main(){
//     SSTvalue test;
//     test.insert("123****");
//     test.insert("456");
//     test.insert("7777");

//     cout << test.getValNum() << endl;
    
//     cout << test.getVal(0) << endl;


//     test.writeToFile("./mydata",0);

//     cout << test.getValFromFile("./mydata", 3, 6) << endl;
//     return 0;
// }




// #include "sstheader.h"
// #include "bloomfliter.h"
// #include <iostream>

// using namespace std;

// int main(){
//     SSTheader myheader;
//     myheader.timeStamp = 123;
//     myheader.keyValNum = 100;
//     myheader.minKey = 1;
//     myheader.maxKey = 10;

//     cout << myheader.writeToFile("./mydata",128) << endl;
//     // myheader.writeToFile("./mydata",32);


//     SSTheader myheaderRead;
//     cout << myheaderRead.readFile("./mydata", 0) << endl;
//     cout << myheaderRead.timeStamp << endl;
//     cout << myheaderRead.keyValNum << endl;
//     cout << myheaderRead.minKey << endl;
//     cout << myheaderRead.maxKey << endl;

//     return 0;
// }


// int main(){
//     SSTheader myheader;
//     myheader.timeStamp = 123;
//     myheader.keyValNum = 100;
//     myheader.minKey = 1;
//     myheader.maxKey = 10;
    
//     BloomFliter<int,1024> bf;
//     bf.BFinsert(310);
//     bf.BFinsert(2345);

//     myheader.writeToFile("./mydata", 0);
//     bf.BFwriteToFile("./mydata", 32);


//     SSTheader myread;
//     BloomFliter<int,1024> bfRead;

//     myread.readFile("./mydata", 0);
//     bfRead.BFreadFile("./mydata",32);
    
//     cout << myread.timeStamp  << endl;
//     cout << myread.keyValNum  << endl;
//     cout << myread.minKey << endl;
//     cout << myread.maxKey << endl;
   

//     cout << bfRead.BFfind(310) << endl;
//     cout << bfRead.BFfind(2347) << endl;
    
//     myheader.keyValNum = 100;

//     return 0;
// }







    
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
