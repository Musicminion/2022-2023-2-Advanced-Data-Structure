## 红黑树、跳表、AVL对比

> 这次作业要求使用红黑树、跳表和 AVL 树的插入、查询开销。

### 测试数据

我测试了数据集合大小在下面的8种情况下的，输出的结果。

```
const int testSize[] = {512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
```

测试采用两阶段法，首先数据准备阶段，准备好不同大小乱序和顺序的两组数据文件。然后再测试阶段的时候，读取文件然后计算时间。测试数据的生成的代码如下所示。

```cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <string>
#include <map>

using namespace std;

const string DATA_PATH = "./testData/";
const string FilePrefix = "data-";
const int testSize[] = {512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};

void generateData(size_t dataSize, string fileName){
    srand(time(NULL));

    vector<int> testData;
    map<int, bool> record;
    // 生成数据，保证数据不重复
    for(size_t i = 0; i < dataSize; i++){
        int randNum = rand();
        while(record.count(randNum) != 0){
            randNum = rand();
        }
        record[randNum] = true;
        testData.push_back(randNum);
    }

    // 乱序写入文件
    ofstream fout_OrderNone(fileName + "-OrderNone" , ios::out);
    for(size_t i = 0; i < dataSize; i++){
        fout_OrderNone << testData[i] << endl;
    }
    fout_OrderNone.close();

    // 把数据排序
    sort(testData.begin(), testData.end());

    // 顺序写入文件
    ofstream fout_OrderAscend(fileName + "-OrderAscend" , ios::out);
    for(size_t i = 0; i < dataSize; i++){
        fout_OrderAscend << testData[i] << endl;
    }
    fout_OrderAscend.close();
}

int main(){
    
    for(int i = 0; i < 8; i++){
        generateData(testSize[i], DATA_PATH + FilePrefix + to_string(testSize[i]));
    }
    return 0;
}
```

### 测试结果

#### RBT-红黑树

- file部分的数字，例如512代表数据集合的大小
- OrderNone代表乱序，反之代表顺序
- Insert列代表数据的插入的时间
- Search代表数据搜索的时间
- RotateTime代表旋转的次数

| dataStructure | file                              | Insert   | Search   | RotateTime |
| ------------- | --------------------------------- | -------- | -------- | ---------- |
| RBT           | ./testData/data-512-OrderNone     | 0.000197 | 6.00E-05 | 362        |
| RBT           | ./testData/data-512-OrderAscend   | 0.000137 | 5.20E-05 | 496        |
| RBT           | ./testData/data-1024-OrderNone    | 0.000389 | 0.000124 | 718        |
| RBT           | ./testData/data-1024-OrderAscend  | 0.000306 | 0.000114 | 1006       |
| RBT           | ./testData/data-2048-OrderNone    | 0.000666 | 0.000212 | 1435       |
| RBT           | ./testData/data-2048-OrderAscend  | 0.000498 | 0.000204 | 2028       |
| RBT           | ./testData/data-4096-OrderNone    | 0.001261 | 0.000465 | 2810       |
| RBT           | ./testData/data-4096-OrderAscend  | 0.0009   | 0.000379 | 4074       |
| RBT           | ./testData/data-8192-OrderNone    | 0.002288 | 0.000847 | 5658       |
| RBT           | ./testData/data-8192-OrderAscend  | 0.001446 | 0.000633 | 8168       |
| RBT           | ./testData/data-16384-OrderNone   | 0.003956 | 0.001368 | 11138      |
| RBT           | ./testData/data-16384-OrderAscend | 0.002763 | 0.001392 | 16358      |
| RBT           | ./testData/data-32768-OrderNone   | 0.006796 | 0.002622 | 22306      |
| RBT           | ./testData/data-32768-OrderAscend | 0.0056   | 0.002433 | 32740      |
| RBT           | ./testData/data-65536-OrderNone   | 0.01305  | 0.004038 | 44443      |
| RBT           | ./testData/data-65536-OrderAscend | 0.011049 | 0.004191 | 65506      |

- 对比有序和无序的插入，明显可以看出无序的时候效果更快，而且旋转的次数也大大的减少
- 无序的插入旋转的次数可能大概在50~70%的数据量之间，但是有序的插入基本上和数据量恒等

#### 跳表

| dataStructure | file                              | Insert   | Search   |
| ------------- | --------------------------------- | -------- | -------- |
| SkipList      | ./testData/data-512-OrderNone     | 0.004411 | 9.50E-05 |
| SkipList      | ./testData/data-512-OrderAscend   | 0.004842 | 9.90E-05 |
| SkipList      | ./testData/data-1024-OrderNone    | 0.009433 | 0.000225 |
| SkipList      | ./testData/data-1024-OrderAscend  | 0.009055 | 0.000181 |
| SkipList      | ./testData/data-2048-OrderNone    | 0.019443 | 0.000461 |
| SkipList      | ./testData/data-2048-OrderAscend  | 0.020031 | 0.000419 |
| SkipList      | ./testData/data-4096-OrderNone    | 0.040916 | 0.000997 |
| SkipList      | ./testData/data-4096-OrderAscend  | 0.041007 | 0.000888 |
| SkipList      | ./testData/data-8192-OrderNone    | 0.094134 | 0.002511 |
| SkipList      | ./testData/data-8192-OrderAscend  | 0.084757 | 0.001853 |
| SkipList      | ./testData/data-16384-OrderNone   | 0.200466 | 0.0065   |
| SkipList      | ./testData/data-16384-OrderAscend | 0.185259 | 0.004446 |
| SkipList      | ./testData/data-32768-OrderNone   | 0.449256 | 0.017013 |
| SkipList      | ./testData/data-32768-OrderAscend | 0.48799  | 0.014361 |
| SkipList      | ./testData/data-65536-OrderNone   | 1.00503  | 0.046899 |
| SkipList      | ./testData/data-65536-OrderAscend | 1.09848  | 0.037201 |

