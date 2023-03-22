#include <algorithm>
#include <cstdint>
#include <stack>
#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include <utility>

class RandGenerator {
    private:
        uint64_t seed = 0;
        constexpr static int32_t BASE = 1103515245;
        constexpr static int32_t OFFSET = 12345;
        constexpr static int32_t MOD = 1e9 + 7;

    public:
        uint32_t treap_rand() {
            seed = (BASE * seed + OFFSET) % MOD;
            return seed;
        }

        void reset() {
            seed = 0;
        }
};


template<typename T>
class Treap;

template<typename T>
class TreapNode {

    private:
        T val;
        TreapNode<T> *left;
        TreapNode<T> *right;
        int32_t count; /* number of repeated elements contained in current node */
        int32_t size;  /* number of nodes in current subtree */
        uint32_t weight;
        friend class Treap<T>;

    public:
        TreapNode() : val(T()), left(nullptr), right(nullptr), count(0), size(1), weight(0) {}

        TreapNode(T val,TreapNode *left, TreapNode *right,
                int32_t count, int32_t size, uint32_t weight) :
            val(val), left(left), right(right),
            count(count), size(size), weight(weight) {}
};

template<typename T>
class Treap {

    private:
        RandGenerator rand;

        TreapNode<T> *treap_root = nullptr;

    public:
        TreapNode<T> * find(TreapNode<T> * rootNode, T val) {
            /* Your code here. */
            if(rootNode == nullptr)
                return nullptr;
            if(rootNode->val == val)
                return rootNode;
            else if(rootNode->val > val)
                return find(rootNode->left, val);
            else
                return find(rootNode->right, val);
        }

        TreapNode<T> * merge(TreapNode<T> * leftTree, TreapNode<T> * rightTree){
            // 如果左右有一个不是空的，直接返回
            if(leftTree == nullptr && rightTree != nullptr) {
                return rightTree;
            }
            else if(rightTree == nullptr && leftTree != nullptr){
                return leftTree;
            }

            // 如果左右⼦树都为空，直接返回
            if(leftTree == nullptr && rightTree == nullptr)
                return nullptr;

            // 如果左⼦树根节点的权重⼤于等于右⼦树的权重 
            // 右⼦树递归合并到左⼦树的右⼉⼦中
            if(leftTree->weight >= rightTree->weight){
                leftTree->right = merge(leftTree->right, rightTree);
                leftTree->size = (leftTree->left? leftTree->left->size : 0) + (leftTree->right ? leftTree->right->size : 0) + 1;
                return leftTree;
            }
            // 反之，左⼦树递归合并到右⼦树的左⼉⼦中
            else{
                rightTree->left = merge(leftTree, rightTree->left);
                rightTree->size = (rightTree->left? rightTree->left->size : 0) + (rightTree->right ? rightTree->right->size : 0) + 1;
                return rightTree;
            }
        }

        std::pair<TreapNode<T> *, TreapNode<T>* > split(TreapNode<T> * rootNode,T splitVal){
            if(rootNode == nullptr){
                return std::make_pair(nullptr, nullptr);
            }
            if(rootNode->val <= splitVal){
                std::pair<TreapNode<T> *, TreapNode<T>* > result = split(rootNode->right, splitVal);
                rootNode->right = result.first;
                rootNode->size = (rootNode->left ? rootNode->left->size : 0) + (rootNode->right ? rootNode->right->size : 0) + 1;
                if(result.first != nullptr)
                    result.first->size = (result.first->left ? result.first->left->size : 0) + (result.first->right ? result.first->right->size : 0) + 1;
                if(result.second != nullptr)
                    result.second->size = (result.second->left ? result.second->left->size : 0) + (result.second->right ? result.second->right->size : 0) + 1;
                return std::make_pair(rootNode, result.second);
            }
            else{
                std::pair<TreapNode<T> *, TreapNode<T>* > result = split(rootNode->left, splitVal);
                rootNode->left = result.second;
                rootNode->size = (rootNode->left ? rootNode->left->size : 0) + (rootNode->right ? rootNode->right->size : 0) + 1;
                if(result.first != nullptr)
                    result.first->size = (result.first->left ? result.first->left->size : 0) + (result.first->right ? result.first->right->size : 0) + 1;
                if(result.second != nullptr)
                    result.second->size = (result.second->left ? result.second->left->size : 0) + (result.second->right ? result.second->right->size : 0) + 1;
                return std::make_pair(result.first, rootNode);
            }
        }

