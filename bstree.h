/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: bstree.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 二叉搜索树 
        使用方法:
            见 bstree_demo.c
时间	   	: 2020-09-22 11:03
***************************************************************/
#include <stdbool.h>
#include "tools.h"
#include "list.h"

typedef int (*__compare_fn)(const void *, const void *);

/**
 * 定义节点数据域以及结构体名字
 * @type:	    数据域类型
 * @node_name:	节点结构体名字
 * @tree_name:	树构体名字
 */
#define DEFINE_BSTREE_ELEMENT_TYPE(type, node_name, tree_name)   \
    typedef struct node_name {          \
        struct node_name *left;         \
        struct node_name *right;        \
        type *data;                     \
    } node_name;                        \
    typedef struct tree_name {          \
        node_name *root;                \
        __compare_fn __compare;         \
    } tree_name


enum TraversalType     
{   PRE_ORDER = 0,    
    IN_ORDER = 1,  
    POST_ORDER = 2,  
};


/**
 * calloc_node - 分配节点空间, 插入键值
 * @val:	待插入键值
 * @return: 新分配的节点指针
 */
#define calloc_node(val, type)    ({              \
    type *__node =  calloc_buf(1, type);    \
    __node->data = calloc_buf(1, typeof( *__node->data ) ); \
    *__node->data = *(val);                      \
    __node;                                 \
})

/**
 * insert - 向二叉搜索树插入键值
 * @tree:   树(二级指针, 数指针的地址)
 * @key:    待插入键值	
 */
#define insert(tree, val)   ({  \
    typeof( (tree)->root ) *node = &( (tree)->root ); \
    typeof( (tree)->root ) __temp = calloc_node( (val), typeof( *((tree)->root) ) );   \
    while (*node != NULL)           \
        if ( (tree)->__compare( __temp->data, (*node)->data ) <= 0 ) node = &(*node)->left; \
        else node = &(*node)->right;                    \
    (*node) = __temp;                         \
})

/**
 * find - 在二叉搜索树中搜索指定的键值
 * @tree:   树
 * @key:	待搜索的键值
 * @return: 找到成功返回true, 失败返回false
 */
#define find(tree, val)     ({          \
    bool __result = false;              \
    typeof( (tree)->root ) __node = (tree)->root; \
    while (true)    {                   \
        if (__node == NULL)     break;  \
        if      ( (tree)->__compare( (val), __node->data ) < 0 )     __node = __node->left;  \
        else if ( (tree)->__compare( (val), __node->data ) > 0 )     __node = __node->right; \
        else {  __result = true;    break;  }               \
    }           \
    __result;   \
})

/**
 * find_rightmost_key - 查找最右子树键值, 返回其上的键值, 而后删除该节点
 * @tree:   待搜索的子树	
 * @return: 最右子树的键值
 */
#define find_rightmost_key(tree)    ({  \
    typeof( (tree) ) __node_ = tree; \
    typeof( *(tree) ) __temp;       \
    typeof( (*(tree))->data ) __key;  \
\
    while (true)    {   \
        if ((*__node_)->right) __node_ = &(*__node_)->right;   \
        else {  \
            __temp = *__node_;   \
            __key = __temp->data;\
            if (__temp->left)   *__node_ = __temp->left;  \
            else                *__node_ = NULL;         \
            free_buf(__temp);   \
            break;              \
        }   \
    }   \
    *__key;  \
})

/**
 * delete - 在二叉搜索树中删除指定节点
 *      Note:
 *          __node 为 node_type 的二级指针
 * @tree:   树结构体的地址
 * @key:	待删除的键值的地址
 * @return: 无
 */
#define delete(tree, val)   ({  \
    typeof( (tree)->root ) *__node = &( (tree)->root ); \
    typeof( *__node )   __temp;         \
    while ((*__node) != NULL)  {           \
        if      ( (tree)->__compare( (val), (*__node)->data ) < 0 )     (__node) = &(*__node)->left;  \
        else if ( (tree)->__compare( (val), (*__node)->data ) > 0 )     (__node) = &(*__node)->right; \
        else {  \
            if ((*__node)->left && (*__node)->right)            *(*__node)->data = find_rightmost_key(&(*__node)->left);  \
            else if (!((*__node)->left || (*__node)->right))    free_buf(*__node);                                      \
            else {  \
                __temp = *__node;   \
                if ((*__node)->left)    (*__node) = (*__node)->left;        \
                else                    (*__node) = (*__node)->right;       \
                free_buf(__temp);   \
            }   \
        }   \
    }   \
})
