#include<vector>
#include<cstdlib>
#include<iostream>
#include<ctime> 
#include <cmath>


double Jump_Probaility = 0.5;
const int MAX_Level = 20;

// 三种节点类型
const int nodeType_Head  = 0;
const int nodeType_Node  = 1;
const int nodeType_End  = 2;


template <typename T>
class node{
private:
    T value;
    int height;
    int nodeType;
    std::vector<node*> next;
    
public:
    node(T val,int level, int type){
        this->nodeType = type;

        if(type == nodeType_Node){
            this -> value = val;
            this -> height = level;
        }

        if(type == nodeType_Head){
            this->height = MAX_Level;
        }
        
        if(type == nodeType_End){
            this->height = 0;
        }

        // 先放空的吧
        for(int i = 0; i < this->height; i++){
            next.push_back(NULL);
        }
    }

    int getHeight(){
        return next.size();
    }

    void updateHeight(){
        // only update node height
        if(this ->nodeType == nodeType_Node)
            this -> height = next.size();
    }

    int getNodeType(){
        return this->nodeType;
    }

    int setNextVec(node* pt_next, int index){
        if(index < next.size()){
            next[index] = pt_next;
        }
        return 0;
    }

    T getValue(){
        return this->value;
    }

    node* getNextVec(int index){
        if(index < this->next.size() && index >= 0)
            return this->next[index];
        else 
            return NULL;
    }
};

template <typename T>
class skipList{
private:
    node<T>* listhead;
    std::vector <node<T>*> data;
public:
    skipList(){
        // init for rand
        srand(time(NULL));
        node<T>* head = new node<T>(0, MAX_Level, nodeType_Head);
        node<T>* tail = new node<T>(0, 0, nodeType_End);

        for(int i = 0; i < MAX_Level; i++){
            int result = head->setNextVec(tail, i);
        }
        data.push_back(head);
        data.push_back(tail);

        this->listhead = head;
    }

    ~skipList(){
        for(int i = 0; i < data.size(); i++){
            delete data[i];
        }
    }

    void tranverse(){
        //std::cout  << "height value: " << std::endl;
        node<T>* pt = this -> listhead;
        while(pt->getNextVec(0)->getNodeType() != nodeType_End){
            //std::cout  << "height value: " << pt->getHeight() <<  " " << pt->getValue() << std::endl;
            pt = pt->getNextVec(0);
        }
    }

    // Tool function for creat a rand num
    double my_rand();
    
    // generate random level and make it < maxLevel
    int random_level();
    
    // return search path lenght
    int SearchLength(T target);
    
    // insert T value target
    void insertNode(T target);
    
    // delete T value target
    void deleteNode(T target);
};



template <typename T>
double skipList<T>::my_rand(){
    return rand() * 1.0 / INT_MAX;
}

template <typename T>
int skipList<T>::random_level(){
    int level = 1;
    
    while(level < MAX_Level &&  this->my_rand() <= Jump_Probaility){
        level ++;
    }

    return level;
}

template <typename T>
void skipList<T>::insertNode(T target){
    node<T>* pt = this -> listhead;
    while(pt->getNextVec(0)->getNodeType() != nodeType_End){
        bool ifJump = false;
        for(int i = pt->getHeight() - 1; i >= 0; i--){
            //std::cout  << "for  " << i << std::endl;
            node<T>* curLevelNext = pt->getNextVec(i);

            assert(curLevelNext != NULL);
            if(curLevelNext->getNodeType() == nodeType_End)
                continue;
            if(curLevelNext->getValue() == target)
                return;
            if(curLevelNext->getValue() < target){
                pt = curLevelNext;
                ifJump = true;
            }
        }

        if(!ifJump)
            break;
    }

    int new_level = random_level();
    node<T>* new_pt = new node<T>(target, new_level, nodeType_Node);

    // pt 是经过搜索的得到的目标指针，因为涉及到链表断开重链接，所以必须把两头都拿好！
    node<T>* pt_next_original = pt->getNextVec(0);

    //  --------         ----------      ---------------------
    // |        |       |          |    |                     | 
    // |   pt   |   ->  |  new_pt  | -> |   pt_next_original  |
    // |        |       |          |    |                     |
    //  --------          --------       ---------------------

    // 更新上一个节点的vec数组，不同楼层的指针需要更新到合适的
    for(int i = 0; i < new_pt -> getHeight(); i++){
        //std::cout << "update" << i << " " << new_pt->getHeight() << std::endl;
        pt->setNextVec(new_pt, i);
    }

    // 更新新的节点的vec数组
    for(int i = 0; i < new_pt->getHeight(); i++){
        while(pt_next_original->getNodeType() !=nodeType_End && i + 1 > pt_next_original->getHeight()){
            pt_next_original = pt_next_original->getNextVec(pt_next_original->getHeight() - 1);
            
        }
        new_pt->setNextVec(pt_next_original, i);
    }
}

template <typename T>
void skipList<T>::deleteNode(T target){

}


template <typename T>
int skipList<T>::SearchLength(T target){
    int result = 0;
    node<T>* pt = this -> listhead;
    while(pt->getNextVec(0)->getNodeType() != nodeType_End){
        bool ifJump = false;
        for(int i = pt->getHeight() - 1; i >= 0; i--,result++){
            node<T>* curLevelNext = pt->getNextVec(i);
            assert(curLevelNext != NULL);
            if(curLevelNext->getNodeType() == nodeType_End)
                continue;
            if(curLevelNext->getValue() == target)
                return result + 1;
            if(curLevelNext->getValue() < target){
                pt = curLevelNext;
                ifJump = true;
                result+=2;
                break;
            }
        }
        if(!ifJump)
            break;
    }

    std::cout <<"no found" << std:: endl; 
    return -1;
}


double test(int elemSize, double p){
    skipList<int> mylist;

    Jump_Probaility = p;
    std::vector<int> dataAll;
    for(int x = 0; x < elemSize; x++){
        int num = rand();
        dataAll.push_back(num);
        mylist.insertNode(num);
    }

    int times_search = 0;
    double final_result = 0;
    for(int i = 0; i < dataAll.size(); i++){
        times_search++;
        final_result += mylist.SearchLength(dataAll[i]);
    }
    final_result /= times_search;

    return final_result;
}


int main(){
    int elemSizeGroup[8] = {50, 100, 200, 500, 1000, 2000, 5000, 10000};
    double p[4] = {0.5, 1/exp(1), 0.25, 0.125};
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            std::cout << elemSizeGroup[i] << "," << p[j] << ","  << test(elemSizeGroup[i], p[j]) << std::endl;
        }
    }
    return 0;
}


