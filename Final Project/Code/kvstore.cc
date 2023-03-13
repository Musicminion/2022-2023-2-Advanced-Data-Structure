#include "kvstore.h"
#include "config.h"
#include <string>
#include <sstream>
#include <chrono>

const bool Tiering = 0;
const bool Leveling = 1;
const std::string confFilePath = "./default.conf";

bool cachePolicy[4] = {true, true, true, true};


KVStore::KVStore(const std::string &dir): KVStoreAPI(dir)
{	
	// 初始化目录和已有的最大时间戳
	this->dataDir = dir;
	this->sstMaxTimeStamp = 0;
	// 读取配置文件
	this->readConfig(confFilePath);
	// 根据配置文件执行文件检查，如果存在文件，就读取到缓存
	this->sstFileCheck(dir);

	// 创建MemTable
	this->memTable = new MemTable();
}


KVStore::~KVStore()
{
	// 要检查内存表是否有数据，有的话执行一次写入。

	// 从内存表里面拷贝数据
	std::list<std::pair<uint64_t, std::string> > dataAll;
	this->memTable->copyAll(dataAll);

	if(dataAll.size() > 0){
		// 时间戳获取
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::microseconds nsTime;
		nsTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
		// 文件创建 [文件名称规则，为防止重复，用的时间戳作为文件名]
		this->sstMaxTimeStamp  = this->sstMaxTimeStamp + 1;
		std::string newFilePath = this->dataDir + "/level-0/" + std::to_string(nsTime.count()) + ".sst";
		SStable* newSStable = new SStable(sstMaxTimeStamp, dataAll,  newFilePath , cachePolicy);
		
		// 写入类的索引里面！
		ssTableIndex[0][nsTime.count()] = newSStable;

		// 内存表格重置
		this->memTable->reset();

		// 发起归并检查，递归执行
		int checkResult = this->mergeCheck();
		while(checkResult > -1){
			this->merge(checkResult);
			// 再次发起归并检查 
			checkResult = this->mergeCheck();
		}
	}

	// 先释放掉内存表
	delete this->memTable;

	// 再针对文件index，一个一个删除
	for(auto iterX = ssTableIndex.begin(); iterX != ssTableIndex.end(); iterX ++){
		for(auto iterY = ssTableIndex[iterX->first].begin(); iterY !=  ssTableIndex[iterX->first].end(); iterY++){
			delete iterY->second;
		}
	}
}



/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{	
	// 发起插入检查！通过插入检查之后，直接插入，退出
	if(this->memTable->putCheck(key,s)){
		this->memTable->put(key, s);
		return;
	}
	// std::cout << "开始写入磁盘 \n";
	// 插入检查失败。发起写入内存
	// 从内存表里面拷贝数据
	std::list<std::pair<uint64_t, std::string> > dataAll;
	this->memTable->copyAll(dataAll);

	// 时间戳获取
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::microseconds msTime;
    msTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
	// 文件创建 [文件名称规则，为防止重复，用的时间戳作为文件名]
	this->sstMaxTimeStamp  = this->sstMaxTimeStamp + 1;
	std::string newFilePath = this->dataDir + "/level-0/" + std::to_string(msTime.count()) + ".sst";
	SStable* newSStable = new SStable(sstMaxTimeStamp, dataAll,  newFilePath , cachePolicy);
	
	
	ssTableIndex[0][msTime.count()] = newSStable;

	// 内存表格重置
	this->memTable->reset();

	// 发起归并检查，递归执行
	int checkResult = this->mergeCheck();

	while(checkResult != -1){
		this->merge(checkResult);
		checkResult = this->mergeCheck();
	}
	this->memTable->put(key, s);
}

