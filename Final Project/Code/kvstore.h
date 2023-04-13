#pragma once

#include "kvstore_api.h"
#include "memtable.h"
#include "sstable.h"
#include "utils.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>

class KVStore : public KVStoreAPI {
	// You can add your implementation here
private:
	// 数据的目录
	std::string dataDir;
	// 保存每一层的sst文件的数量限制
	std::map<uint64_t, uint64_t> config_level_limit;
	// 保存每一层的类型，Tiering-0，Leveling-1
	std::map<uint64_t, bool> config_level_type;
	// 保存每一层的索引，ssTableIndex[level-i][写入的时间戳(微秒级)] = 指针
	std::map<uint64_t, std::map<uint64_t, SStable*> > ssTableIndex;
	// 内存表
	MemTable * memTable;

	// kvstore的已经写入时间戳
	uint64_t sstMaxTimeStamp = 0;

	void readConfig(std::string path);
	void writeConfig(std::string path);
	void sstFileCheck(std::string dataPath);
	
	int mergeCheck();
	void merge(uint64_t X);


public:
	// ********************************************************
	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

	void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string> > &list) override;

};
