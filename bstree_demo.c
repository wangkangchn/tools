/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: binary_search_tree_while.c.c
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 二叉搜索树示例
时间	   	: 2020-09-22 11:03
***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "tools.h"
#include "log.h"
#include "list.h"
#include "bstree.h"
#include "stack.h"

/* 1. 定义树节点数据域类型, 树节点名字, 数名字 */
DEFINE_BSTREE_ELEMENT_TYPE(double, bstree_node, bstree);

/* 2. 定义树节点间的大小关系
    arg1 == arg2:   0
    arg1 < arg2:    -1
    arg1 > arg2:    1
    */
int compare(const void *arg1, const void *arg2)
{
    double __arg1 = *(double *)arg1;
    double __arg2 = *(double *)arg2;

    if (fabs(__arg1 - __arg2) < 1e-8)   return 0;
    else if (__arg1 < __arg2)           return -1;
    else                                return 1;
}

/**
 * traversal - 遍历树(先根法)
 * @tree:	待遍历的树
 * @id:     节点编号
 * @father: 父节点编号
 * @type:   遍历类型(先根和后根)
 * @op:     遍历过程中执行的操作
 * @return: 无
 */
void traversal(bstree_node *tree, enum TraversalType type, void (*op)(bstree_node **))
{   
    /* 1. 递归退出条件 */
    if (tree == NULL) {
        return;
    }

    /* 2. 输出前序遍历结果 */
    if (type == PRE_ORDER) 
        op(&tree);
    
    /* 3. 遍历左右子树 */
    traversal(tree->left, type, op);
    if (type == IN_ORDER) 
        op(&tree);
    traversal(tree->right, type, op);

    if (type == POST_ORDER) 
        op(&tree);
}

#define free_node(node) ({  \
    if ((node)) {   \
        free_buf((node)->data); \
        free_buf((node));       \
    }   \
})



/**
 * clear_up_tree_node - 清空树节点
 * @tree:	待删除的树
 * @return: 无
 */
void clear_up_tree_node(bstree_node *tree)
{   
    if (tree) {
        /* 从左子树退出时清空左子树 */
        clear_up_tree_node(tree->left);
        free_node(tree->left);

        /* 从右子树退出时清空右子树和根 */
        clear_up_tree_node(tree->right);
        free_node(tree->right);
    }
}

/**
 * clear_up_tree - 清空树
 * @tree:	待删除的树
 * @return: 无
 */
void clear_up_tree(bstree *tree)
{   
    clear_up_tree_node(tree->root);
    free_buf(tree->root->data);
    free_buf(tree->root);
}

// #define MAX  1024
// DEFINE_STACK_ELEMENT_TYPE(bstree_node, stack);
// void clear_up_tree_node1(bstree *tree)
// {   
//     struct stack *S = calloc(1, sizeof(struct stack));
// 	printf("%d\n", init(S, MAX));

//     bstree_node *node = tree->root;
//     spush(S, node);
//     while(!sis_empty(S)) {
//         node = spop(S);

//         if (node->left) {
//             spush(S, node);
//             spush(S, node->left);   /* 左 根 */
//             node = node->left;
//             continue;
//         } else {
//             /* 左子树不存在, 处理 */


//             if (node->right) {
//                 spush(S, node->right);   /* 左 根 */
//                 node = node->left;
//                 continue;
//             }
//         }
//     }



//     if (tree) {
//         /* 从左子树退出时清空左子树 */
//         clear_up_tree_node(tree->left);
//         free_node(tree->left);

//         /* 从右子树退出时清空右子树和根 */
//         clear_up_tree_node(tree->right);
//         free_node(tree->right);
//     }
// }

/**
 * show - 显示节点键值
 * @node:   节点
 * @return: 无
 */
static inline void show(bstree_node **node) { printf("%lf ", *(*node)->data); }


const int UPPER = 4000;

int main(int argc, char *argv[])
{
    int i;
    int key;
    double _key;
    
    bstree tree = {
        .root = NULL,
        .__compare = compare,
    };

    srand(time(NULL)); /* 根据当前时间设置随机种子 */

    /* 3. 插入 */
    for (i = 0; i < 200; ++i) {
        _key = (double)(rand() % UPPER);
        insert(&tree, &_key);
    }

    /* 4. 遍历 */
    printf("中序遍历: \n"); 
    traversal(tree.root, IN_ORDER, show);
    printf("\n");

    /* 5. 查找和删除 */
    for (i = 0; i < 100; ++i) {
        _key = (double)(rand() % UPPER);
        if (find(&tree, &_key)) {
            printf("yes\t");
            delete(&tree, &_key);
        } else {
            printf("no\t");
        }
        if (!(i % 10))
            printf("\n");
    }

    printf("\n删除后中序遍历: \n"); 
    traversal(tree.root, IN_ORDER, show);
    printf("\n");

    /* 6. 清空树 */
    clear_up_tree(&tree);
    printf("%p\n", tree.root);

    printf("\n清空后中序遍历: %p\n", tree.root); 
    traversal(tree.root, IN_ORDER, show);
    printf("\n");

    return 0;
}