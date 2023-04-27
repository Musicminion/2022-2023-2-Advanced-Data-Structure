## Lab2实验报告

### 一、实现思路

为了解决这个问题，我参考作业文档里面的提示，把整个Lab设计为下面几个部分。

#### 1）结构体的设计

根据作业问题，首先这是个和图有关的问题，必然可能涉及到一些自定义的结构体。由于这个题目里面节点类型可以直接`int`表示出来，而且和节点（也就是题目里面说的港口）没什么关系，那就不需要。

但是边的数据很复杂，涉及到随着时间的动态变化。而且边里面包含最大、最小、波动步长的信息，比较复杂。我的设计如下：

```c
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
```

- 由于后面的代码需要根据时间获取到当前路径的权值，所以我额外写了一个函数
- 因为权值是周期性变化的，所以只需要取模就可以了。

#### 2）读取数据

- 参考数据的处理

```cpp
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
```

#### 3）计算节点到达时间

- 我设计了`int getPathTime(int startTime)`的函数处理，根据出发时间，计算到达的时间。

```c
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
```

- 函数会返回一个到达终点的时间（绝对时间，而不是相对于起点的时间）
- 然后我看到了作业文档里面的这一句话

> 高启强可以在起点等待边权降低，使需要花费的总时间减少。等待的时间存在上限，超出该上限后，边权降低带来的收益无法弥补推迟出发的惩罚。如何简单估算出这个上限？（不一定需要很紧凑）

- 这时候我们可以设想，我们先计算出来**这个上限的时间。**（怎么理解最大可以节约的时间呢？等待的时间带来的是处罚，但是可能节约的时间是边权值的变小。
- 我这里简单处理，把所有的边的权值的求和（$\sum upper-lower$））然后就计算出来了最大可以节约的时间。如果我等待的时间超过了这个时间，那不行了！无论怎么样都不可能弥补回来，不能再等了，后面的不需要计算！
- 那其实我计算的这个思路只是一很简单的思路。具体来说，随着时间的推移，不同节点的power都在动态的变化，如果我把所有可以到达终点路径里面的所有边的周期$T_i=period[i]$计算出最大公约数，得到所有边的周期$T_{all}$，然后计算在$T_{all}$时间里面，所有的边的和的最大值、最小值，然后求差值，这才是最精确的可以节约的时间。（**但是这样做的时间复杂开销要远远大于之前我求个和，首先要找到路径，然后要计算公约数，太复杂了，所以不妨放宽松一点，假设所有的边都用得到**）
- 所以main函数里面的代码根据临界值，然后处理。
- 最后输出变量相关的值，然后释放内存空间

```cpp
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
```

### 二、回答问题

> 简单描述一下，输入数据满足什么要求时，在起点等待一下再出发反而会让到达终点的时间更少？

- 我们不妨先把问题简单化，假设起点到中间之间没有任何港口，起点A、终点B
- 然后路径的power权值在$t=0$的时候，权值为10。也就是说，假如我0时刻出发，要等到t=10的时候才能到达。我们看看不同的step对于到达时间的影响！
  - 假设step是5。那么在$t=1$的时候，权值就变成了5，这时候出发，在t=6的时候就能到达。（节约了4）
  - 假设step是1，那么在$t=1$的时候，权值就变成了9，这时候出发，在t=10的时候就能到达。（没节约）
  - 假设step是0.5（不妨假设可以为小数吧！），那么在$t=1$的时候，权值就变成了9.5,这时候出发，在t=10.5的时候才能到达。（延长了0.5）
- 从上面的例子就可以看出，假设等待的时间为$t_{wait}$，因为等待带来旅途的节约的时间是$t_{saved}$
- 其中计算$t_{saved}$方法是$t_{saved}=[t_{0}时刻出发最短的路途时间-t_{wait}时刻出发最短路途时间]$
- 如果$t_{wait} > t_{saved}$，也就是节约的时间超过了等待带来的处罚时间，在起点等待一下再出发反而会让到达终点的时间更少
- 如果$t_{wait} = t_{saved}$，节约的时间等于了等待带来的处罚时间，等不等到达的时间一个样
- 如果$t_{wait} < t_{saved}$，也就是节约的时间超过了等待带来的处罚时间，等待反而导致最终到达的时间变晚了，不如不等，立即出发。