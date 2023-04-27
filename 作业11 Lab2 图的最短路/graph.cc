#include<iostream>
#include<map>
#include<vector>
#include<climits>
#include<queue>
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
int maxSavedTime = 0;

void readData(){
  cin>>N>>M>>K;
  for(int i=0;i<M;i++){
    int u, v, lower, upper, step;
    cin>>u>>v>>lower>>upper>>step;
    
    // 最大能够节约的时间
    maxSavedTime += upper - lower;
    AllPaths[u][v] = new Path(lower, upper, step);
  }
}

// 获取从起点到终点的路径的权值
int getPathTime(int startTime) {
  // 创建一个vector数组，用来存储每个节点的最快的到达时间
  vector<int> arriveTime(N, INT_MAX);
  // 起点的到达时间是startTime
  arriveTime[0] = startTime;

  // 创建一个队列，用来存储当前正在遍历的节点
  queue<int> q;
  q.push(0);

  // 当队列不为空时，说明还有节点没有遍历完
  while(!q.empty()){
    // 获取当前正在遍历的节点
    int cur = q.front();
    q.pop();

    // 假如当前节点没有邻接节点，那么就跳过
    if (AllPaths.count(cur) == 0){
      continue;
    }
    // 遍历当前节点的所有邻接节点
    for(auto it = AllPaths[cur].begin(); it != AllPaths[cur].end(); it++){
      int next = it->first;
      int power = it->second->getPathPower(arriveTime[cur]);
      // 如果从当前节点到达邻接节点的时间比之前的到达时间要短
      if (arriveTime[next] > arriveTime[cur] + power){
        // 更新到达时间
        arriveTime[next] = arriveTime[cur] + power;
        // 将邻接节点加入队列
        q.push(next);
      }
    }
  }
  
  // 返回终点的到达时间
  return arriveTime[N-1];
}

void freeMemory(){
  for(auto it = AllPaths.begin(); it != AllPaths.end(); it++){
    for(auto it2 = it->second.begin(); it2 != it->second.end(); it2++){
      delete it2->second;
    }
  }
}

int main() {
  readData();
  // 遍历不同的出发时间，找到最短的时间
  int earliestArriveTime = INT_MAX;
  int departTime = 0;
  // 我出发的时间是K的倍数，比如0、K、2K、3K...
  // 路径的权值是周期变化的
  // 我需要确保我出发的时间不能超过最大能够节约的时间
  int iLimit = maxSavedTime / K;
  for (int i = 0; i < iLimit; i++){
    // 出发时间是 i * K 
    int curArriveTime = getPathTime(i * K);
    if (curArriveTime < earliestArriveTime){
      earliestArriveTime = curArriveTime;
      departTime = i * K;
    }
  }
  cout<< departTime << " " << earliestArriveTime << endl;
  freeMemory();
  return 0;
}