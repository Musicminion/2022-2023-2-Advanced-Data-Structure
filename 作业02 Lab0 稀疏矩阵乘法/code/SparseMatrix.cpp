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

// 
SparseMatrix SparseMatrix::operator*(const SparseMatrix &right) {
  /* TODO: Your code here. */
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
      // std::cout << "@" << std::endl;
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