        // 将⼦树划分为⼩于val的sub左⼦树 和等于val的sub右⼦树
        // 就我已知这个tree里面全是 <= val 的值了，那我直接去找最大值就好了
        std::pair<TreapNode<T> *, TreapNode<T>* > splitForDelete(TreapNode<T> * rootNode,T splitVal){
            if(rootNode == nullptr){
                return std::make_pair(nullptr, nullptr);
            }
            
            if(rootNode->val == splitVal){
                TreapNode<T> * nodeLeft = rootNode->left;
                rootNode->left = nullptr;
                rootNode->size = (rootNode->left ? rootNode->left->size : 0) + (rootNode->right ? rootNode->right->size : 0) + 1;
                return std::make_pair(nodeLeft, rootNode);
            }

            TreapNode<T>* node = rootNode;
            std::vector<TreapNode<T>* > path;
            path.push_back(node);

            while (node->right != nullptr)
            {
                path.push_back(node->right);
                if(node->right->val == splitVal){
                    TreapNode<T> * partLeft = node->right->left;
                    TreapNode<T> * partRight = node->right;
                    partRight->left = nullptr;
                    // node->right->size = (node->right->left ? node->right->left->size : 0) + (node->right->right ? node->right->right->size : 0) + 1;
                    node->right = partLeft;
                    node->size = (node->left ? node->left->size : 0) + (node->right ? node->right->size : 0) + 1;
                    partRight->size = (partRight->left ? partRight->left->size : 0) + (partRight->right ? partRight->right->size : 0) + 1;
                    
                    // 更新size
                    for(int i = path.size() - 2; i >= 0; i--){
                        path[i]->size = (path[i]->left ? path[i]->left->size : 0) + (path[i]->right ? path[i]->right->size : 0) + 1;
                    }
                    return std::make_pair(rootNode, partRight);
                }
                node = node->right;
            }
            
            return std::make_pair(rootNode, nullptr);
            
            // 递归的写法
            // if(rootNode->val == splitVal){
            //     TreapNode<T> * nodeLeft = rootNode->left;
            //     rootNode->left = nullptr;
            //     rootNode->size = (rootNode->left ? rootNode->left->size : 0) + (rootNode->right ? rootNode->right->size : 0) + 1;

            //     return std::make_pair(nodeLeft, rootNode);
            // }

            // std::pair<TreapNode<T> *, TreapNode<T>* > result = splitForDelete(rootNode->right, splitVal);
            // if(result.first != nullptr){
            //     rootNode->right = result.first;
            //     rootNode->size = (rootNode->left ? rootNode->left->size : 0) + (rootNode->right ? rootNode->right->size : 0) + 1;
            //     return std::make_pair(rootNode, result.second);
            // }
            // else
            //     result.first = rootNode;
            // return std::make_pair(result.first, result.second);
        }


        void insert(T val) { 
            /* Your code here. */
            // 空的树就直接插入
            if(treap_root == nullptr){
                treap_root = new TreapNode<T>();
                treap_root->val = val;
                treap_root->count = 1;
                treap_root->size = 1;
                treap_root->weight = rand.treap_rand();
                treap_root->left = nullptr;
                treap_root->right = nullptr;
                this->pre_traverse();
                return;
            }

            TreapNode<T>* findResult = find(treap_root, val);

            if(findResult != nullptr){
                findResult->count = findResult->count + 1;
                return;
            }

            // 确认是新的数字，需要插入
            // 1. 以val为界限，把树分割成两个树
            std::pair<TreapNode<T> *, TreapNode<T>* > result = split(treap_root, val);

            // 2. 创建⼀个val的节点。
            TreapNode<T>* newNode = new TreapNode<T>();
            newNode->val = val;
            newNode->count = 1;
            newNode->size = 1;
            newNode->weight = rand.treap_rand();
            newNode->left = nullptr;
            newNode->right = nullptr;
            
            TreapNode<T> * mergeResult =  merge(result.first, newNode);
            treap_root = merge(mergeResult, result.second);
        }

        void remove(T val) {
            /* Your code here. */
            TreapNode<T>* findResult = find(treap_root, val);
            if(findResult == nullptr){
                return;
            }
            if(findResult->count > 1){
                findResult->count = findResult->count - 1;
                return;
            }

            // findResult->count == 1 开始删除节点
            // 1. 以val为界限，把树分割成两个树
            std::pair<TreapNode<T> *, TreapNode<T>* > result1 = split(treap_root, val);
            // 2. 将左⼦树划分为⼩于val的sub左⼦树(图中LeftLeftRoot)和等于val的sub右⼦树
            std::pair<TreapNode<T> *, TreapNode<T>* > result2 = splitForDelete(result1.first, val);

            if(result2.second != nullptr){
                result2.second->count = result2.second->count - 1;
                if(result2.second->count == 0){
                    // delete result2.second;
                    removeAllNode(result2.second);
                    result2.second = nullptr;
                }
            }

            // 3. 将sub左⼦树、sub右⼦树(如果没有被删除的话)以及右⼦树进⾏合并
            TreapNode<T> *mergeResult =  merge(result2.first, result2.second);
            treap_root = merge(mergeResult, result1.second);
        }

