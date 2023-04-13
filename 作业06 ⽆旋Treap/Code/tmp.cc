#include <iostream>
#include "treap.h"

using namespace std;

int main(){
    Treap<int> t;
    t.insert(1);
    t.insert(2);
    t.insert(2);
    t.insert(2);
    t.insert(3);
    t.insert(3);
    t.insert(3);
    t.insert(4);

    // std::cout << "123" << endl;
    // std::cout << t.rank(2) << endl;
    // std::cout << t.rank(4) << endl;

    std::cout << t.kth_element(3);
    std::cout << t.kth_element(9);
    return 0;
}