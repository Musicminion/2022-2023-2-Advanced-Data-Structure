// stu_name: zhangziqian
// stu_ID: 520111910121
// Date: 2023-2-17

// Note: According to rules of template, there seems to be somethings wrong with function signatre
//       so I made some changes as follows:
// ORIGINAL node * rightRotation(node * head).
// Now      node<T> * rightRotation(node<T> * head).
// 


#include<iostream>
#include<vector>
#include <stdlib.h>
#include <time.h> 

using namespace std;

template <typename T>
class node{
    public:
        T key;
        int height;
        node * left;
        node * right;
        node(T k){
            height = 1;
            key = k;
            left = NULL;
            right = NULL;
        }
};

template <typename T>
class AVL{
    public:
        node<T> * root = NULL;
        int num = 0;

        void insert(T x){
            root=insertUtil(root, x);
        }

        void remove(T x){
            root=removeUtil(root, x);
        }

        node<T> * search(T x){
            return searchUtil(root,x);
        }

        void inorder(){
            inorderUtil(root);
            cout<<endl;
        }
    
        int getTreeHeight(){
            return height(root);
        }

    private:
        int height(node<T> * head){
            if(head==NULL) return 0;
            return head->height;
        }

        node<T> * rightRotation(node<T> * head){
            node<T> * newhead = head->left;
            head->left = newhead->right;
            newhead->right = head;
            head->height = 1+max(height(head->left), height(head->right));
            newhead->height = 1+max(height(newhead->left), height(newhead->right));
            return newhead;
        }

        node<T> * leftRotation(node<T> * head){
            node<T> * newhead = head->right;
            head->right = newhead->left;
            newhead->left = head;
            head->height = 1+max(height(head->left), height(head->right));
            newhead->height = 1+max(height(newhead->left), height(newhead->right));
            return newhead;
        }

        // Traverse and print AVL tree
        void inorderUtil(node<T> * head){
            if(head==NULL) return ;
            inorderUtil(head->left);
            cout<<head->key<<" ";
            inorderUtil(head->right);
        }

        node<T> * insertUtil(node<T> * head, T x){
            // TODO: finish insertUtil
            // If no root node, just create one new node
            if(root == NULL){
                root = new node<T>(x);
                return root;
            }
            
            // If new node x exist already, just return
            if(searchUtil(root, x) != NULL) {
                return searchUtil(root, x);
            }
            
            // now we ensure we will insert a completely new node
            num++;
            return insertNode(head, x);
        }

        node<T> * removeUtil(node<T> * head, T x){
            // TODO: finish removeUtil
            // no exist node will be ignored
            if(searchUtil(root, x) != NULL){
                num--;
            }
            
            // this function use &, so will make head pointer to root or new root
            // so after that, just return head directly!
            deleteNode(head, x);
            return head;
        }

        node<T> * searchUtil(node<T> * head, T x){
            if(head == NULL) {
                return NULL;
            };
            T k = head->key;
            
            if(k == x) return head;
            if(k > x) return searchUtil(head->left, x);
            if(k < x) return searchUtil(head->right, x);
        }

        // ************************************************************************
        //                              工具函数区域
        // ************************************************************************

        node<T> * insertNode(node<T> * &head, T x){
            if(head == NULL){
                head = new node<T>(x);
            }
            else if(x < head->key){
                insertNode(head->left, x);
                if(height(head->left) - height(head->right) ==2){
                    if(x < head->left->key)
                        LL(head);
                    else
                        LR(head);
                }
            }
            else if(x > head->key){
                insertNode(head->right, x);
                if(height(head->right) - height(head->left) == 2){
                    if(x > head->right->key)
                        RR(head);
                    else
                        RL(head);
                }
            }

            head->height = 1 + max(height(head->left), height(head->right));
            return head;
        }