/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{	
	std::string result = this->memTable->get(key);
	
	// 内存表里面找完了，看是否存在，存在就返回
	// 发现内存表已经有了删了标记
	if(result == memtable_already_deleted)
		return "";

	if(result != memtable_not_exist)
		return result;

	// 不存在，那就进入sst查找模式
	uint64_t findLatestTimeStamp = 0;
	for (auto iterX = ssTableIndex.begin(); iterX != ssTableIndex.end(); iterX++)
	{
		for(auto iterY = iterX->second.begin(); iterY != iterX->second.end(); iterY++){

			SStable * curSST = iterY->second;
			// 检查是否可能存在
			if(curSST->checkIfExist(key)){
				uint32_t indexResult = curSST->getKeyIndexByKey(key);
				if(indexResult == UINT32_MAX)
					continue;
				std::string valueGet = curSST->getSStableValue(indexResult);
				if(valueGet == sstable_outOfRange)
					continue;
				if(curSST->getSStableTimeStamp() >= findLatestTimeStamp){
					findLatestTimeStamp = curSST->getSStableTimeStamp();
					result = valueGet;
				}
			}
		}
	}

	if(result == delete_tag || result == memtable_not_exist)
		return "";
	return result;
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	// std::cout << "尝试删除：" << key << '\n';
	if(this->get(key) == "")
		return false;
	this->put(key, delete_tag);
	return true;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	this->memTable->reset();
	// 再针对文件index，一个一个删除
	for(auto iterX = ssTableIndex.begin(); iterX != ssTableIndex.end(); iterX ++){
		for(auto iterY = ssTableIndex[iterX->first].begin(); iterY !=  ssTableIndex[iterX->first].end(); iterY++){
			iterY->second->clear();
			delete iterY->second;
		}
	}
	this->ssTableIndex.clear();
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
 * 写入配置文件，把新增加的层的数据写入到配置文件的里面去
 * @param path : 配置文件的路径(相对路径，默认为"./default.conf")
*/
void KVStore::writeConfig(std::string path){
	/**
	 * To Do 
	*/
}


/** 
 * 根据阅读的配置文件，检查相关目录下面的sst文件
 * @param dataPath : 数据文件的目录，会在这个目录下面根据配置文件检查level-X目录
 * 检查完成之后，会尝试读取不同层里面可能存在的sst文件，如果有的话读取，没有的话不读取
*/
void KVStore::sstFileCheck(std::string dataPath){
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

		std::vector<std::string> scanResult;
		utils::scanDir(levelPathStr, scanResult);

		// 针对扫描的所有文件，尝试读取
		for(size_t i = 0; i <scanResult.size(); i++){
			std::string fileName = scanResult[i];
			std::string fileID = fileName.substr(0, fileName.find('.'));
			
			uint64_t fileIDNum = 0; 
			std::istringstream iss(fileID); 
			iss >> fileIDNum;

			std::string fullPath = levelPathStr + "/" + fileName;
			SStable * newTable = new SStable(fullPath, cachePolicy);

			// 初始化读取的时候，更新当前最大的时间戳
			this->sstMaxTimeStamp = std::max(newTable->getSStableTimeStamp(), this->sstMaxTimeStamp);
			
			ssTableIndex[iter->first][fileIDNum] = newTable;
		}
	}

}


/**
 * 归并检查，如果返回-1，代表没有问题，反之返回从第i层(最小的一个i)出现文件超过限制
 * 举例：假如第Level0有两个文件，Level1有5个文件，那就会返回”1“
 * @return 从level-0开始往下，首先出现文件异常的楼层，返回-1代表正常，通过归并检查。
*/
int KVStore::mergeCheck(){
	for(auto iterX = ssTableIndex.begin(); iterX != ssTableIndex.end(); iterX ++){
		// iterX->second.size() 代表当前楼层的文件数量
		// iterX->first 代表当前的楼层
		if(config_level_limit[iterX->first] < iterX->second.size())
			return iterX->first;
	}
	return -1;
}


