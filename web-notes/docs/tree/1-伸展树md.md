---
sidebar_position: 2
---

### 第01部分 伸展树

#### 一）背景

> 引入：树的计树问题，一个二叉树可以有多少个形状？例如三个节点的二叉树可以有多少个拓扑形状？

我们可以考虑中序遍历（根节点在中间，依次遍历左子树、根节点、右子树），遍历左子树之前把当前节点PUSH，遍历完成左子树当前节点POP出来。本质就是问 $n$ 个数字合法的PUSH和POP序列有多少个。

当然，也可以等价思考为一个 $n \times n$ 的坐标轴网格，起点在$(0,0)$，然后每次只能向右或者向上走一步，并且向上走的步数不能多余向右走的步数！也就是说，路线不可以和 $y=x+1$ 有交点。对于与 $y=x+1$ 有交点的路径，把第一个交点之后的路径沿 $y=x+1$ 对称过去，可以发现每一个与 $y=x+1$ 有交点的路径都唯一对应一条从 $(0,0)$ 到 $(n-1,n+1)$ 的路径。

![image-20230317230512220](./1-%E4%BC%B8%E5%B1%95%E6%A0%91md.assets/image-20230317230512220.png)

这样的路径一共有 $C_{2n}^{n-1}$条，也就是说非法的路径是$C_{2n}^{n-1}$条，总共可能的路径是 $C_{2n}^{n}$ 条，合法的路径是一共有$C_{2n}^{n} - C_{2n}^{n-1}$。

拓展：如果限制条件改为在任一时刻，向右走的次数不能少于(向上走的次数 ± k)，只需要把 $y=x+1$ 上下移动即可。([神奇的卡塔兰(Catalan)数 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/385994583))

#### 二）伸展树的理论基础

- 对于刚刚访问的数据，很有可能再次被访问，这一现象在信息处理的过程很常见。所以节点V一旦被访问，随机转移到树根。
- 将被访问的下一元素，极有可能就处于不久之前被访问过的某个元素的附近

因此，只需将刚被访问的节点，及时地“转移”至树根（附近），即可加速后续的操作。

#### 三）伸展树的伸展方法

##### 1）直接伸展法

一种直接方式是：每访问过一个节点之后，随即反复地以它的父节点为轴，经适当的旋转将 其提升一层，直至最终成为树根。以图8.1为例，若深度为3的节点E刚被访问，无论查找或插入，甚至“删除”，都可通过3次旋转，将该树等价变换为以E为根的另一棵二叉搜索树。

![截屏2023-03-18 12.24.48](./1-%E4%BC%B8%E5%B1%95%E6%A0%91md.assets/%E6%88%AA%E5%B1%8F2023-03-18%2012.24.48.png)

这种方法不好，因为假如依次插入${1,2,3,4,5}$，然后依次搜索，经过一轮搜索之后会还原。这样显然就调整了一个寂寞。若节点总数为 $n$ ，则旋转操作的总次数应为$(n - 1) + { (n - 1) + (n - 2) + ... + 1 } $,时间复杂度 $n^2$。分摊下来，每次访问平均需要 $O(n)$ 时间

![截屏2023-03-18 12.26.04](./1-%E4%BC%B8%E5%B1%95%E6%A0%91md.assets/%E6%88%AA%E5%B1%8F2023-03-18%2012.26.04.png)

##### 2）双层伸展法

> 首先我们还是牢记一点，伸展树最终目标是把节点提升到树的根节点，只不过需要分为多次逐步提升，我们分三类情况，分别介绍具体的处理方法。

第一种就是我们常说的LL，目标节点V在g节点的左边的左边（当然g上面还有什么我们不关心，调完之后再说。）

![截屏2023-03-18 12.33.27](./1-%E4%BC%B8%E5%B1%95%E6%A0%91md.assets/%E6%88%AA%E5%B1%8F2023-03-18%2012.33.27.png)

第二种就是RL情况，v节点在g节点的右边的左边。旋转方法是右旋p，然后左旋g。

![截屏2023-03-18 12.35.38](./1-%E4%BC%B8%E5%B1%95%E6%A0%91md.assets/%E6%88%AA%E5%B1%8F2023-03-18%2012.35.38.png)

剩下的RR和LR以此类推即可。

第三种就是L或者R，为什么会有这种情况呢？因为上面两种都是把V节点提了两层，那么就要分情况讨论，最后一次提升的时候，是吧节点直接提到了Root节点，还是Root的一个儿子节点呢，所以这时候需要考虑提升一层的情况。如下所示

![截屏2023-03-18 12.40.21](./1-%E4%BC%B8%E5%B1%95%E6%A0%91md.assets/%E6%88%AA%E5%B1%8F2023-03-18%2012.40.21.png)

势能分析法（potential analysis）业已证明，在改用“双 层伸展”策略之后，伸展树的单次操作均可在分摊的$O(logn)$ 的时间内完成。

#### d）伸展树的算法实现

##### 1）查找算法

查找算法实现：

- 在伸展树中查找Key
- 将最后一个被访问的节点伸展至根

调用二叉搜索树的通用算法searchIn()尝试查找具有关键码e的节点。 无论查找是否成功，都继而调用splay()算法，将查找终止位置处的节点伸展到树根。

```c++
template <typename T> BinNodePosi(T) & Splay<T>::search(const T& e) { 
		//在伸展树中查找关键码e
		BinNodePosi(T) p = searchIn(_root, e, _hot = NULL);
		_root = splay((p ? p : _hot)); //将最后一个被讵问癿节点伸展至根
		return _root; 
}
```

##### 2）插入算法

为将关键码e插至伸展树T中，首先调用伸展树查找接口`Splay::search(e)`， 查找该关键码（图(a)）。于是，其中最后被访问的节点t，将通过伸展被提升为树根，其左、 右子树分别记作 $T_L$ 和 $T_R$（图(b)）。

接下来，根据 $e$ 与 $t$ 的大小关系（不妨排除二者相等的情况），以 $t$ 为界将 $T$ 分裂为两棵子树。 比如，不失一般性地设 $e$ 大于 $t$ 。于是，可切断t与其右孩子之间的联系（图(c)），再将以 $e$ 为关键码的新节点 $v$ 作为树根，并以t作为其左孩子，以 $T_R$ 作为其右子树（图(d)）。

![截屏2023-03-18 16.31.32](./1-%E4%BC%B8%E5%B1%95%E6%A0%91md.assets/%E6%88%AA%E5%B1%8F2023-03-18%2016.31.32.png)

##### 3）删除算法

如下图所示，为了从伸展树 $T$ 中删除关键码为 $e$ 的节点，首先亦调用接口`Splay::search(e)`， 查找该关键码，且不妨设命中节点为 $t$图(a)）。于是，$v$ 将随即通过伸展被提升为树根，其左、 右子树分别记作$T_L$和$T_R$（图(b)）。接下来，将 $v$ 摘除（图(c)）。然后，在 $T_R$ 中再次查找关键码 $e$。 尽管这一查找注定失败，却可以将$T_R$ 中的最小节点 $m$，伸展提升为该子树的根。最后把 $m$ 和左子树接上即可！

![截屏2023-03-18 16.35.12](./1-%E4%BC%B8%E5%B1%95%E6%A0%91md.assets/%E6%88%AA%E5%B1%8F2023-03-18%2016.35.12.png)

#### e）伸展树的代码

一种可能的代码实现如下所示：

```c++
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
```

