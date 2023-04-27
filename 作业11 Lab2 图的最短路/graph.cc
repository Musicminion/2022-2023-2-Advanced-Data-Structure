#include<iostream>
#include<map>
using namespace std;


class Path
{
  int lower;
  int upper;
  int step;
public:
  Path(int l, int u, int s):lower(l), upper(u), step(s){}
  // 获取当前时间的路径的权值
  int getPathPower(int time){
    // 路径的权值是周期变化的，每个周期内的权值是一样的
    int period = 2 * (upper - lower) / step;
    // 所以取个mod就可以了
    int t = time % period;

    if (t <= (upper - lower) / step){
      return upper - t * step; 
    }
    else{
      t -= (upper - lower) / step;
      return lower + t * step;
    }
  }
};


int N, M, K;
map<int, map<int, Path*> > AllPaths;

void readData(){
  cin>>N>>M>>K;
  for(int i=0;i<M;i++){
    int u, v, lower, upper, step;
    cin>>u>>v>>lower>>upper>>step;
    AllPaths[u][v] = new Path(lower, upper, step);
  }
}

int main() {
  return 0;
}