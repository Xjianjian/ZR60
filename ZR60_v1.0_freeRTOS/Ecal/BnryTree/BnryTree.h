/******************************************************
文件名：	BnryTree.h

描述:

Data			  Vasion			author
2019/1/11		  V1.0			    liujian
*******************************************************/
#ifndef		BNRYTREE_H
#define		BNRYTREE_H


/*******************************************************
description include the header file
*******************************************************/
#include "MyType.h"


/*******************************************************
description macro definitions
*******************************************************/
/**********宏开关定�*******/
#define   PRINT_LOG      1 //定义输出log


/**********宏常量定�*******/
/***********空指�**************/
#ifndef   STD_NULL_PTR
#define   STD_NULL_PTR   ((void*)0)
#endif


/***********宏函�*********/

/*******************************************************
description struct definitions
*******************************************************/


/*******************************************************
description typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef int DataType;
typedef struct Node 
{
    DataType element;
    struct Node *lchild, *rchild;
}BnryTreeNode;



/******enum definitions******/

/******union definitions*****/

/*******************************************************
description variable External declaration
*******************************************************/


/*******************************************************
description function External declaration
*******************************************************/
extern BnryTreeNode* CreateBnryTree(DataType* Le_p_dt, uint16 Le_w_n);
extern BnryTreeNode* BnryTree_MakeEmpty(BnryTreeNode* tree);
extern BnryTreeNode* BnryTree_FindNode(DataType element, BnryTreeNode* tree);
extern BnryTreeNode* BnryTree_FindMax(BnryTreeNode* tree);
extern BnryTreeNode* BnryTree_FindMin(BnryTreeNode* tree);
extern void BnryTree_Test(void);
#endif
