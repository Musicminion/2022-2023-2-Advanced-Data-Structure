#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>

class SparseMatrix {
private:
  int _row;
  int _col;
  /* TODO: Add any necessary data structure here. */
  /* Map from row to col to Matrix Value */
  std::map<int, std::map<int, int> > rowColMap;
  /* Map from col to row to Matrix Value */
  std::map<int, std::map<int, int> > colRowMap;

public:

  SparseMatrix() = default;

  SparseMatrix(const std::string input_file);

  void to_file(const std::string output_file);

  SparseMatrix operator*(SparseMatrix &right);

  // x is row, y is col, num means array[x][y] += num; 
  void elementAddNum(int rowIndex, int colIndex, int num);

  void setRowNum(int rowNum);
  void setColNum(int colNum);

  int getElementNum(int rowIndex, int colIndex);

};