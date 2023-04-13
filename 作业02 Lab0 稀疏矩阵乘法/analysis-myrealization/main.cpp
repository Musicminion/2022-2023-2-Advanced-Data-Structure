#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <ctime>
#include "SparseMatrix.h"



double getRandNum(){
  return rand() * 1.0 / RAND_MAX;
}

void generateData(int index, int scale, double p){
  SparseMatrix left;
  SparseMatrix right;

  for(int x = 0; x < scale; x++){
    for(int y = 0; y < scale; y++){
      if(getRandNum() < p)
        left.elementAddNum(x,y,rand());
      if(getRandNum() < p)
        right.elementAddNum(x,y, rand());
    }
  }

  left.setColNum(scale);
  left.setRowNum(scale);
  right.setColNum(scale);
  right.setRowNum(scale);

  left.to_file("input/" + std::to_string(index) + ".left");
  left.to_file("input/" + std::to_string(index)  + ".right");
}

void generateFileData(){
  srand(time(NULL));
  int fileID = 0;
  int scaleGroup[] = {10, 50, 100, 200, 250};
  double pGroup[] = {0.001, 0.005, 0.01, 0.05, 0.1, 0.2, 0.3, 0.5, 0.7, 0.9};
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 10; j++){
      generateData(fileID, scaleGroup[i], pGroup[j]);
      fileID++;
    }
  }
}


void runTest(int fileNum){
  for(int i = 0; i < fileNum; i++){
    std::string test_case = std::to_string(i);
    SparseMatrix left = SparseMatrix("input/" + test_case + ".left");
    SparseMatrix right = SparseMatrix("input/" + test_case + ".right");
    SparseMatrix ans = left * right;
  }
}

int main(int argc, char *argv[]) {

  // generateFileData();
  runTest(50);

  return 0;
}