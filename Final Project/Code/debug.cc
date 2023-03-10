// #include "kvstore.h"
#include "bloomfliter.h"
#include <iostream>

int main(){
    std::cout << "debug程序开始运行" << std::endl;
    BloomFliter<int,50000> bf;
    for(int i = 0; i < 100; i++){
        bf.BFinsert(i);
    }
    std::cout << "插入完成" << std::endl;

    for(int i = 0; i < 100; i++){
        if(bf.BFfind(i))
             std::cout << "插入成功" << std::endl;
    }
    for(int i = 100; i < 300; i++){
        if(bf.BFfind(i))
             std::cout << "missMatch" << std::endl;
    }
    std::cout << "测试完成" << std::endl;
    return 0;
}
