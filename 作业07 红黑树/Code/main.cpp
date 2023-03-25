#include "include/rbtree.h"


int main()
{
    RedBlackTree tree;

    for (int i = 0; i < 10; ++i)
    {
        tree.insert(i + 1);
    }
    tree.inOrderTraversal();
    return 0;
}