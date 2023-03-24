## Lab1：无旋Treap

### 程序性能分析

#### 测试方法
关于性能分析，由于要求必须使用Cycle作为时钟的单位，所以我本来想参考了PDF里面测量时钟周期的方法，具体来说代码如下：

```cpp
static inline uint64_t rdtsc(){
    uint32_t low, high;
    asm volatile ("rdtsc" : "=a" (low), "=d" (high));
    return ((uint64_t)high << 32) | low;
}

void testFunction(){
    uint64_t start, end;
    start = rdtsc();

    /* 计时器代码开始 */

    /*
    * TODO
    */

    /* 计时器代码结束 */
    end = rdtsc();
    cout << end - start << endl;
}
```

但是，我写了一部分就发现写不下去了。因为我的一些函数都是诸如下面的样子的！因为有一些中途就直接返回了的结果，那我每一次return之前都需要加一段代码，简直是噩梦。
```cpp
int merge(node* argv1, int val){
    if(/***/){
        /* code */
        return 1;
    }
    /* some code */
    /* some code */
    /* some code */
    if(/***/){
        /* code */
        return 2;
    }

    return 3;
}
```