/**
 * 发起归并，归并X层和X+1层！
*/
void KVStore::merge(uint64_t X){
	// 检查X+1层是否存在，不存在就创建一层
	if(config_level_limit.count(X+1) == 0){
		config_level_limit[X + 1] = config_level_limit[X] * 2;
		// 新建的层都默认是Leveling
		config_level_type[X + 1] = Leveling;
		
		// 新建的层还需要创建目录
		std::string levelPathStr = this->dataDir + "/" + "level-" + std::to_string(X+1);
		utils::mkdir(levelPathStr.c_str());
	}
	// 现在我们需要进入sstable选择阶段，选择好sstable用map存储
	// ssTableSelect[层数][时间戳]
	std::map<uint64_t, std::map<uint64_t, SStable*> > ssTableSelect;
	// 如果X层是Tiering，那就需要全部选中
	if(config_level_type[X] == Tiering){
		for(auto iter = ssTableIndex[X].begin(); iter != ssTableIndex[X].end(); iter++){
			ssTableSelect[X][iter->first] = iter->second;
		}
	}
	// Leveling的话就需要选择时间戳最小的几个文件
	else if(config_level_type[X] == Leveling){
		// 确定要选择的文件数量
		int selectNum = ssTableIndex[X].size() - config_level_limit[X];
		int alreadyChooseNum = 0;
		// auto iter = ssTableIndex[X].begin();

		// 需要根据文件里面的时间戳，选择前面的selectNum个
		// 创建一个排序用的表 sortMap[时间戳][minKey]
		std::map<uint64_t, std::map<uint64_t, SStable*> > sortMap;
		std::map<uint64_t, std::map<uint64_t, uint64_t> > tableName;

		for(auto iter = ssTableIndex[X].begin(); iter != ssTableIndex[X].end(); iter++){
			// iter->first 代表文件名.sst前面的数字
			SStable * curTable = iter->second;
			uint64_t timeStamp = curTable->getSStableTimeStamp();
			uint64_t minKey = curTable->getSStableMinKey();
			
			sortMap[timeStamp][minKey] = curTable;
			tableName[timeStamp][minKey] = iter->first;
		}

		for(auto iterX = sortMap.begin(); iterX != sortMap.end(); iterX++){
			for(auto iterY = iterX->second.begin(); iterY != iterX->second.end(); iterY++){
				if(alreadyChooseNum < selectNum){
					uint64_t curFileID = tableName[iterX->first][iterY->first];

					ssTableSelect[X][curFileID] = iterY->second; 
					alreadyChooseNum++;
				}
			}
		}

	}
	
	// 如果X+1层是leveling
	if(config_level_type[X+1] == Leveling){
		
		if(ssTableSelect[X].size() > 0){
			// 获取最小的key、最大的key！
			uint64_t LevelXminKey = UINT64_MAX;
			uint64_t LevelXmaxKey = 0;
			// iter 遍历 计算LevelX的最小最大key！

			for(auto iter = ssTableSelect[X].begin(); iter != ssTableSelect[X].end(); iter++){
				SStable * curSStable = iter->second;
				LevelXminKey = std::min(LevelXminKey, curSStable->getSStableMinKey());
				LevelXmaxKey = std::max(LevelXmaxKey, curSStable->getSStableMaxKey());
			}
			// 完成遍历后，去Level X+1 层查找！查找有重叠范围的！
			for(auto iter = ssTableIndex[X+1].begin(); iter != ssTableIndex[X+1].end(); iter++){

				SStable * curSStable = iter->second;
				uint64_t curMinKey = curSStable->getSStableMinKey();
				uint64_t curMaxKey = curSStable->getSStableMaxKey();
				// 如果区间没有重叠(那就要求当前的表的 max 小于上一层的 min 或者 当前表的 min 大于上一层的max)
				if(curMaxKey < LevelXminKey || curMinKey > LevelXmaxKey)
					continue;
				// 反之有重叠，选择！
				ssTableSelect[X+1][iter->first] = curSStable;
			}
		}		
	}
	
	// 现在，需要把map变成一维的 ssTableSelectProcessed[时间戳] = 指针
	std::vector<SStable *> ssTableSelectProcessed;
	
	// 排序使用的内存空间
	// sortMap[key][时间戳] = stringValue
	std::map<uint64_t, std::map<uint64_t, std::string> > sortMap;
	// iterX 对应level - i
	
	// 遍历过程中，找到最终写到文件内部的时间戳（根据要求，是所有选择文件里面的最大的时间戳！）
	uint64_t finalWriteFileTimeStamp = 0;

	for(auto iterX = ssTableSelect.begin(); iterX != ssTableSelect.end(); iterX++){
		// iterY->first 对应 时间戳
		// iterY->second 对应指针
		for(auto iterY = ssTableSelect[iterX->first].begin(); iterY != ssTableSelect[iterX->first].end(); iterY++){
			SStable * tableCur = iterY->second;
			ssTableSelectProcessed.push_back(tableCur);
			finalWriteFileTimeStamp = std::max(tableCur->getSStableTimeStamp(), finalWriteFileTimeStamp);
		}
	}


	for(size_t i = 0; i < ssTableSelectProcessed.size(); i++){
		// iter->first 时间戳 iter->second 指针
		size_t target = ssTableSelectProcessed.size() - 1 - i;
		SStable * curTablePt = ssTableSelectProcessed[target];
		uint64_t KVNum = curTablePt->getSStableKeyValNum();
		for (uint64_t i = 0; i < KVNum; i++)
		{
			uint64_t curKey = curTablePt->getSStableKey(i);
			std::string curVal = curTablePt->getSStableValue(i);
			uint64_t timeStamp = curTablePt->getSStableTimeStamp();
			// Bug 4744 号：根据就是是个sortMap的问题
			sortMap[curKey][timeStamp] = curVal;
		}
	}

	// 二次处理
	std::map<uint64_t, std::string> sortMapProcessed;


	for(auto iterX = sortMap.begin(); iterX != sortMap.end(); iterX++){
		// if(iterX->first == 4744){
		// 	std::cout << "开始输出4744号的归并信息：\n";
		// 	for(auto iterY = iterX->second.begin();  iterY != iterX->second.end(); iterY++){
		// 		std::cout << "时间戳：" << iterY->first << "的value前10个信息是" << iterY->second.substr(0,10) <<
		// 		"value的长度是" << iterY->second.size() << "\n";
		// 	}
		// 	auto iterY = iterX->second.end();
		// 	iterY--;
		// 	std::cout << X << "层归并，选择的是(前10个)："<<  iterY->second.substr(0,10) << "归并之后时间戳" << finalWriteFileTimeStamp <<  std::endl;
		// 	std::cout << "\n";

		// }

		// uint64_t maxTimeStamp = 0;
		// std::string latestValue = "";
		// for(auto iterY = iterX->second.begin(); iterY != iterX->second.end(); iterY++){
		// 	if(iterY->first > maxTimeStamp){
		// 		maxTimeStamp = iterY->first;
		// 		latestValue = iterY->second;
		// 		sortMapProcessed[maxTimeStamp] = latestValue;
		// 	}
		// }
		auto iterY = iterX->second.end();
		if(iterX->second.size() > 0){
			// 寻找最新的时间戳的信息
			iterY--;
			// 正好是删除tag，根据要求，只有当X+1是最后一层的时候，才能删除(说白了就是X+2层不存在！)
			if(iterY->second == delete_tag && config_level_limit.count(X + 2) == 0){
				continue;
			}
			// 如果是最后一层，那就删除掉deletetag！
			if(iterY->second == delete_tag && ssTableIndex[X + 2].size() == 0){
				continue;
			}
			// 不是删除tag，保留这个元素
			// 此时 iterY->second 指向最新的元素 iterX->first是key
			sortMapProcessed[iterX->first] = iterY->second;
		}
	}
	
	// 释放空间
	sortMap.clear();

	// 可以开始插入list了
	std::list <std::pair<uint64_t, std::string> > list;
	uint64_t listSSTfileSize = sstable_headerSize + sstable_bfSize;
	
	for(auto iter = sortMapProcessed.begin(); iter != sortMapProcessed.end(); iter++){
		uint64_t curKey = iter->first;
		std::string curVal = iter->second;

		// 添加之后的文件增量
		uint64_t addFileSize = sstable_keySize + sstable_keyOffsetSize + curVal.size();

		// 尝试插入list
		if(addFileSize + listSSTfileSize <= sstable_maxSize){
			listSSTfileSize += addFileSize;
			list.push_back({curKey, curVal});
		}
		else{
			// 时间戳获取
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			std::chrono::microseconds msTime;
			msTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());

			std::string newFilePath = this->dataDir + "/level-" + std::to_string(X+1) + "/" + std::to_string(msTime.count()) +".sst";
			SStable* newSStable = new SStable(finalWriteFileTimeStamp, list, newFilePath, cachePolicy);
			this->ssTableIndex[X+1][msTime.count()] = newSStable;
			
			list.clear();
			listSSTfileSize = sstable_headerSize + sstable_bfSize;

			// 归并之后，清空了list，现在就可以push啦！！
			listSSTfileSize += addFileSize;
			list.push_back({curKey, curVal});
		}
	}

	// 如果缓存区域还有数据，继续进行sstable构建
	if(list.size() > 0){
		// 时间戳获取
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::microseconds msTime;
		msTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());

		std::string newFilePath = this->dataDir + "/level-" + std::to_string(X+1) + "/" + std::to_string(msTime.count()) +".sst";
			
		SStable* newSStable = new SStable(finalWriteFileTimeStamp, list, newFilePath, cachePolicy);
		this->ssTableIndex[X+1][msTime.count()] = newSStable;

		list.clear();
		listSSTfileSize = sstable_headerSize + sstable_bfSize;			
	}

	// 接下来要删除掉老的table(不仅要删除文件、删除指针、还要删除在mapIndex里面的索引！)

	
	for(auto iterX = ssTableSelect.begin(); iterX != ssTableSelect.end(); iterX++){
		// iterY->first 对应 时间戳
		// iterY->second 对应指针
		for(auto iterY = ssTableSelect[iterX->first].begin(); iterY != ssTableSelect[iterX->first].end(); iterY++){
			// 先删除文件
			SStable * tableCur = iterY->second;
			tableCur->clear();

			if(tableCur != NULL)
				delete tableCur;
			if(ssTableIndex[iterX->first].count(iterY->first) == 1){
				ssTableIndex[iterX->first].erase(iterY->first);
			}
			
			
		}
	}

}