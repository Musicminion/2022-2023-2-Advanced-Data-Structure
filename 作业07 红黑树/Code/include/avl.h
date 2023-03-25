#pragma once

namespace avl{
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
            int rotateTimes = 0;

            int getRotateTimes(){
                return rotateTimes;
            }

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
                std::cout << std::endl;
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
                this->rotateTimes++;
                node<T> * newhead = head->left;
                head->left = newhead->right;
                newhead->right = head;
                head->height = 1+ std::max(height(head->left), height(head->right));
                newhead->height = 1+std::max(height(newhead->left), height(newhead->right));
                return newhead;
            }

            node<T> * leftRotation(node<T> * head){
                this->rotateTimes++;
                node<T> * newhead = head->right;
                head->right = newhead->left;
                newhead->left = head;
                head->height = 1+ std::max(height(head->left), height(head->right));
                newhead->height = 1+ std::max(height(newhead->left), height(newhead->right));
                return newhead;
            }

            // Traverse and print AVL tree
            void inorderUtil(node<T> * head){
                if(head==NULL) return ;
                inorderUtil(head->left);
                std::cout<<head->key<<" ";
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

                head->height = 1 + std::max(height(head->left), height(head->right));
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
}
