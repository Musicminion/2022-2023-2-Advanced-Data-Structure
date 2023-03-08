#include "kvstore.h"
#include <string>
#include <fstream>
#include <iostream>


KVStore::KVStore(const std::string &dir): KVStoreAPI(dir)
{
	// 读取配置文件
	this->readConfig("./default.conf");
	// 创建MemTable
	this->memTable = new MemTable();
}

KVStore::~KVStore()
{
	delete this->memTable;
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{
	this->memTable->put(key, s);
}

/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
	return this->memTable->get(key);
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	return this->memTable->del(key);;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	this->memTable->reset();
}

/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */
void KVStore::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string> > &list)
{
	this->memTable->scan(key1, key2, list);
}

/** 
 * 阅读配置文件，并且写入到相关变量中
 * Path 配置文件的相对路径
*/
void KVStore::readConfig(std::string path){
	std::ifstream infile;
  	infile.open(path);
	
	uint64_t level,limit;
	std::string levelType;

	while(infile >> level >> limit >> levelType){
		config_level_limit[level] = limit;
		if(levelType == "Leveling")
			config_level_type[level] = Leveling;
		else if(levelType == "Tiering")
			config_level_type[level] = Tiering;
	}

	infile.close();
}