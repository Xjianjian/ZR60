/******************************************************
文件名：	BnryTree.c

描述:

Data			  Vasion			author
2019/1/11		  V1.0			    liujian
*******************************************************/

/*******************************************************
description include the header file
*******************************************************/
#include "BnryTree.h"

/*******************************************************
description function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/


/*******************************************************
description global variable definitions
*******************************************************/


/*******************************************************
description static variable definitions
*******************************************************/
static uint8 InsertBnryTree(BnryTreeNode **tree, DataType dt);
static void PreOrder(BnryTreeNode *tree);

/******************************************************
description function code
******************************************************/
/******************************************************
*函数名：CreateBnryTree

*形参：

*返回值：

*描述：创建二叉树

*备注：返回创建的二叉树的头结点地址
******************************************************/
BnryTreeNode* CreateBnryTree(DataType* Le_p_dt, uint16 Le_w_n)
{
    BnryTreeNode *tree = STD_NULL_PTR;
    uint16 Le_w_i = 0;
    while(Le_w_i < Le_w_n)
	{
		if (InsertBnryTree(&tree, Le_p_dt[Le_w_i]) == 1)
		{
			Le_w_i++;
		}
	}
    return tree;
}

/******************************************************
*函数名：BnryTree_MakeEmpty

*形参：

*返回值：

*描述：清空二叉树

*备注：
******************************************************/
BnryTreeNode* BnryTree_MakeEmpty(BnryTreeNode* tree)
{
	if(tree)
	{
		BnryTree_MakeEmpty(tree->lchild);
		BnryTree_MakeEmpty(tree->rchild);
		free(tree);
		tree = STD_NULL_PTR;
	}
	return tree;
}

/******************************************************
*函数名：BnryTree_FindNode

*形参：

*返回值：

*描述：查找节点

*备注：查找不到，返回NULL
******************************************************/
BnryTreeNode* BnryTree_FindNode(DataType element, BnryTreeNode* tree)
{
next:
	if(tree)
	{
		if(tree->element == element)
		{
			return tree;
		}
		else if(tree->element > element)
		{
			//return BnryTree_FindNode(element, tree->lchild);
			tree = tree->lchild;
			goto next;
		}
		else
		{
			//return BnryTree_FindNode(element, tree->rchild);
			tree = tree->rchild;
			goto next;
		}
	}
	return STD_NULL_PTR;
}


/******************************************************
*函数名：BnryTree_FindMax

*形参：

*返回值：

*描述：查找最大值

*备注：
******************************************************/
BnryTreeNode* BnryTree_FindMax(BnryTreeNode* tree)
{
next:
	if(tree)
	{
		if(tree->rchild == STD_NULL_PTR)
		{
			return tree;
		}
		tree = tree->rchild;
		goto next;
	}
	return tree;
}

/******************************************************
*函数名：BnryTree_FindMin

*形参：

*返回值：

*描述：查找最小值

*备注：
******************************************************/
BnryTreeNode* BnryTree_FindMin(BnryTreeNode* tree)
{
next:
	if(tree)
	{
		if(tree->lchild == STD_NULL_PTR)
		{
			return tree;
		}
		tree = tree->lchild;
		goto next;
	}
	return STD_NULL_PTR;
}

/******************************************************
*函数名：InsertBnryTree

*形参：

*返回值：

*描述：插入二叉树

*备注：
******************************************************/
static uint8 InsertBnryTree(BnryTreeNode **tree, DataType dt)
{
    if(*tree == STD_NULL_PTR)
    {
        *tree = (BnryTreeNode*)malloc(sizeof(BnryTreeNode));
		if(STD_NULL_PTR != tree)
		{
			(*tree)->element = dt;
			(*tree)->lchild = (*tree)->rchild = STD_NULL_PTR;
			return 1;
		}
		else//鍐呭瓨鍒嗛厤澶辫触锛岀洿鎺ラ€€鍑
		{
#if PRINT_LOG
			printf("内存申请失败\n");
#endif
			return 0;
		}
    }
    else if(dt == (*tree)->element)
	{
#if PRINT_LOG
			printf("数据元素%d重复，跳过不处理\n",dt);
#endif
        return 0;//杩欓噷璺宠繃涓嶅仛澶勭悊
	}
    else if(dt < (*tree)->element)
	{
        return InsertBnryTree(&((*tree)->lchild), dt);
	}
    else
	{
        return InsertBnryTree(&((*tree)->rchild), dt);
	}
}

