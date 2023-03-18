/**
 * 代码来源：https://github.com/BigWheel92/Splay-Tree/blob/master/SplayTree.h
 * 向仓库作者致谢！
*/

#pragma once

#include <iostream>
#include <queue>
using namespace std;

template <typename k, typename v>
struct SplayNode
{
	k key;
	v value;
	SplayNode<k, v>* lChild,
		* rChild,
		* parent;

	SplayNode()
	{
		this->lChild = this->rChild = this->parent = nullptr;
	}

	bool isLeafNode()
	{
		return (!lChild && !rChild);
	}

};


template <typename k, typename v>
class SplayTree
{
	SplayNode<k, v>* root;

	void deleteAll(SplayNode<k, v>* node)
	{
		if (node == nullptr)
			return;

		deleteAll(node->lChild);
		deleteAll(node->rChild);
		delete node;
	}

	void inOrderPrintKeys(SplayNode<k, v>* node) const
	{
		if (node == nullptr)
			return;
		inOrderPrintKeys(node->lChild);
		cout << node->key << ", ";
		inOrderPrintKeys(node->rChild);

	}

	v const* search(SplayNode<k, v>* node, k key)
	{
		if (node->key == key) //key has been found, so splay the node containing this key, and then return the pointer to the value.
		{
			this->splay(node);
			return &root->value;
		}

		if (node->lChild == nullptr && key < node->key) //key does not exist so splay the last accessed node
		{
			this->splay(node);
			return nullptr;
		}

		else if (node->rChild == nullptr && key > node->key) //key does not exist so splay the last accessed node
		{
			this->splay(node);
			return nullptr;
		}

		else if (key < node->key)
		{
			return search(node->lChild, key);
		}

		else
		{
			return search(node->rChild, key);
		}

	}


	void deleteKey(SplayNode<k, v>*& current, k const key)
	{
		//the key to delete does not exist in the tree, so splay the last access node and return from the function.
		if ((key > current->key&& current->rChild == nullptr) || (key < current->key && current->lChild == nullptr))
		{
			splay(current);
			return;
		}

		else if (key > current->key)
		{	//move to the right subtree
			deleteKey(current->rChild, key);
			return;
		}

		else if (key < current->key)
		{	//move to the left subtree
			deleteKey(current->lChild, key);
			return;
		}
		else //current node has the key that we need to delete
		{

			//case 1: the node to delete is the leaf node.
			if (current->isLeafNode() == true)
			{

				SplayNode<k, v>* parent = current->parent;
				delete current;
				current = nullptr;
				splay(parent);
				return;
			}//end of case 1

			//case 2: one of the subtrees of current is nullptr;
			else if (current->lChild == nullptr || current->rChild == nullptr)
			{
				if (current->lChild == nullptr)
				{
					SplayNode<k, v>* nodeToDelete = current;
					SplayNode<k, v>* parent = current->parent;
					current = current->rChild;
					current->parent = parent;
					splay(current->parent);

					delete nodeToDelete;
					return;
				}

				else //right child is nullptr, but left child is not
				{
					SplayNode<k, v>* nodeToDelete = current;
					SplayNode<k, v>* parent = current->parent;
					current = current->lChild;
					current->parent = parent;
					splay(current->parent);

					delete nodeToDelete;
					return;
				}

			}//end of case 2

			else  //case 3: none of the subtrees of node to delete is nullptr.
			{
				SplayNode<k, v>* newNodeToDelete = current->rChild;

				while (newNodeToDelete->lChild != nullptr)
				{
					newNodeToDelete = newNodeToDelete->lChild;
				}

				current->key = newNodeToDelete->key;
				current->value = newNodeToDelete->value;

				//in this new recursive call, case 0 and 3 will never exist. Only case 1 or case 2 will exist.
				deleteKey(current->rChild, current->key);
			}//end of case 3.
		}
	}

	void insert(SplayNode<k, v>* node, k key, v value)
	{
		if (node == nullptr)
		{
			root = new SplayNode<k, v>;
			root->key = key;
			root->value = value;
			return;
		}

		if (node->lChild == nullptr && node->key > key)
		{
			node->lChild = new SplayNode<k, v>();
			node->lChild->key = key;
			node->lChild->value = value;
			node->lChild->parent = node;
			this->splay(node->lChild);
			return;
		}

		else if (node->rChild == nullptr && key > node->key)
		{
			node->rChild = new SplayNode<k, v>();
			node->rChild->key = key;
			node->rChild->value = value;
			node->rChild->parent = node;
			this->splay(node->rChild);
			return;
		}


		else if (key < node->key)
		{
			insert(node->lChild, key, value);
		}

		else if (key > node->key)
		{
			insert(node->rChild, key, value);
		}

		else  //duplicate key, so update the value
		{
			node->value = value;
			this->splay(node);
			return;
		}

	}

