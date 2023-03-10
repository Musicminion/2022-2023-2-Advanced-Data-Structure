## Project LSM-KV: KVStore using Log-structured Merge Tree


The handout files include two main parts:

- The `KVStoreAPI` class in `kvstore_api.h` that specifies the interface of KVStore.
- Test files including correctness test (`correctness.cc`) and persistence test (`persistence.cc`).

Explanation of each handout file:

```text
.
├── Makefile  // Makefile if you use GNU Make
├── README.md // This readme file
├── correctness.cc // Correctness test, you should not modify this file
├── data      // Data directory used in our test
├── kvstore.cc     // your implementation
├── kvstore.h      // your implementation
├── kvstore_api.h  // KVStoreAPI, you should not modify this file
├── persistence.cc // Persistence test, you should not modify this file
├── utils.h         // Provides some cross-platform file/directory interface
├── MurmurHash3.h  // Provides murmur3 hash function
└── test.h         // Base class for testing, you should not modify this file
```


First have a look at the `kvstore_api.h` file to check functions you need to implement. Then modify the `kvstore.cc` and `kvstore.h` files and feel free to add new class files.

We will use all files with `.cc`, `.cpp`, `.cxx` suffixes to build correctness and persistence tests. Thus, you can use any IDE to finish this project as long as you ensure that all C++ source files are submitted.

For the test files, of course you could modify it to debug your programs. But remember to change it back when you are testing.

Good luck :)

### 代码时间日志
- 先把跳表（SkipList）的写好解决，测试：常见的插入、删除、遍历、操作，保证正常然后下一步【3.8日】
- 规划项目架构（确定项目需要的类和层级依赖关系）【3.8日】
- 压力测试skiptable，测试常见插入、查找项目通过【3.8日】
- 双向开工，熟悉utils.h的文件，完成当kvstore初始化的文件操作【3.9】
- 准备完成sstable的通用接口，然后进行测试（测试文件读写）【3.9】


### 代码逻辑

#### kv-store初始化
- 首先读取配置文件（default.conf）
- 