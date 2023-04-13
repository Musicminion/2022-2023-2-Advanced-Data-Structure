## 作业04 BloomFliter

> 在本次作业中需要自行实现一个简单的 Bloom Filter，并按照实验步骤探究 Bloom Filter 各项参数与误
> 报率（False Positive）的关系。

#### 一、设计思路

##### （a）哈希函数的设置

我测试了一下关于stl里面的Hash函数，发现``Hash<int> hash_int`的这个函数，效果非常不好。他直接返回的就是`hash_int(i) == i`，这显然没有任何意义，如果我插入1-100的数据，然后检测101-200的数据，这显然没一个命中的，所以我对于哈希函数进行了重置！由于我另一门课的云操作系统刚做完Lab1里面的网络数据包加密正好用到了字符串加密，所以我就借鉴了一下这个思想。这个函数如下：

```c++
// 三个参数，第一个是要hash的X，第二个是第i个Hash函数
// 根据PDF要求，hash_i = hash(x+i)
int myHashI(int x,  int times, int limit){
    int num = x + times;
    string num_str = to_string(num);
    return hash_str(num_str) % limit;
}
```

- 函数会界接受三个参数，第一个是要hash的X，第二个是第i个Hash函数。
- 根据作业要求，后续的哈希函数$H_i(x)=H_1(x+i)$可由第一个哈希函数简单变化生成
- 为了让这个哈希函数能够控制范围为$[0,limit-1]$，我通过去模的方式进行处理。

##### （b）布隆过滤器

- 整个过滤器的构建其实不是很难，只需要对外暴露相应的元素操作的函数接口即可！
- 在实例初始化的时候，需要指定哈希数组的大小、哈希函数的个数。实例化的时候会根据数组大小动态创建数组，回收的时候delete。
- 然后在插入一个元素的时候，会依次计算hash，把数组对应的元素修改一下。
- 查找元素的时候，会到对应的数组里面的去看（计算出来哈希的位置是否都是1），如果都是1说明可能是已经存在的，反之就是不存在的，把这个结果返回

```c++
class BloomFilter{
private:
    int m;  // 哈希数组的大小
    int k;  // hash函数的个数
    int *data;
public:
    BloomFilter(int set_m, int set_k){
        m = set_m;
        k = set_k;
        data = new int[set_m];

        for(int i = 0; i < m; i++){
            data[i] = 0;
        }
    }
    ~BloomFilter(){
        if(data != NULL)
            delete[] data;
    }
    // 插入一个元素
    void insertNum(int num){
        for(int i = 0; i < k; i++){
            int hashed_val = myHashI(num, i, m);
            data[hashed_val] = 1;
            //cout << "set hashed val to 1 " << hashed_val << endl;
        }
    }
    // 返回是否存在某个元素
    bool findNum(int num){
        //cout << "try find! ############ "  << endl;
        for(int i = 0; i < k; i++){
            int hashed_val = myHashI(num, i, m);
            //cout << "try find! hashed val is : " << hashed_val << endl;
            if(data[hashed_val] == 0){
                return false;
            }
        }
        return true;
    }
};
```

##### （c）更优雅的输出表格

- 为了让表格能够更优雅的输出（类似MySQL的输出，表格能够根据内容长度适度伸展），我百度了一下，找到了一个开源库！[p-ranav/tabulate: Table Maker for Modern C++ (github.com)](https://github.com/p-ranav/tabulate)

- 然后为了把库导入：

  - 首先需要引入头文件，并且把`/include/tabulate`文件夹移动到项目目录里面：

    ```c++
    #include "tabulate/table.hpp"
    ```

  - 然后，在编译的时候，需要加上包含的`include`文件夹

    ```
    g++ -std=c++17 ./main.cpp -o main -I ./
    ```

  - 补充：根据项目要求，必须使用c++17，才能让这个库正常使用。

- 为了验证这个库效果如何。编写一个测试代码：

```c++
#include <tabulate/table.hpp>
using namespace tabulate;

int main() {

  Table universal_constants;
  universal_constants.add_row({"Quantity", "Value"});
  std::cout << universal_constants << std::endl;
}
```

- 如何动态的插入元素呢，我仔细检查了他的源代码，add_row必须要传入一个Row_t类型！

```c++
tabulate::Table resultTable;
using Row_t = std::vector<variant<std::string, const char *, string_view, tabulate::Table>>;
Row_t t;
t.push_back("1");
t.push_back("1");
t.push_back("1");
resultTable.add_row(t);
```

##### （d）makefile的编写

- 这次我尝试了一下自己编写makefile的文件，尽管可能写的不是很好。

```makefile
main:main
	g++ -std=c++17 ./main.cpp -o main -I ./
