#pragma once

#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <map>
#include <list>

// 三种节点类型
const int nodeType_Head = 1;
const int nodeType_Data = 2;
const int nodeType_End = 3;
const int MAX_Level = 8;
const double Jump_Probaility = 0.5;

// 跳表的节点，包括四种类型的数据：右侧节点、上层节点、当前节点的Key和Val
template<typename K, typename V>
struct Node{
    K key;
    V val;
    int type;
    std::vector<Node<K, V>*> next;

    // 默认的构造函数
    Node();
    // 数据节点的构造函数
    Node(K setKey, V setVal, int height){
        this->key = setKey;
        this->val = setVal;
        this->type = nodeType_Data;

        for (int i = 0; i < height; i++)
        {
            next.push_back(NULL);
        }
        
    };
    // 首部/尾节点的构造函数
    Node(int setType){
        this->type = setType;
    }
    ~Node(){};
};

template<typename K, typename V>
class Skiplist {
private:
    size_t size;
    Node<K, V> *head;
    int randomLevel();
public:
    Skiplist();
    ~Skiplist();

    // 增删查节点的函数
    Node<K,V>* insertNode(K elemKey, V elemVal);
    Node<K,V>* findNode(K elemKey);
    void deleteNode(K elemKey);

    size_t getSize();
    void clear();
    void tranverse();
};

// 产生一个随机的楼层，返回一个大于1,小于等于max level的Int
template<typename K, typename V>
int Skiplist<K,V>::randomLevel(){
    int level = 1;
    while(level < MAX_Level  && (rand() * 1.0 / INT_MAX) <= Jump_Probaility){
        level ++;
    }
    return level;
}

// 初始化一个跳表
template<typename K, typename V>
Skiplist<K,V>::Skiplist(){
    srand(time(NULL));
    // 初始化一个跳表，创建头节点
    head = new Node<K, V>(nodeType_Head);
    // 创建尾部节点
    Node<K, V> *end = new Node<K, V>(nodeType_End);
    // 让头部节点的每一层都指向尾部节点
    for(int i = 0; i < MAX_Level; i++){
        head->next.push_back(end);
        end->next.push_back(NULL);
    }
    // 设置，数据的大小为0
    this->size = 0;
}

// 销毁一个跳表
template<typename K, typename V>
Skiplist<K,V>::~Skiplist(){
    // 清空元素
    this->clear();
    // 删除head节点、尾巴节点
    delete head->next[0];
    delete head;
}


// 插入节点
template<typename K, typename V>
Node<K,V>* Skiplist<K,V>::insertNode(K elemKey, V elemVal){
    std::map<int, std::vector<Node<K,V>*> > findPath;
    // 如果这个节点已经存在，根据要求需要替换里面的数据
    Node<K,V>* tryFind = this->findNode(elemKey);
    if(tryFind != NULL){
        tryFind -> val = elemVal;
        return tryFind;
    }

    // 反之，可以放心的新建节点：
    int newNode_level = randomLevel();
    Node<K,V>* newNode = new Node<K,V>(elemKey, elemVal, newNode_level);
    Node<K,V>* iter = head;

    // 这个while循环是需要把迭代器移动到【要插入位置的前一个节点】！
    // 比如新的节点要插入到 A 和 B 之间，就把iter移动到 A。
    while(iter->next[0]->type != nodeType_End){
        bool ifJump = false;
        // 遍历当前迭代器的楼层
        for(int i = iter->next.size() - 1; i >= 0; i--){
            findPath[i].push_back(iter);
            Node<K,V>* level_next = iter->next[i];
            assert(level_next != NULL);
            // 如果当前层指向的是End节点，下楼继续找
            if(level_next->type == nodeType_End)
                continue;
            // 如果当前层指向的数据层的数据小于我们要插入的，执行跳跃操作
            // 移动迭代器，到下一个节点
            if(level_next->key < elemKey){
                iter = level_next;
                ifJump = true;
                findPath[i].push_back(iter);
                break;
            }
            // 如果当前层指向的数据层的数据大于我们要插入的，老老实实继续下楼
            // 啥都不要做，相当于continue
        }

        // 如果从最高楼下楼到最低楼，都没有执行跳跃操作，说明当前节点
        // 每一个楼层的指向的节点，不是End节点就是比 elemKey 要大的
        // （不可能等于，前面已经排查了等于的情况）
        // 这时候说明，新的节点就插入在iter后面就可以了！
        if(!ifJump)
            break;
    }

    // iter 是经过搜索的得到的目标指针，因为涉及到链表断开重链接，所以必须把两头都拿好！
    Node<K,V>* iterNext = iter->next[0];

    //  --------         ----------      ---------------------
    // |        |       |          |    |                     | 
    // |  iter  |   ->  |  newNode | -> |       iterNext      |
    // |        |       |          |    |                     |
    //  --------          --------       ---------------------


    // 更新上一个节点的 next 容器，需要把较低高度的 next 容器里面的指针，指向新的节点
    // 上一个节点的 next 容器较高的高度的 next 容器里面的指针，不需要任何操作
    // [分界线]？ 新节点的高度及其以下的，就是较低高度，反正就是更高高度
    for(int i = 0; i < std::min(newNode_level, (int)iter->next.size()); i++){
        iter->next[i] = newNode;
    }

    // 更新路径上的，可以到达新节点的楼层
    for(auto mapIter = findPath.begin(); mapIter != findPath.end(); mapIter++){
        // 从小到大遍历经过的路径容器
        int level = mapIter->first;
        // 如果层数超过了newNode的层数，退出
        if(level >= newNode->next.size())
            break;
        // 寻找每一层最接近那个新插入的节点的节点
        Node<K,V>* levelLastNode = mapIter->second[mapIter->second.size() - 1];
        // 把这个节点的后继设为新节点。
        levelLastNode->next[level] = newNode;
    }

    // 当新插入节点的高度比新插入节点后面的那个节点要高的时候，高出去的那一部分要指向后面的节点
    // 所以，还需要更新新插入节点的 next 容器！
    for(int i = 0; i < newNode_level; i++){
        // 从底层往上面爬楼，当前的这一层楼，已经超过iterNext的楼层数量 i > iterNext->next.size() - 1
        while(iterNext->type != nodeType_End && i + 1 > iterNext->next.size()){
            // 执行 iterNext 的跳跃操作！
            // 是否会丢失？不会，因为我们是从下往上更新的，不需要担心丢失原来的iterNext
            iterNext = iterNext->next[iterNext->next.size() - 1];
        }
        newNode->next[i] = iterNext;
    }
    this->size = this->size + 1;
    return newNode;
}