        // Based on leftRotation and rightRotation, build these function!
        void RR(node<T> * & head){
            head = leftRotation(head);
        }
        void RL(node<T> * & head){
            head->right = rightRotation(head->right);
            head = leftRotation(head);
        }
        void LR(node<T> * & head){
            head->left = leftRotation(head -> left);
            head = rightRotation(head);
        }
        void LL(node<T> * & head){
            head = rightRotation(head);
        }

        bool deleteNode(node<T> * &head, T x){
            if(head == NULL)
                return true;
            if(x == head -> key){
                // deleted node is leaf node or has only one child
                if(head -> left == NULL || head -> right == NULL){
                    node<T> * oldNode = head;
                    head = (head ->left != NULL)? head ->left: head->right;
                    delete oldNode;
                    return false;
                }
                // deleted node has two children
                else{ 
                    // Data Structure Page 271, exchange the min value of right tree with head root node
                    // And then delete the node(has the min value of the right tree) 
                    node<T> * tmp = head ->right;
                    while(tmp->left != NULL) tmp = tmp ->left;
                    head->key = tmp ->key;
                    if(deleteNode(head -> right ,tmp->key))
                        return true;
                    return adjust(head, 1);
                }
            }
            // delete on left child tree
            else if(x < head -> key){
                if(deleteNode(head ->left, x))
                    return true;
                else   
                    return adjust(head , 0);
            }
            // delete on right child tree
            else if(x > head -> key){
                if(deleteNode(head ->right, x))
                    return true;
                else   
                    return adjust(head , 0);
            }
        }

        bool adjust(node<T> * &head, int ifRightSubTree){
            // delete on right sub tree
            if(ifRightSubTree){
                if(height(head->left) - height(head->right) == 1)
                    return true;
                if(height(head->right) ==  height(head->left)){
                    --(head ->height);
                    return false;
                }
                if(height(head->left->right) > height(head->left->left)){
                    LR(head);
                    return false;
                }
                LL(head);
                if(height(head->right) == height(head->left))
                    return false;
                else
                    return true;
            }
            else{
                if(height(head->right) - height(head->left) == 1)
                    return true;
                if(height(head->right) ==  height(head->left)){
                    --(head ->height);
                    return false;
                }
                if(height(head->left->right) > height(head->left->left)){
                    RL(head);
                    return false;
                }
                RR(head);
                if(height(head->right) == height(head->left))
                    return false;
                else
                    return true;
            }
        }
};



template <typename T>
class BST{
    // TODO: finish BST according to AVL
    public:
        node<T> * root = NULL;
        int num = 0;

        void insert(T x){
            root=insertUtil(root, x);
        }

        void remove(T x){
            root=removeUtil(root, x);
        }

        node<T> * search(T x){
            return searchUtil(root,x);
        }

        void inorder(){
            inorderUtil(root);
            cout<<endl;
        }

        int getTreeHeight(){
            return height(root);
        }

    private:
        int height(node<T> * head){
            if(head==NULL) return 0;
            return head->height;
        }

        int updateHeight(node<T> * head){
            if(head == NULL)
                return 0;
            else{
                int leftHeight = updateHeight(head->left);
                int rightHeight = updateHeight(head->right);

                head -> height = 1 + max(leftHeight, rightHeight);
                return head -> height;
            }
        }

        void inorderUtil(node<T> * head){
            if(head==NULL) return ;
            inorderUtil(head->left);
            cout<<head->key<<" ";
            inorderUtil(head->right);
        }

        node<T> * searchUtil(node<T> * head, T x){
            if(head == NULL) {
                return NULL;
            };
            T k = head->key;
            
            if(k == x) return head;
            if(k > x) return searchUtil(head->left, x);
            if(k < x) return searchUtil(head->right, x);
        }

        node<T> * insertUtil(node<T> * head, T x){
            // TODO: finish insertUtil
            // If no root node, just create one new node
            if(root == NULL){
                root = new node<T>(x);
                return root;
            }
            
            // If new node x exist already, just return
            if(searchUtil(root, x) != NULL) {
                return searchUtil(root, x);
            }
            
            // now we ensure we will insert a completely new node
            num++;
            node<T> * result = insertNode(head, x);
            updateHeight(head);
            return result;
        }

