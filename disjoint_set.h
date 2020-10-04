/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: disjoint_set.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 互质集合
时间	   	: 2020-10-04 16:46
***************************************************************/
#ifndef __DISJOINT_SET__ 
#define __DISJOINT_SET__ 
#include <stdbool.h>
#include "tools.h"

typedef struct disjoint_set {
    int *rank;    
    int *set;     
} disjoint_set;


/**
 * make_set - 创建并初始化互质集合
 * @dset:	disjoint_set 类型指针
 * @n:      互质集合个数
 * @return: 无
 */
static inline void make_set(disjoint_set * set, size_t n) {
    set->set = calloc_buf((n), int);   
    set->rank = calloc_buf((n), int);  
    for (int i = 0; i < n; ++i)         
        set->set[i] = i;             
}

/**
 * find_set - 求包含元素 x 的集合的代表元素
 * @@set:   所有的集合
 * @x:	    带求元素
 * @return: x 的集合的代表元素(根)
 */
static inline int find_set(disjoint_set *set, int x)
{
    int parent;
    int temp = x;
    /* 1. 寻找代表元素 */
    while (true) {
        parent = set->set[temp];
        if (parent == temp)
            break;
        else
            temp = parent;
    }

    /* 2. 压缩路径 */
    while (true) {
        if (set->set[x] == x)    /* 不是根 */
            break;
        else {
            temp = set->set[x];  
            set->set[x] = parent;/* 调整路径上节点的父直接指向代表元素 */
            x = temp;
        }
    }
    return parent;
}

/**
 * link - 合并两个集合, 将较低的树合并到较高的树中
 * @set:    所有的集合
 * @rank:   记录各节点为根时的树高
 * @x:      树代表元素
 * @y:	    树代表元素
 * @return: 无
 */
static inline void link(disjoint_set *set, int x, int y)
{   
    int father = x, child = y;

    /* x 合并进 y */
    if (set->rank[x] < set->rank[y]) {
        father = y;
        child = x;
    }
    ++set->rank[father];
    set->set[child] = father;
}

/**
 * unite - 合并指定元素 x, y
 * @set:    所有的集合
 * @rank:   记录各节点为根时的树高
 * @x:      待合并元素
 * @y:	    待合并元素
 * @return: 无
 */
static inline  void unite(disjoint_set *set, int x, int y)
{
    link(set, find_set(set, x), find_set(set, y));
}

/**
 * same - 判断两元素是否属于同一集合
 * @set:    所有的集合
 * @x:      带判断元素
 * @y:	    带判断元素
 * @return: 同一集合返回 true, 否则返回 false
 */
static inline bool same(disjoint_set *set, int x, int y)
{
    return find_set(set, x) == find_set(set, y);
}

#endif	/* !__DISJOINT_SET__ */