clean:
	rm ./main
run: 
	./main
```

#### 二、测试思路

##### （a）实现单次测试的抽象

- 要测试这个布隆过滤器，我们把每次一组（m、n、k）抽象为一次测试
- 我测试的是会分两组生产数据，并**确保两组数据完全不一样！不会重复**
  - 一组用来插入，生成的数量根据n来决定
  - 一组用来测试，测试的数据要保证绝对不能曾经被插入
  - 此外不仅要保证组之间的数据具有唯一性，组内的数据也必须要有唯一性。
- 把数据生成好了之后，我们只需要检测**测试组**里面的数据，用我自己编写的查找函数，并统计有多少个找到了。
- 为了让测试数据的量也具有可控制性，noneExitNum给上层提供设置的权利
- 最终，返回错误的概率$P=\frac{测试组被误判为存在的数量}{测试组数据的数量}$

```c++
double bloomTest(int m, int n, int k, int noneExitNum){
    BloomFilter bloomFilter(m, k);
    map<int,bool> preparedData;
    vector<int> noneExistNumData;
    for(int i = 0; i < n; i++){
        int randVal = rand();
        while(preparedData.count(randVal) == 1){
            randVal = rand();
        }
        preparedData[randVal] = 1;
        bloomFilter.insertNum(randVal);
    }

    for(int i = 0; i < noneExitNum; i++){
        int randVal = rand();
        while(preparedData.count(randVal) == 1){
            randVal = rand();
        }
        noneExistNumData.push_back(randVal);
    }

    int wrongNum = 0;
    for(int i = 0; i < noneExitNum; i++){
        if(bloomFilter.findNum(noneExistNumData[i]) == true){
            wrongNum++;
        }
    }

    // return wrongNum;
    return wrongNum * 1.0 / noneExitNum;
}
```

##### （b）实现整次测试的抽象

- 首先，因为我在测试的时候，主要依赖于随机数的生成！随机数依赖于时间，所以，需要初始化种子。
- 然后，我们要设置好对应的$k,m,n$。由于作业PDF里面$\frac{m}{n}$可以取值为$[2,8]$，所以我决定固定$m=403200$，为啥呢，这玩野可以正好被$[2,8]$所有的整数整除！
- 剩下的就只是调用函数的事情了：`bloomTest(m, n, k, 10000)`
- 代码如下！

```c++
void runTest(){
    srand(time(NULL));
    int m = 403200;
    using Row_t = std::vector<variant<std::string, const char *, string_view, tabulate::Table>>;
    tabulate::Table resultTable;
    Row_t row_header;
    row_header.push_back("data");
    for(int k = 1; k <= 8; k++){
        string str = "k = " + to_string(k);
        row_header.push_back(str);
    }
    resultTable.add_row(row_header);

    for(int i = 2; i <=8; i++){
        Row_t row_data;
        string str = "m/n = " + to_string(i);
        row_data.push_back(str);
        for(int k = 1; k <= 8; k++){
            int n = m / i;
            // cout << bloomTest(m, n, k, 10000) << "\t";
            row_data.push_back(to_string(bloomTest(m, n, k, 10000)));
        }
        resultTable.add_row(row_data);
    }
    cout << resultTable << endl;
}

