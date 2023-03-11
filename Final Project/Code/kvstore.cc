#include "kvstore.h"
#include "config.h"
#include <string>

const bool Tiering = 0;
const bool Leveling = 1;
const std::string confFilePath = "./default.conf";

KVStore::KVStore(const std::string &dir): KVStoreAPI(dir)
{
	// 读取配置文件
	this->readConfig(confFilePath);
	// 根据配置文件执行文件检查，如果存在文件，就读取到缓存
	this->sstFileCheck(dir);

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
	std::string result = this->memTable->get(key);
	if(result != memtable_not_exist)
		return result;
	return "";
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
 * @param path : 配置文件的路径(相对路径，默认为"./default.conf")
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


/** 
 * 根据阅读的配置文件，检查相关目录下面的sst文件
 * @param dataPath : 数据文件的目录，会在这个目录下面根据配置文件检查level-X目录
 * 检查完成之后，会尝试读取不同层里面可能存在的sst文件，如果有的话读取，没有的话不读取
*/
void KVStore::sstFileCheck(std::string dataPath){
	std::cout << dataPath << std::endl;

	// 检查dataPath是否存在，没有的就创建
	if(!utils::dirExists(dataPath))
		utils::mkdir(dataPath.c_str());

	// 根据配置文件，检查level-i文件夹是否存在
	for(auto iter = config_level_limit.begin(); iter != config_level_limit.end(); iter++){
		// 拼接第level-i层的文件
		std::string levelPathStr = dataPath + "/" + "level-" + std::to_string(iter->first);
		// 判断目录是否存在，不存在创建
		if(!utils::dirExists(levelPathStr)){
			utils::mkdir(levelPathStr.c_str());
		}
	}

}