// 查找节点，不存在就返回NULL
template<typename K, typename V>
Node<K,V>* Skiplist<K,V>::findNode(K elemKey){
    Node<K,V>* iter = head;
    
    // 借鉴插入的思路即可，但是略有不同
    // 这个while循环是需要把迭代器移动到【搜索的节点】！
    // 比如新的节点要插入到 A 和 B 之间，就把iter移动到 A。
    while(iter->next[0]->type != nodeType_End){
        bool ifJump = false;
        // 遍历当前迭代器的楼层
        for(int i = iter->next.size() - 1; i >= 0; i--){
            Node<K,V>* level_next = iter->next[i];
            assert(level_next != NULL);
            // 如果当前层指向的是End节点，下楼继续找
            if(level_next->type == nodeType_End)
                continue;
            // 如果当前层指向的数据层的数据小于我们要查找的，执行跳跃操作
            // 移动迭代器，到下一个节点
            if(level_next->key < elemKey){
                iter = level_next;
                ifJump = true;
                break;
            }
            // 如果发现下一个节点就是我要找的
            else if(level_next->key == elemKey){
                return level_next;
            }
            // 如果当前层指向的数据层的数据大于我们要插入的，老老实实继续下楼
            // 啥都不要做，相当于continue
        }

        // 如果从最高楼下楼到最低楼，都没有找到
        // 每一个楼层的指向的节点，不是End节点就是比 elemKey 要大的
        // 这时候说明，要找的节点恰好应该排在iter后面，但是后面又没有
        if(!ifJump)
            break;
    }

    // 经过查找没有找到，返回NULL

    return NULL;
}

