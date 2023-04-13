#include <fstream>
#include <stdexcept>

#include "SparseMatrix.h"

SparseMatrix::SparseMatrix(const std::string input_file) {
  /* TODO: Your code here. */
  /* read file from file path */
  std::ifstream infile; 
  infile.open(input_file); 

  int rowNum, colNum;
  
  infile >> this->_row;
  infile >> this->_col;

  int rowID, colID, matrixData;
  while (infile >> rowID >> colID >> matrixData)
  {
    rowColMap[rowID][colID] = matrixData;
    colRowMap[colID][rowID] = matrixData;
  }
  infile.close();
}

void SparseMatrix::to_file(const std::string output_file) {
  /* TODO: Your code here. */
  std::ofstream outfile;
  outfile.open(output_file);
  outfile << _row << " " << _col << "\n";
  for(auto iterX = rowColMap.begin(); iterX != rowColMap.end(); iterX++){
    for(auto iterY = iterX->second.begin(); iterY !=iterX->second.end(); iterY++){
      // ensure 0 can't be outPUT
      if(iterY ->second !=0)
        outfile << iterX->first << " " << iterY -> first << " " << iterY ->second  << "\n";
    }
  }
  outfile.close();
}

// 简要介绍一下我的实现思路，因为我是map存储的
// map rowColMap[X][Y] = value, X is row, Y is col
// map colRowMap[Y][X] = value, X is row, Y is col
// 做乘法的时候首先，遍历行，再在每一行里面遍历不为0的列，然后和要做乘法的right做比较，
// 比如第一行，第三列不为0，那就去找右边right的第三行，去里面看不为0的，然后做乘法，如果发现right的第3行 第5列
// 不是0，那好，最终的结果第一行、第五列的结果就是【上面两个矩阵对应元素的】乘积
// 写入到对应的矩阵即可
SparseMatrix SparseMatrix::operator*(const SparseMatrix &right) {
  /* TODO: Your code here. */
  clock_t start,end;
  start = clock();
  try
  {
    /* check if they can be multied */
    if(this->_col != right._row)
      throw "Can't be muilipled";
    
    SparseMatrix result;
    result.setRowNum(_row);
    result.setColNum(right._col);
    /* continue to do muiliple*/
    /* tranverse by ROW, From Row[0] to Row[END] */ 
    for(auto iterX = rowColMap.begin(); iterX != rowColMap.end(); iterX++){
      /* tranverse by Col */ 
      
      for(auto iterY = iterX->second.begin(); iterY !=iterX->second.end(); iterY++){
        // Now array[iterX->first][iterY->first] must match rightArray[iterY->first][??]
        // ?? is the final result array and we use for to find ??
        // outfile << iterX->first << " " << iterY -> first << " " << iterY ->second  << "\n";
        if(right.rowColMap.count(iterY -> first) == 1){
          auto zTarget =right.rowColMap.find(iterY -> first);
          
          for(auto iterZ = zTarget -> second.begin(); iterZ != zTarget ->second.end(); iterZ++){
            result.elementAddNum(iterX->first, iterZ->first, (iterZ->second) * (iterY ->second) );
          }
        }

      }
    }
    end = clock();
    std::cout << double(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
    return result; 
  }
  catch(const std::exception& e)
  {
    std::cout << e.what() << '\n';
  }
  
  return SparseMatrix();
}

void SparseMatrix::elementAddNum(int  rowIndex, int colIndex, int num){
  if(this->rowColMap.count(rowIndex) == 1){
    if(this->rowColMap[rowIndex].count(colIndex) == 1){
      rowColMap[rowIndex][colIndex] += num;
      colRowMap[colIndex][rowIndex] += num;
    }
    else{
      rowColMap[rowIndex][colIndex] = num;
      colRowMap[colIndex][rowIndex] = num;
    }
  }
  else{
    rowColMap[rowIndex][colIndex] = num;
    colRowMap[colIndex][rowIndex] = num;
  }
}


void SparseMatrix::setRowNum(int rowNum){
  this->_row = rowNum;
};
void SparseMatrix::setColNum(int colNum){
  this->_col = colNum;
};


