/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: graph.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 创建有向图, 当需要创建无向图时, 需要插入全部的边, 不会自动插入任何边
时间	   	: 2020-09-27 09:10
***************************************************************/
#ifndef __GRAPH_H__ 
#define __GRAPH_H__ 
#include "tools.h"
#include "list.h"

#define __iteration__(count)    \
    for (size_t i = 0; i < count; ++i) 

const int WEIGHT_INFTY = (1 << 20);

typedef enum _vertex_state_ {
    NOT_VISIT,
    VISITED,
} _vertex_state_;

typedef struct _Vertex_ {
    int id;
    struct list_head list;
} _Vertex_, _graph_;

/* 邻接表顶点 */
typedef struct Edge {
    int id;
    int w;                  /* 权重 */ 
    struct list_head list;
} _adj_node_;


/**
 * insert - 向邻接表中插入节点(尾插法, 保证小的序号在前)
 * @G:      图(邻接表)
 * @id:     待插入边的左顶点
 * @_vertex: 与 id 邻接的顶点
 * @w:      权	
 * @return: 无
 */
#define insert(G, s, t, wight) ({     \
    _adj_node_ *__node = calloc_buf(1, _adj_node_);   \
    __node->id = (t);                        \
    __node->w = (wight);                              \
    list_add_tail(&(__node->list), &(( (G) + (s))->list)); })

/**
 * clear_list - 释放队列节点
 * @header:	待清理链表的头指针
 * @return: 无
 */
#define clear_list(header) ({       \
    struct list_head *__pos, *__q;  \
    _adj_node_ *__node;             \
    list_for_each_safe(__pos, __q, (header) ) {     \
        __node = list_entry( (__pos), _adj_node_, list ); \
        list_del(__pos);        \
        free_buf(__node);   \
    }   \
})

/**
 * clear_G - 释放图中节点空间
 * @G:	    图(邻接表)
 * @count:  图中节点个数
 * @return: 无
 */
#define clear_G(G, count)   ({  \
    for (size_t i = 0; i < count; ++i)    \
        clear_list(&(( (G) + i)->list));   \
})

/**
 * init - 初始化队列头邻接表
 * @G:	    图
 * @count:  图中节点个数
 * @return: 无
 */
#define init(G, count) ({       \
    __iteration__( (count) )          \
        INIT_LIST_HEAD(&(( (G) + i)->list));    \
})

/**
 * list2matrix - 将图的邻接表表示法表示为邻接矩阵
 * @G:      图的邻接表
 * @n:      图中顶点的个数
 * @matrix: 保存转换后的邻接矩阵(int **)
 * @return: 无
 */
#define list2matrix(G, n, matrix) ({    \
    int __i, __j, __temp;       \
    _adj_node_ *__node;             \
    for (__i = 0; __i < (n); ++__i)         \
        for (__j = 0; __j < (n); ++__j) {   \
            __temp = WEIGHT_INFTY;           \
            if (__i == __j)             \
                __temp = 0;           \
            *(((int *)(matrix) + __i * (n)) + __j) = __temp;  \
        }   \
    for (__i = 0; __i < n; ++__i)     \
        list_for_each_entry(node, &(( (G) + __i)->list), list)   \
            *(((int *)(matrix) + __i * (n)) + node->id) = node->w;    \
})


/**
 * matrix2list - 邻接矩阵转邻接表
 * @matrix:     邻接矩阵	    
 * @count:      图中顶点个数      
 * @G:          保存邻接表 	
 * @return: 无
 */
#define matrix2list(matrix, count, G) ({   \
    int __i, __j;   \
    int __w;    \
    for (__i = 0; __i < (count); ++__i)  {  \
        for (__j = 0; __j < (count); ++__j) {   \
            __w = *(((int *)(matrix) + __i * (count)) + __j);   \
            if (__w != WEIGHT_INFTY)    \
                insert( (G), __i, __j, __w);    \
        }   \
    }   \
})


/**
 * show_adj_list - 显示图的邻接表表示法
 * @G:          图	(邻接表)
 * @count:      图中顶点的个数	
 * @return:     无
 */
#define show_adj_list(G, count) ({      \
    _Vertex_ *__vertex;     \
    _adj_node_ *__node;     \
    for (int i = 0; i < (count); ++i) { \
        printf("%d -> ", i);    \
        list_for_each_entry(__node, &(((G) + i)->list), list) { \
            printf("%d(%d) ", __node->id, __node->w);   \
        }   \
        printf("\n");   \
    }   \
})

/**
 * show_adj_matrix - 显示图的邻接矩阵表示法
 * @G:          图	(邻接矩阵)
 * @count:      图中顶点的个数	
 * @return:     无
 */
#define show_adj_matrix(G, count) ({   \
    int __i, __j;   \
    for (__i = 0; __i < (count); ++__i)  {  \
        for (__j = 0; __j < (count); ++__j)     \
            printf("%d ", *(((int *)(G) + __i * (count)) + __j));   \
        printf("\n");   \
    }   \
})



// /**
//  * bfs - 广度优先搜索得到从顶点1到某个顶点的最短距离
//  * @G:      图(邻接表表示)	
//  * @id:     搜索的起点
//  * @id:     节点上执行的操作
//  * @return: 无
//  */
// #define bfs(G, s, op) ({    \
//     _Vertex_ *_vertex; \
//     _adj_node_ *_node; \
//     int vertex_id, dist = -1;   \
//     queue *_Q = calloc_buf(1, queue);    \
//     assert(qinit(_Q, MAX));  \
//     ((G) + (s))->dist = 0;  \
//     qpush(_Q, (s));\
//     while(!qis_empty(_Q)) {      \
//         vertex_id = qpop(_Q);        \
//         _vertex = (G) + vertex_id;   \
//         list_for_each_entry(_node, &(_vertex->list), list) {\
//             qpush(_Q, _node->id);\

//             op((G) + _node->id);    \


//             if ((( (G) + _node->id)->dist) == -1) {\
//                 ( (G) + _node->id)->dist = _vertex->dist + 1;\
//             }\
//         }\
//     }\
//     free_buf(_Q);    })

#endif	/*  __GRAPH_H__ */