        void findPre_element(TreapNode<T> * node, T &result, T targetVal){
            if(node == nullptr){
                return;
            }
            findPre_element(node->left, result, targetVal);
            if(node->val < targetVal){
                result = node->val;
            }
            findPre_element(node->right, result, targetVal);
        }

        T pre_element(T val) {
            /* Your code here. */
            T result = -1;
            findPre_element(treap_root, result, val);
            return result;
        }

        void findSuc_element(TreapNode<T> * node, T &result, T targetVal){
            if(node == nullptr){
                return;
            }
            findSuc_element(node->right, result, targetVal);
            if(node->val > targetVal){
                result = node->val;
            }
            findSuc_element(node->left, result, targetVal);
        }

        T suc_element(T val) {
            /* Your code here. */
            T result = -1;
            findSuc_element(treap_root, result, val);
            return result;
        }

        int32_t findValRank(T val, TreapNode<T>* node){
            if(node == nullptr){
                return -1;
            }
            if(node->val == val){
                return (node->left ? node->left->size : 0) + 1;
            }
            else if(node->val < val){
                return (node->left ? node->left->size : 0) + 1 + findValRank(val, node->right);
            }
            else{
                return findValRank(val, node->left);
            }
            return 0;
        }

        // 查询val的排名
        int32_t rank(T val) {
            /* Your code here. */
            if(this->find(treap_root, val) == nullptr){
                return -1;
            }
            return findValRank(val, treap_root);
        }


        int32_t findKth_element(TreapNode<T>* node, int32_t rk){
            if(node == nullptr || rk <= 0 || rk > node->size){
                return -1;
            }
            else if(node->left != nullptr && rk > node->left->size + 1){
                return findKth_element(node->right, rk - node->left->size - 1);
            }
            else if(node->left != nullptr && rk == node->left->size + 1){
                return node->val;
            }
            else if(node->left != nullptr && rk < node->left->size + 1){
                return findKth_element(node->left, rk);
            }
            else if(node->left == nullptr && rk == 1){
                return node->val;
            }
            else
                return findKth_element(node->right, rk - 1);
        }

        int32_t kth_element(int32_t rk) {
            /* Your code here. */
            int32_t result = findKth_element(treap_root, rk);
            return findKth_element(treap_root, rk);
        }

        // 重置整个树
        void clear() {
            /* Your code here. */
            removeAllNode(treap_root);
            treap_root = nullptr;
            rand.reset();
        }

        // 递归删除
        void removeAllNode(TreapNode<T>* node){
            if(node == nullptr){
                return;
            }
            removeAllNode(node->left);
            removeAllNode(node->right);
            delete node;
        }

        void pre_traverseNode(TreapNode<T>* node, std::string &s) {
            /* Your code here. */
            if(node == nullptr){
                return;
            }
            s += std::to_string(node->val) + " ";
            pre_traverseNode(node->left, s);
            pre_traverseNode(node->right, s);
        }

        void print(TreapNode<T>* node){
            if(node == nullptr){
                return;
            }
            

            std::cout << node->val << " ";
            print(node->left);
            print(node->right);
        }
        

        std::string pre_traverse() {
            /* Your code here. */
            if(treap_root == nullptr){
                return "empty";
            }
            std::string result;
            pre_traverseNode(treap_root, result);
            if(result[result.size() - 1] == ' ')
                result.pop_back();
            return result;
        }

        // void sizeTraversal(TreapNode<T>* node){
        //     if(node == nullptr){
        //         return;
        //     }
        //     std::cout << node->size << " ";
        //     sizeTraversal(node->left);
        //     sizeTraversal(node->right);
        //     int correctSize = (node->left ? node->left->size : 0) + (node->right ? node->right->size : 0) + 1;
        //     if(correctSize != node->size){
        //         std::cout << "!!!!!!!!!!寄了\n";
        //         node->size = correctSize;
        //     }
                
        // }

        // void test(){
        //     std::cout << "开测！";
        //     sizeTraversal(treap_root);
        // }
};