// 删除节点
template<typename K, typename V>
void Skiplist<K,V>::deleteNode(K elemKey){
    std::map<int, std::vector<Node<K,V>*> > findPath;
    // 如果这个节点已经存在，根据要求需要替换里面的数据
    Node<K,V>* delNode = this->findNode(elemKey);
    if(delNode == NULL)
        return;

    // 开始查找
    Node<K,V>* iter = head;
    // 同样参考之前写的find的代码，但是删除的时候需要注意，【必须把目标节点的前一个节点拿下！】
    // 这个while循环是需要把迭代器移动到【要删除位置的前一个节点】！
    // 比如 A -> B, 要删除B，就把iter移动到A
    while(iter->next[0]->type != nodeType_End){
        bool ifJump = false;

        // 遍历当前迭代器的楼层
        for(int i = iter->next.size() - 1; i >= 0; i--){
            findPath[i].push_back(iter);
            Node<K,V>* level_next = iter->next[i];

            assert(level_next != NULL);
            // 如果当前层指向的是End节点，下楼继续找
            if(level_next->type == nodeType_End)
                continue;
            // 如果当前层指向的数据层的数据小于我们要查找的，执行跳跃操作
            // 移动迭代器，到下一个节点
            if(level_next->key < elemKey){
                iter = level_next;
                ifJump = true;
                findPath[i].push_back(iter);
                break;
            }
            if(level_next->type == nodeType_Head){
                std::cout << "error! head again\n" << iter->key << '\n';
                this->tranverse();
                exit(0);
            }
            
            // 这里和刚刚的逻辑又不同了，因为哦们要找target节点的前面的一个
            // 【！注意！】如果到了第0层，并且下一个就是elemKey，这时候强制退出循环，我们已经找到
            // 【！注意！】如果在比较高的一层，比如第3层，发现下一个就是elemKey，那这个时候，我们什么也不用做，下楼
            if(i == 0 && level_next->key == elemKey){
                ifJump = false;
                break;
            }

            // 如果当前层指向的数据层的数据大于我们要查找的，老老实实继续下楼
            // 啥都不要做，相当于continue
        }
        if(!ifJump)
            break;
    }

    // 经过while之后，可以肯定iter->next[0] == delNode
    assert(iter->next[0] == delNode);

    //  --------         ----------      ---------------------
    // |        |       |          |    |                     | 
    // |  iter  |   ->  |  delNode | -> |     delNodeNext     |
    // |        |       |          |    |                     |
    //  --------          --------       ---------------------
    Node<K,V>* delNodeNext  = delNode->next[0];

    for(int i = 0; i < std::min(delNodeNext->next.size(), iter->next.size()); i++){
        iter->next[i] = delNodeNext;
    }

    for(auto mapIter = findPath.begin(); mapIter != findPath.end(); mapIter++){
        // 从小到大遍历经过的路径容器
        int level = mapIter->first;

        // 寻找每一层最接近那个新插入的节点的节点
        Node<K,V>* levelLastNode = mapIter->second[mapIter->second.size() - 1];

        while(delNodeNext->type != nodeType_End && level + 1 > delNodeNext->next.size()){
            delNodeNext = delNodeNext->next[delNodeNext->next.size() - 1];
        }

        // 把这个节点的后继设为新节点。
        levelLastNode->next[level] = delNodeNext;
    }
    delete delNode;
    this->size = this->size - 1;
}

// 获取表格里面的数据节点的个数
template<typename K, typename V>
size_t Skiplist<K,V>::getSize(){
    return this->size;
}

// 清空表格里面的数据节点，不包括头结点、尾部节点
template<typename K, typename V>
void Skiplist<K,V>::clear(){
    Node<K,V>* iter = head;
    Node<K,V>* del_node;
    while(iter->type != nodeType_End){
        // 保存当前节点
        del_node = iter;
        // 迭代器后移动
        iter = iter->next[0];

        // 删除掉当前的数据节点
        if(del_node->type == nodeType_Data){
            delete del_node;
        }
    }

    // 把调表的大小设置为 0 
    this->size = 0;

    // 经过while循环，此时的 iter 指向的一定是终止节点！
    // 更新head节点里面的数据
    for (int i = 0; i < MAX_Level; i++)
    {
        head->next[i] = iter;
    }
    
    // 完成清理！
}


template<typename K, typename V>
void Skiplist<K,V>::tranverse(){
    std::cout << "####################\n";
    Node<K,V>* iter = head;
    while(iter->type != nodeType_End){
        if(iter->type == nodeType_Head){
            std::cout << "Head level [";

            for(int i = 0; i < iter->next.size(); i++){
                if(iter->next[i]->type == nodeType_End)
                    std::cout << "END, ";
                else if(iter->next[i]->type == nodeType_Data)
                    std::cout << iter->next[i]->key << ", ";
                else if(iter->next[i]->type == nodeType_Head)
                    std::cout << "Head, ";
            }
            std::cout << "]" << std::endl;
            iter = iter->next[0];
            continue;
        }

        // 输出当前的值
        std::cout << "key: " << iter->key << " value: " << iter->val;
        std::cout << " level[ ";
        for(int i = 0; i < iter->next.size(); i++){
            if(iter->next[i]->type == nodeType_End)
                std::cout << "END, ";
            else if(iter->next[i]->type == nodeType_Data)
                std::cout << iter->next[i]->key << ", ";
            else if(iter->next[i]->type == nodeType_Head)
                std::cout << "Head, ";
        }
        std::cout << "]" << std::endl;

        // 迭代器后移动
        iter = iter->next[0];
    }
}