	/*T1, T2and T3 are subtrees of the tree
		rooted with y(on the left side) or x(on
			the right side)
		   y                               x
		  / \       Right Rotation        / \
		 x   T3        ------->         T1   y
		/ \            <-------             / \
		T1  T2       Left Rotation         T2  T3
	*/

	SplayNode<k, v>* rightRotate(SplayNode<k, v>* node)
	{
		SplayNode<k, v>* y = node;
		SplayNode<k, v>* yParent = node->parent;
		SplayNode<k, v>* x = y->lChild;
		//SplayNode<k, v> T3 = y->rChild;
		//SplayNode<k, v> T1 = x->lChild;
		SplayNode<k, v>* T2 = x->rChild;


		x->rChild = y;
		y->parent = x;

		y->lChild = T2;

		if (T2 != nullptr)
			T2->parent = y;


		x->parent = yParent;;

		if (x->parent != nullptr)
		{
			if (x->parent->lChild == y) //y is the old child of x's new parent (yParent)
				x->parent->lChild = x;

			else x->parent->rChild = x;

		}
		return x;

	}

	SplayNode<k, v>* leftRotate(SplayNode<k, v>* node)
	{
		SplayNode<k, v>* x = node;
		SplayNode<k, v>* xParent = node->parent;
		SplayNode<k, v>* y = x->rChild;
		SplayNode<k, v>* T2 = y->lChild;

		y->lChild = x;
		x->parent = y;

		x->rChild = T2;

		if (T2 != nullptr)
			T2->parent = x;

		y->parent = xParent;

		if (y->parent != nullptr)
		{
			if (y->parent->lChild == x)  //x is the old child of y's new parent (xParent)
				y->parent->lChild = y;

			else y->parent->rChild = y;
		}
		return y;
	}

	void splay(SplayNode<k, v>* curr)
	{

		if (curr == root || curr == nullptr)
			return;


		//pictorial help for cases: https://www.geeksforgeeks.org/splay-tree-set-1-insert/

		//zig case
		if (curr->parent == root)
		{
			if (curr->parent->lChild == curr)
			{
				root = rightRotate(root);
			}

			else
				root = leftRotate(root);

			return;
		}//end of zig case

		//zig zig case
		else if (curr->parent->lChild == curr && curr->parent->parent->lChild == curr->parent)
		{
			if (curr->parent->parent == root)
			{
				//root ptr of tree will be passed by reference, so root ptr of tree will also change.
				root = rightRotate(root);
				root = rightRotate(root);
				return;
			}
			else
			{
				curr->parent = rightRotate(curr->parent->parent);
				curr = rightRotate(curr->parent);

				splay(curr);
				return;
			}
		} //end of zig zig case

		//zag zag case
		else if (curr->parent->rChild == curr && curr->parent->parent->rChild == curr->parent)
		{
			if (curr->parent->parent == root)
			{

				root = leftRotate(root);
				root = leftRotate(root);
				return;
			}
			else
			{
				curr->parent = leftRotate(curr->parent->parent);
				curr = leftRotate(curr->parent);

				splay(curr);
				return;
			}
		} //end of zag zag case

		//zag-zig cases
		else if (curr->parent->rChild == curr && curr->parent->parent->lChild == curr->parent)
		{
			curr = leftRotate(curr->parent);

			if (curr->parent == root)
			{
				root = rightRotate(root);
				return;
			}
			else
			{
				curr = rightRotate(curr->parent);
				splay(curr);
				return;

			}
		} //end of zag zig case

		//zig zag case
		else if (curr->parent->lChild == curr && curr->parent->parent->rChild == curr->parent)
		{
			curr = rightRotate(curr->parent);

			if (curr->parent == root)
			{
				root = leftRotate(root);
				return;
			}
			else
			{
				curr = leftRotate(curr->parent);
				splay(curr);
				return;

			}

		}
	}//end of splay function

public:

	SplayTree()
	{
		this->root = nullptr;
	}

	void inOrderPrintKeys() const
	{

		inOrderPrintKeys(root);
		cout << endl;
	}
	void levelOrderPrintKeys() const
	{
		queue<SplayNode<k, v>*> nodeQueue;

		if (root != nullptr)
			nodeQueue.push(root);

		while (nodeQueue.empty() == false)
		{
			SplayNode<k, v>* n = nodeQueue.front();
			nodeQueue.pop();

			if (n->lChild != nullptr)
				nodeQueue.push(n->lChild);

			if (n->rChild != nullptr)
				nodeQueue.push(n->rChild);

			cout << n->key << (nodeQueue.empty() == false ? ", " : "\n");

		}
	}

	void insert(k const key, v const value)
	{

		this->insert(root, key, value);
	}

	void deleteKey(k const key)
	{
		this->deleteKey(this->root, key);
	}

	v const* search(k const key)
	{
		return this->search(root, key);
	}

	void deleteAll()
	{
		deleteAll(this->root);
		this->root = nullptr;
	}

	~SplayTree()
	{
		deleteAll(this->root);
	}
};