        node<T> * removeUtil(node<T> * head, T x){
            // TODO: finish removeUtil
            if(searchUtil(head, x) == NULL)
                return head;
            num--;
            deleteNode(head, x);
            return head;
        }
        node<T> * insertNode(node<T> * &head, T x){
            if(head == NULL)
                head = new node<T>(x);
            else if(x < head ->key)
                insertNode(head->left ,x);
            else if(x > head -> key)
                insertNode(head->right, x);
            return head;
        }

        void deleteNode(node<T> * &head, T x){
            if(head == NULL)   return;
            if(x < head->key)   deleteNode(head->left, x);
            else if(x > head->key)   deleteNode(head->right, x);
            else if(head->left != NULL && head->right != NULL){
                node<T> * tmp = head->right;
                while(tmp->left !=NULL) tmp = tmp->left;
                head -> key = tmp ->key;
                deleteNode(head->right, head -> key);
            }
            else{
                node<T> * tmpOldNode = head;
                head = (head ->left !=NULL)? head->left: head->right;
                delete tmpOldNode;
            }
        }
};


void generateDataTest(vector<int> * dataVec, int times){
    srand((unsigned)time(NULL));
    for(int i = 0; i < times; i++){
        dataVec->push_back( rand() );
    }
}



void exervise1(){
    AVL<int> avl;
    avl.insert(35);
    avl.insert(23);
    avl.insert(57);
    avl.insert(9);
    avl.insert(47);
    avl.insert(91);
    avl.insert(37);
    avl.insert(96);
    avl.inorder();
    avl.insert(5);
     avl.inorder();
    avl.insert(42);
     avl.inorder();
    avl.insert(51);
    avl.inorder();
}

void exervise2(){

    AVL<int> avl;
    BST<int> bst;
    clock_t start,end;

    int dataSize = 2000;
    int step = 3;

    int totalSaerch = dataSize / step; 

    vector<int> * data = new vector<int>;
    generateDataTest(data, dataSize);


    start = clock();
    // 计时区域代码开始 计算BST插入的时间
    // **********************************************************************
    for(int i = 0; i < data->size(); i++){
        avl.insert((*data)[i]);
    }
    // 计时区域代码结束
    // **********************************************************************
    end = clock();
    cout << "AVL insert Time = " << double(end-start) / CLOCKS_PER_SEC << "s"<<endl;
    



    start = clock();
    // 计时区域代码开始 计算BST插入的时间
    // **********************************************************************
    for(int i = 0; i < data->size(); i++){
        bst.insert((*data)[i]);
    }
    // 计时区域代码结束
    // **********************************************************************
    end = clock();
    cout << "BST insert Time = " << double(end-start) / CLOCKS_PER_SEC << "s"<<endl;


    // 高度获取
    cout << "[Info] avl height: " << avl.getTreeHeight() << endl;
    cout << "[Info] bst height: " << bst.getTreeHeight() << endl;


    // 搜索的过程我是用搜索部分的方法，因为产生的数据已经放在vec容器里面，直接遍历一下

    start = clock();
    // 计时区域代码开始 计算搜索AVL的时间
    // **********************************************************************
    for(int i = 0; i < data->size(); i+= step){
        avl.search((*data)[i]);
    }
    // 计时区域代码结束
    // **********************************************************************
    end = clock();
    cout << "Avl search time = " << double(end-start) / CLOCKS_PER_SEC << "s" << endl;



    start = clock();
    // 计时区域代码开始 计算搜索BST的时间
    // **********************************************************************
    for(int i = 0; i < data->size(); i+= step){
        bst.search((*data)[i]);
    }
    // 计时区域代码结束
    // **********************************************************************
    end = clock();
    cout << "BST search Time = " << double(end-start) / CLOCKS_PER_SEC<<"s"<<endl;

    cout << "totalSaerch: " << totalSaerch << endl;

}


int main(){
    exervise1();
    exervise2();
    return 0;
}