- 跳表的乱序、顺序没有看出对于插入或者搜索的时间有太大的影响，基本上都是差不多
- 跳表相比较其他的两个Tree，实现起来虽然说容易一些，但是实际测出来的时间复杂度却这么大，可能跟我的实现有关。例如我为了适应期末KV-store，在跳表插入之前执行了检查是否存在的操作，这可能会带来额外的时间开销。

#### AVL树

| dataStructure | file                              | Insert   | Search   | RotateTime |
| ------------- | --------------------------------- | -------- | -------- | ---------- |
| AVL           | ./testData/data-512-OrderNone     | 0.000144 | 3.60E-05 | 351        |
| AVL           | ./testData/data-512-OrderAscend   | 0.000119 | 2.70E-05 | 502        |
| AVL           | ./testData/data-1024-OrderNone    | 0.00031  | 8.00E-05 | 725        |
| AVL           | ./testData/data-1024-OrderAscend  | 0.000256 | 5.80E-05 | 1013       |
| AVL           | ./testData/data-2048-OrderNone    | 0.00073  | 0.000176 | 1437       |
| AVL           | ./testData/data-2048-OrderAscend  | 0.000546 | 0.000117 | 2036       |
| AVL           | ./testData/data-4096-OrderNone    | 0.001459 | 0.000372 | 2880       |
| AVL           | ./testData/data-4096-OrderAscend  | 0.0012   | 0.00026  | 4083       |
| AVL           | ./testData/data-8192-OrderNone    | 0.003324 | 0.000862 | 5929       |
| AVL           | ./testData/data-8192-OrderAscend  | 0.002492 | 0.000546 | 8178       |
| AVL           | ./testData/data-16384-OrderNone   | 0.007233 | 0.001948 | 11504      |
| AVL           | ./testData/data-16384-OrderAscend | 0.005408 | 0.00118  | 16369      |
| AVL           | ./testData/data-32768-OrderNone   | 0.015616 | 0.004583 | 22751      |
| AVL           | ./testData/data-32768-OrderAscend | 0.011528 | 0.00248  | 32752      |
| AVL           | ./testData/data-65536-OrderNone   | 0.033263 | 0.009267 | 45715      |
| AVL           | ./testData/data-65536-OrderAscend | 0.024442 | 0.004838 | 65519      |

- AVL的搜索可以看出明显比插入要快很多，这是因为搜索不涉及任何的树平衡条件的改变，所以快。
- AVL旋转的次数基本和插入的节点数量保持一致，而且尤其当数据有序的时候，插入需要的旋转的次数反而越多，当数据乱序的时候，反而旋转次数要少

### 结论分析

- 首先，跳表的时间复杂度是 O(logn)，同样的道理这两个Tree的时间复杂度也是O(logn)，但是实际上测出来的插入时间差异还比较大。我跳表的代码用的是期末课程项目的跳表，然后AVL是第一次作业的跳表，红黑树属于本次作业完成的内容。究其根本，差异来源于。尽管都是O(logn)，但是在实际实现的程度来说，有的可能是一倍的logn，有的可能是两倍或者三倍的logn（所以**不能说时间复杂度一样最终的时间效果就完全一样**），这也造成了这个数据上的差异。
- 对比有序和无序的插入，对于跳表来说观察到两者的数据差异很小，但是有序和无序对于AVL和红黑树的差异比较大。当数据无序无规则的时候，旋转的次数比较少，当数据有序的时候，插入的点比较固定，带来的旋转次数也增加了很多，所以无序的时候反而效果更好。
- 跳表的时间复杂度在三种数据结构里面尤为突出大，究其原因根具体的代码实现有很大的关系。因为跳表的参数也具有**动态性**，比如最大高度，跳的概率，尽管我们可以控制跳的概率最高，但是调表的最大高度也可以调整，对于不同的数据也需要适应，所以可能反而不如红黑树、AVL那种有固定算法的实现。
- 此外，时间复杂度O(logn)也有一定的差异，推测可能是因为数据过大的时候，因为树的旋转带来了过多的开销。因为旋转的步骤也比较复杂，此外例如红黑树出现调整，有可能会递归式调整，这些因素都可能带来时间开销。
- 综上，但考虑时间复杂度来说 AVL < RBT < SkipList。这样体现出来RBT的平衡性比AVL要弱的结论。

#### 
