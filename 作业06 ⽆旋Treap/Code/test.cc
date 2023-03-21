#include <ios>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
#include <fstream>
#include "treap.h"
#include "util.h"

void generate_output(Treap<int32_t>& treap, int32_t round) {
    int32_t n, op, val;

    JudgeFile input_file(input_path(round), std::ios_base::in);
    auto &fin = input_file.get();

    JudgeFile output_file(output_path(round), std::ios_base::out);
    auto &fout = output_file.get();

    fin >> n;

    for (int i = 1; i <= n; i ++ ) {
        fin >> op >> val;
        switch (op) {
            case 1:
                treap.insert(val);
                fout << treap.pre_traverse() << std::endl;
                break;
            case 2:
                treap.remove(val);
                fout << treap.pre_traverse() << std::endl;
                break;
            case 3: 
                fout << treap.rank(val) << std::endl;
                break;
            case 4:
                fout << treap.kth_element(val) << std::endl;
                break;
            case 5:
                fout << treap.pre_element(val) << std::endl;
                break;
            case 6:
                fout << treap.suc_element(val) << std::endl;
                break;
        }
    }
}

bool check_answer(int32_t round) {
    JudgeFile output_file(output_path(round), std::ios_base::in);
    JudgeFile answer_file(answer_path(round), std::ios_base::in);
    auto &out_handle = output_file.get();
    auto &ans_handle = answer_file.get();
    std::string ol, al;
    bool ret;

    for (int32_t lineno = 0; !out_handle.eof() && !ans_handle.eof(); lineno ++ ) {
        getline(out_handle, ol);
        getline(ans_handle, al);
        if (trim(ol) != trim(al)) {
            std::cerr << "ERROR: Failed at case #" << round << " line:" << lineno << 
                " output:" << ol << " answer:" << al << std::endl;
            return false;
        }
    }
    if (!(ret = out_handle.eof() && ans_handle.eof())) {
        std::cerr << "ERROR: Line number miss match" << std::endl;
    }
    return ret;
}

int main () {
    Treap<int32_t> treap;

    std::cout << "TEST_NUM:" << TEST_NUM << std::endl;
    for (int32_t round = 1; round <= TEST_NUM; round ++ ) {
        try {
            generate_output(treap, round);
            treap.clear();
            if (!check_answer(round))
                return -1;
        } catch(std::exception &e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }
    std::cout << "Tests All Passed :)" << std::endl;
    return 0;
}