/******************************************************
*函数名：BnryTree_DeleteNode

*形参：

*返回值：

*描述：删除节点

*备注：
******************************************************/
void BnryTree_DeleteNode(BnryTreeNode *tree,DataType dt)
{
	BnryTreeNode *L,*LL;//在删除左右子树都有的结点时使用；
	BnryTreeNode *p=tree;
	BnryTreeNode *parent=tree;
	char child=0;//0表示左子树，1表示右子树；
	if(!tree)//如果排序树为空，则退出;
		return ;
	while(p)//二叉排序树有效；
	{
		if(p->element==dt)
		{
			if(!p->lchild&&!p->rchild)//叶结点(左右子树都为空)；
			{
				if(p==tree)//被删除的结点只有根结点；
					free(p);
				else if(child==0)
				{
					parent->lchild=STD_NULL_PTR;  //设置父结点左子树为空；
					free(p);   //释放结点空间；
				}
				else//父结点为右子树；
				{
					parent->rchild=STD_NULL_PTR;  //设置父结点右子树为空；
					free(p);  //释放结点空间；
				}
			}
			else if(!p->lchild)  //左子树为空，右子树不为空；
			{
				if(child==0)    //是父结点的左子树；
					parent->lchild=p->rchild;
				else      //是父结点的右子树；
					parent->rchild=p->rchild;
				free(p);  //释放被删除的结点；
			}
			else if(!p->rchild)  //右子树为空，左子树不为空；
			{
				if(child==0)  //是父结点的左子树；
					parent->lchild=p->lchild;
				else      //是父结点的右子树；
					parent->rchild=p->lchild;
				free(p);  //释放被删除的结点；
			}
			else
			{
				LL=p;  //保存左子树的结点；
				L=p->rchild;//从当前结点的右子树进行查找；
				if(L->lchild)//左子树不为空；
				{
					LL=L;
					L=L->lchild;   //查找左子树；
					p->element=L->element;  //将左子树的数据保存到被删除结点；
					LL->lchild=L->lchild;  //设置父结点的左子树指针为空；
					for(;L->lchild;L=L->lchild);
					L->lchild=p->lchild;
					p->lchild=STD_NULL_PTR;
				}
				else
				{
					p->element=L->element;
					LL->rchild=L->rchild;
				}
			}
			p=STD_NULL_PTR;
		}
		else if(dt<p->element)  //需删除记录的关键字小于结点的数据；
		{
			//要删除的结点p是parent的左子树；
			child=0;  //标记在当前结点左子树；
			parent=p;//保存当前结点作为父结点；
			p=p->lchild;  //查找左子树；
		}
		else  //需删除记录的关键字大于结点的数据；
		{
			//要删除的结点p是parent的右子树；
			child=1;  //标记在当前结点右子树查找；
			parent=p;  //保存当前结点作为父结点；
			p=p->rchild;  //查找右子树；
		}
	}
}


/******************************************************
*函数名：PreOrder

*形参：

*返回值：

*描述：中序遍历

*备注：
******************************************************/
static void PreOrder(BnryTreeNode *tree)
{
	if(tree)
	{
		PreOrder(tree->lchild);
#if PRINT_LOG
		printf("%d ",tree->element);
#endif
		PreOrder(tree->rchild);
	}
}

//测试
void BnryTree_Test(void)
{
	char Le_u_i;
	BnryTreeNode* tree= STD_NULL_PTR;
	BnryTreeNode* tree_Max= STD_NULL_PTR;
	BnryTreeNode* tree_Min= STD_NULL_PTR;
	int a[9] = {3,2,5,8,4,7,6,9,10};
	
#if PRINT_LOG
	printf("\r\noriginal data: \r\n");
	for(Le_u_i = 0;Le_u_i < 9;Le_u_i++)
	{
		printf("%d",a[Le_u_i]);
	}
	printf("\r\n");
#endif
	printf("\r\ncreate a binary tree: \r\n");
	tree = CreateBnryTree(a,9);
	
	printf("bt->element=%d",tree->element);
	printf("\r\nin order traversal: \r\n");
	PreOrder(tree);
	
	tree_Max = BnryTree_FindMax(tree);
	printf("\r\nthe max node: %d",tree_Max->element);
	printf("\r\n");
	tree_Min = BnryTree_FindMin(tree);
	printf("\r\nthe min node: %d",tree_Min->element);
	printf("\r\n");
	
	InsertBnryTree(&tree,15);
	InsertBnryTree(&tree,1);
	printf("\r\nin order traversal: \r\n");
	PreOrder(tree);
	tree_Max = BnryTree_FindMax(tree);
	printf("\r\nthe max node: %d",tree_Max->element);
	printf("\r\n");
	tree_Min = BnryTree_FindMin(tree);
	printf("\r\nthe min node: %d",tree_Min->element);
	printf("\r\n");
	
	printf("\r\ndelete 7\r\n");
	BnryTree_DeleteNode(tree,7);
	printf("\r\nin order traversal: \r\n");
	PreOrder(tree);
	tree_Max = BnryTree_FindMax(tree);
	printf("\r\nthe max node: %d",tree_Max->element);
	printf("\r\n");
	tree_Min = BnryTree_FindMin(tree);
	printf("\r\nthe min node: %d",tree_Min->element);
	printf("\r\n");
	
	BnryTree_MakeEmpty(tree);
}


