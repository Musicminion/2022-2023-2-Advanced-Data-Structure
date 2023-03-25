#pragma once

#include <iostream>

namespace rbtree{
    enum Color
    {
        RED,
        BLACK
    };

    struct Node
    {
        int key;
        Color color;
        Node *left, *right, *parent;
        Node(int key) : key(key), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    class RedBlackTree
    {
    public:
        RedBlackTree() : root(nullptr) {}

        ~RedBlackTree()
        {
            destroy(root);
        }

        void destroy(Node *node)
        {
            if (node)
            {
                destroy(node->left);
                destroy(node->right);
                delete node;
            }
        }


        /* 插入 key（注意这里没有额外判断 key 是否已经存在，默认不存在）*/
        void insert(const int &key)
        {
            Node *node = new Node(key);
            if (!root)
            {
                root = node;
                root->color = BLACK;
                return;
            }
            Node *parent = findParent(root, key);
            if(parent->key == key){
                delete node;
                return;
            }
            else if (key < parent->key)
            {
                parent->left = node;
            }
            else
            {
                parent->right = node;
            }
            node->parent = parent;
            fixInsertion(node);
        }
        /* 从节点 node 开始中遍历打印红黑树 */
        void inOrderTraversal(Node *node, int level)
        {
            if (node)
            {
                inOrderTraversal(node->left, level + 1);
                std::cout << "[level-" << level << ": ";
                std::cout << (node->color == RED? "R":"B");
                std::cout << node->key << "] ";
                inOrderTraversal(node->right, level + 1);
            }
        }
        /* 从根节点开始遍历打印红黑树 */
        void inOrderTraversal()
        {
            inOrderTraversal(root, 1);
        }

        /* 查找 key */
        bool search(const int &key)
        {
            if(findNode(root, key) == nullptr)
                return false;
            else
                return true;
        }
        
        int getRotateTimes()
        {
            return rotateTimes;
        }


    private:
        Node *root;
        int rotateTimes = 0;

        /* 查找键为 key 的节点 */
        Node *findNode(Node *node, const int &key)
        {
            /* TODO: 查找结点 */
            if(node == nullptr)
                return nullptr;
            if(node->key == key)
                return node;
            else if(node->key < key)
                return findNode(node->left, key);
            else
                return findNode(node->right, key);

        }
        /* 找到键 key 合适的父节点（仅插入时使用）*/
        Node *findParent(Node *node, const int &key)
        {
            if (key < node->key)
            {
                if (!node->left)
                    return node;
                return findParent(node->left, key);
            }
            else if (key > node->key)
            {
                if (!node->right)
                    return node;
                return findParent(node->right, key);
            }
            else
            {
                return node;
            }
        }
        /* 循环向上解决双红情况 */
        void fixInsertion(Node *node)
        {
            while (node != root && node->parent->color == RED)
            {   
                if (node->parent == node->parent->parent->left)
                {
                    Node *uncle = node->parent->parent->right;
                    /* TODO: 根据叔父节点的颜色选择不同的处理方法 */
                    if(uncle == nullptr || uncle->color == BLACK) {
                        // 根据node节点在p节点的左边还是右边选择不同的旋转方式
                        if(node == node->parent->left) {
                            node->parent->color = BLACK;
                            node->parent->parent->color = RED;
                            rotateRight(node->parent->parent);
                        }
                        else{
                            node->color= BLACK;
                            node->parent->parent->color = RED;
                            rotateLeft(node->parent);
                            rotateRight(node->parent);
                        }
                    }
                    else{
                        // uncle节点是红色
                        node->parent->color = BLACK;
                        uncle->color = BLACK;
                        node->parent->parent->color = RED;
                        
                        // 相当于循环检查grandparent节点
                        node = node->parent->parent;
                    }
                    /* NOTE: 注意局部解决后改变 node 的值以继续循环向上 */
                }
                else
                {
                    Node *uncle = node->parent->parent->left;
                    /* TODO: 根据叔父节点的颜色选择不同的处理方法 */
                    if(uncle == nullptr || uncle->color == BLACK) {
                        // 根据node节点在p节点的左边还是右边选择不同的旋转方式
                        if(node == node->parent->right) {
                            node->parent->color = BLACK;
                            node->parent->parent->color = RED;
                            rotateLeft(node->parent->parent);
                        }
                        else{
                            node->color= BLACK;
                            node->parent->parent->color = RED;
                            rotateRight(node->parent);
                            rotateLeft(node->parent);
                        }
                    }
                    else{
                        // uncle节点是红色
                        node->parent->color = BLACK;
                        uncle->color = BLACK;
                        node->parent->parent->color = RED;

                        // 相当于循环检查grandparent节点
                        node = node->parent->parent;
                    }
                    /* NOTE: 注意局部解决后改变 node 的值以继续循环向上 */
                }
            }
            /* 循环到根节点仍然存在双红，则树的高度 +1（修正！应该是树的黑高度加一） */
            root->color = BLACK;
        }

        /* 左旋 */
        /* 看了一眼逻辑很精密，没有问题这部分旋转的代码 */
        void rotateLeft(Node *node)
        {
            this->rotateTimes++;
            Node *rightChild = node->right;
            node->right = rightChild->left;
            if (rightChild->left)
            {
                rightChild->left->parent = node;
            }
            rightChild->parent = node->parent;
            if (!node->parent)
            {
                root = rightChild;
            }
            else if (node == node->parent->left)
            {
                node->parent->left = rightChild;
            }
            else
            {
                node->parent->right = rightChild;
            }
            rightChild->left = node;
            node->parent = rightChild;
        }
        /* 右旋 */
        void rotateRight(Node *node)
        {
            this->rotateTimes++;
            Node *leftChild = node->left;
            node->left = leftChild->right;
            if (leftChild->right)
            {
                leftChild->right->parent = node;
            }
            leftChild->parent = node->parent;
            if (!node->parent)
            {
                root = leftChild;
            }
            else if (node == node->parent->right)
            {
                node->parent->right = leftChild;
            }
            else
            {
                node->parent->left = leftChild;
            }
            leftChild->right = node;
            node->parent = leftChild;
        }
    };
}