```

#### 四、实验结果：

实验结果如下表格所示（可以说和助教给出的数据基本高度一致）：

<img src="./%E4%BD%9C%E4%B8%9A04%20BloomFliter.assets/%E6%88%AA%E5%B1%8F2023-03-05%2021.12.16.png" alt="截屏2023-03-05 21.12.16" style="zoom:40%;" />

这个是给出的标准值：

<img src="./%E4%BD%9C%E4%B8%9A04%20BloomFliter.assets/%E6%88%AA%E5%B1%8F2023-03-05%2021.13.28.png" alt="截屏2023-03-05 21.13.28" style="zoom:50%;" />

观察规律总结如下：

- 当$\frac{m}{n}$的值固定的时候，随着$k$的增加，误报率先逐渐递减，然后再逐渐递增。
- 根据理论推到的结果，极值点在$k=ln2(\frac{m}{n})$的时候取到最小值，具体在表中的数据反应就是：m/n=2的时候，大概在k=2的时候最小，m/n=4的时候，大概在k=3的时候最小0.14左右。
- 从理论分析：为什么k太大了或者k太小了都不好呢？
  - 如果k太小了每次插入只会标记数组里面的1个位置，不同的元素比较容器出现哈希碰撞，哈希碰撞的概率大大提高，所以K不能太小。
  - 如果k太大了也不好，每次插入标记了太多的元素（试想一下整个表都全部标记了！）那哈希冲突也会发生。
  - 所以基于此来说，K不能太大，也不能太小。
- 当K的值固定的时候，随着m/n的增加，发现误报率逐渐增大，这是为什么呢？因为插入的元素越来越多，到了后面显然就更加容易冲突了，这也是符合我们的理论认知的。

#### 附录：完整代码

```c++
#include<iostream>
#include <functional>
#include <vector>
#include <string>
#include <map>
#include <time.h>
#include "tabulate/table.hpp"

using namespace std;
std::hash<string> hash_str;

// 三个参数，第一个是要hash的X，第二个是第i个Hash函数
// 根据PDF要求，hash_i = hash(x+i)
int myHashI(int x,  int times, int limit){
    int num = x + times;
    string num_str = to_string(num);
    return hash_str(num_str) % limit;
}


class BloomFilter{
private:
    int m;  // 哈希数组的大小
    int k;  // hash函数的个数
    int *data;
public:
    BloomFilter(int set_m, int set_k){
        m = set_m;
        k = set_k;
        data = new int[set_m];

        for(int i = 0; i < m; i++){
            data[i] = 0;
        }
    }
    ~BloomFilter(){
        if(data != NULL)
            delete[] data;
    }
    // 插入一个元素
    void insertNum(int num){
        for(int i = 0; i < k; i++){
            int hashed_val = myHashI(num, i, m);
            data[hashed_val] = 1;
            //cout << "set hashed val to 1 " << hashed_val << endl;
        }
    }
    // 返回是否存在某个元素
    bool findNum(int num){
        //cout << "try find! ############ "  << endl;
        for(int i = 0; i < k; i++){
            int hashed_val = myHashI(num, i, m);
            //cout << "try find! hashed val is : " << hashed_val << endl;
            if(data[hashed_val] == 0){
                return false;
            }
        }
        return true;
    }
};

// 不妨固定 m = 403200 (1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 = 40320, 然后我再乘以了100)
// m/n      2      3         4       5       6       7       8 
// n      202600  134400   100800   80640   67200   57600   50400 

double bloomTest(int m, int n, int k, int noneExitNum){
    BloomFilter bloomFilter(m, k);
    map<int,bool> preparedData;
    vector<int> noneExistNumData;
    for(int i = 0; i < n; i++){
        int randVal = rand();
        while(preparedData.count(randVal) == 1){
            randVal = rand();
        }
        preparedData[randVal] = 1;
        bloomFilter.insertNum(randVal);
    }

    for(int i = 0; i < noneExitNum; i++){
        int randVal = rand();
        while(preparedData.count(randVal) == 1){
            randVal = rand();
        }
        noneExistNumData.push_back(randVal);
    }

    int wrongNum = 0;
    for(int i = 0; i < noneExitNum; i++){
        if(bloomFilter.findNum(noneExistNumData[i]) == true){
            wrongNum++;
        }
    }

    // return wrongNum;
    return wrongNum * 1.0 / noneExitNum;
}


   
void runTest(){
    srand(time(NULL));
    int m = 403200;
    using Row_t = std::vector<variant<std::string, const char *, string_view, tabulate::Table>>;
    tabulate::Table resultTable;
    Row_t row_header;
    row_header.push_back("data");
    for(int k = 1; k <= 8; k++){
        string str = "k = " + to_string(k);
        row_header.push_back(str);
    }
    resultTable.add_row(row_header);

    for(int i = 2; i <=8; i++){
        Row_t row_data;
        string str = "m/n = " + to_string(i);
        row_data.push_back(str);
        for(int k = 1; k <= 8; k++){
            int n = m / i;
            // cout << bloomTest(m, n, k, 10000) << "\t";
            row_data.push_back(to_string(bloomTest(m, n, k, 10000)));
        }
        resultTable.add_row(row_data);
    }
    cout << resultTable << endl;
}


int main(){
    runTest();
    return 0;
}
```







