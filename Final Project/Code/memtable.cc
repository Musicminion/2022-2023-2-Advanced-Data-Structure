#include "memtable.h"

// WAL日志相关
const int log_putID = 0;
const int log_delID = 1;
const std::string log_putStr = "PUT";
const std::string log_delStr = "DEL";


MemTable::MemTable()
{
    skiplist = new Skiplist<uint64_t, std::string>();
    // 初始化占用的sst大小为 头部的header和bf过滤器的size
    sstSpaceSize = sstable_headerSize + sstable_bfSize;

    this->restoreFromLog(logFilePath);
}


MemTable::~MemTable()
{
    utils::rmfile(logFilePath);
    delete skiplist;
}

void MemTable::putKV(uint64_t key, const std::string &s){
    // 是否发起安全检查待考证！
    // 确保安全，再发起一次插入检查
    // if(this->putCheck(key, s) == false)
    //     return;
    // 插入节点之前，先检查节点是否存在
    Node<uint64_t, std::string> * tryFind = this->skiplist->find(key);

    // 插入全新的key-value对，就需要把sstSpaceSize更新
    if(tryFind == NULL){
        this->skiplist->insert(key, s);
        sstSpaceSize += (s.size() + sizeof(uint32_t) + sizeof(uint64_t));
    }
    // 需要比较新插入的和可能存在的已有的 value的大小
    else{
        this->skiplist->insert(key, s);
        // 插入已有的，相当于编辑value，那就要考虑编辑前后的大小
        if(s.size() >= tryFind->val.size()){
            sstSpaceSize += s.size() - tryFind->val.size();
        }
        else{
            sstSpaceSize -= tryFind->val.size() - s.size();
        }
    }
}


bool MemTable::delKV(uint64_t key){
    auto tryFind = this->skiplist->find(key); 
    if(tryFind == NULL)
        return false;
    // 占用的空间要减回去
    sstSpaceSize -= tryFind->val.size();
    this->skiplist->remove(key);
    return true;
}




void MemTable::put(uint64_t key, const std::string &s){
    this->writeLog(logFilePath, log_putID, key, s);
    this->putKV(key, s);
}


bool MemTable::del(uint64_t key){
    this->writeLog(logFilePath, log_delID, key, "");
    return this->delKV(key);
}

/*
*   注意，不存在、和存在一个已经删除的，是两个概念！
 */
std::string MemTable::get(uint64_t key){
    //std::cout << "get :" << key << '\n';
    auto tryFindNode = this->skiplist->find(key);
    if(tryFindNode != NULL){
        std::string result = tryFindNode->val;
        // 检查result是否为删除标记
        if(result == delete_tag)
            return memtable_already_deleted;
        return result;
    }
    return memtable_not_exist;
}

/*
*   重置内存表，会删除所有的内存表里面的数据
 */
void MemTable::reset(){
    utils::rmfile(logFilePath);
    // 重置之后，首先清空跳表数据，一种是直接删指针，一种是调用clear函数，均可
    this->skiplist->clear();
    // delete this->skiplist;
    // skiplist = new Skiplist<uint64_t, std::string>();
    // 然后把当前维护的转换到sst的大小计算出来
    sstSpaceSize = sstable_headerSize + sstable_bfSize;
}

/*
 *  扫描内存表，扫描Key1到Key2直接的数据，
 *  扫描会排除已经删除的数据
 */
void MemTable::scan(uint64_t key1, uint64_t key2, std::map<uint64_t, std::string> &resultMap){
    Node<uint64_t, std::string>* iter = this->skiplist->find(key1);
    // 如果key1不存在 那就手动插入一个 key 1然后再完成之后删除掉。
    if(iter == NULL){
        iter = this->skiplist->insert(key1, "");
        // 跳过自己，因为已经证实自己不存在
        iter = iter->next[0];
        
        while (iter->type == nodeType_Data && iter->key <= key2)
        {
            // 不要把已经删除了的数据插入
            if(iter->val != delete_tag)
                resultMap[iter->key] = iter->val;
            // 删除的数据，必须强制抹掉！内存表时间优先级MAX！
            else
                resultMap.erase(iter->key);
            iter = iter->next[0];
        }
        // 查找完成后记得把自己添加的元素删除
        this->skiplist->remove(key1);
    }
    // 如果key1存在，那就正常查找
    else{
        // 增加检查是否为deleteTag！
        if(iter->key == key1 && iter->val != delete_tag){
            resultMap[iter->key] = iter->val;
        }
        iter = iter->next[0];
        while (iter->type == nodeType_Data && iter->key <= key2)
        {
            if(iter->val != delete_tag)
                resultMap[iter->key] = iter->val;
            iter = iter->next[0];
        }
    }
}

/**
 * 执行插入检查，返回当前数据是否可以插入
 * @param key 要插入的key
 * @param s 要插入的字符
 * @return 返回插入之后，是否会让当前内存表转成sst表超过范围
*/
bool MemTable::putCheck(uint64_t key, const std::string &s){
    // 新增加的keySpace
    size_t keySpace = sizeof(uint64_t) + sizeof(uint32_t);
    // valSpace
    size_t valSpace = s.size();

    Node<uint64_t, std::string> * tryFind = this->skiplist->find(key);

    if(tryFind == NULL){
        if(sstSpaceSize + keySpace + valSpace <= sstable_maxSize)
            return true;
        return false;
    }
    
    std::string originalVal = tryFind->val;
    
    // 新插入的size比旧的还要小，同意插入
    if(s.size() <= originalVal.size())
        return true;
    // 新插入的变大了
    if(s.size() - originalVal.size() + sstSpaceSize <= sstable_maxSize)
        return true;
    return false;
}

/*
*   拷贝内存表，记录内存表的所有数据
 */
void MemTable::copyAll(std::list<std::pair<uint64_t, std::string> > &list){
    this->skiplist->copyAll(list);
}



/**
 * 写日志操作
 * 参考资料：https://blog.csdn.net/hcf999/article/details/77864456
*/

void MemTable::writeLog(std::string path, int operationID, uint64_t key, std::string val){
	// 尝试打开文件
	std::ofstream outFile(path, std::fstream::out| std::fstream::app);

	switch (operationID)
	{
	case log_putID:
		outFile << log_putStr << " " << key << " " << val << "\n";
		break;
	case log_delID:
		outFile << log_delStr << " " << key  << "\n";
		break;
	default:
		break;
	}

	outFile.close();
}



/**
 * 从日志恢复内存表
*/
void MemTable::restoreFromLog(std::string path){
	std::ifstream inFile;
	inFile.open(path);

	std::string operationType;
	uint64_t key;
	std::string val;

	// 如果配置文件存在，那就读取
	if(inFile.is_open()){
		while(inFile >> operationType){
			if(operationType == log_putStr){
				inFile >> key;
				inFile >> val;
				this->putKV(key, val);
			}
			if(operationType == log_delStr){
				inFile >> key;
				this->delKV(key);
			}	
		}
	}
}
