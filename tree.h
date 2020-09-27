/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: tree.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 二叉搜索树(链表实现)
时间	   	: 2020-09-22 10:43
***************************************************************/
#ifndef __TREE_H__ 
#define __TREE_H__ 
#include <stdlib.h>
#include "list.h"
#include "tools.h"

#define DEFINE_STACK_ELEMENT_TYPE(type, name)	\
	typedef struct name	{               \
		struct list_head left;			\
		struct list_head right;			\
		type *data;						\
	} name


/**
 * insert - 向二叉搜索树插入键值
 * @tree:   树
 * @key:    待插入键值	
 * @return: 无
 */
void insert(Node **tree, int key)
{
    /* 来到所需的叶节点, 插入 */
    if (*tree == NULL) {
        *tree = calloc_node(key);
        // (*tree)->parent = parent;
        return;
    }

    /* 向左子树插 */
    if (key <= (*tree)->key)
        insert(&(*tree)->left, key);
    /* 向右子树插 */
    else
        insert(&(*tree)->right, key);
        
}

#endif	/* !__TREE_H__ */