#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "SparseMatrix.h"

const int num_case = 3;
std::string grade_cases[num_case];

void compare_res(std::string test_case) {
  std::ifstream stu, ans;
  std::stringstream errstring;
  std::string s_stu, s_ans;
  int lineno = 0;

  std::string name = "output/" + test_case + ".yourans";
  stu.open(name, std::ios::in);
  if (!stu.is_open()) {
    throw std::runtime_error("Failed to open file: " + name);
  }

  name = "output/" + test_case + ".ans";
  ans.open(name, std::ios::in);
  if (!ans.is_open()) {
    throw std::runtime_error("Failed to open file: " + name);
  }



  while (getline(stu, s_stu) && getline(ans, s_ans)) {
    lineno++;
    if (s_stu != s_ans) {
      errstring << "line " << lineno << " mismatch: " << s_stu << "(stu) vs. "
                << s_ans << "(ans)" << std::endl;
      throw std::runtime_error(errstring.str());
    }
  }
  // 学生输出有多余的内容，检测是否为空行
  if(getline(stu, s_stu)){
    // 如果有多余的空行，忽略 
    if(s_stu!=""){
      errstring << "row num mis match";
      throw std::runtime_error(errstring.str());
    }
  }
  // 标准答案还有多余的内容，说明输出内容欠缺
  if(getline(ans, s_ans)){
    errstring << "输出内容缺行" << s_ans;
    throw std::runtime_error(errstring.str());
  }
    

  stu.close();
  ans.close();
}

void grade_test(std::string test_case) {
  SparseMatrix left = SparseMatrix("input/" + test_case + ".left");
  SparseMatrix right = SparseMatrix("input/" + test_case + ".right");

  SparseMatrix ans = left * right;
  ans.to_file("output/" + test_case + ".yourans");

  try {
    compare_res(test_case);
  } catch (std::exception &e) {
    throw std::runtime_error("Failed to pass " + test_case + ":\n\t" +
                             e.what());
  }
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < num_case; i++) {
    grade_cases[i] = "test" + std::to_string(i + 1);
  }

  try {
    if (argc == 2) {
      grade_test(argv[1]);
    } else if (argc == 1) {
      for (auto test_case : grade_cases) {
        grade_test(test_case);
      }
    } else {
      throw std::runtime_error("[usage]: ./sparsematrix [test_case_name]");
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  std::cout << "Congratulations! You've passed all test(s)." << std::endl;
  return 0;
}