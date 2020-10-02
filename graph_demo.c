/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: graph_demo.c
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: graph.h 使用的示例文档
时间	   	: 2020-10-02 15:23
***************************************************************/
#include <stdio.h>
#include <string.h>
#include "tools.h"
#include "list.h"
#include "log.h"
#include "graph.h"

const int MAX = 100;


/* 测试:
6 6
0 1
1 2
3 1
3 4
4 5
5 2

 */

int main(int argc, char *argv[])
{
    // SET_DEFAULT_LEVEL(CONSOLE_LOGLEVEL_ERR);
    int i, j;
    int n, e, s, t, w;

    int vertex;
    _adj_node_ *node;
    
    scanf("%d %d", &n, &e);

    /* 1. 分配初始化图 */
    _Vertex_ *G = calloc_buf(n, _Vertex_);
    init(G, n);

    /* 2. 插入 */
    for (i = 0; i < e; ++i) {
        scanf("%d %d", &s, &t);
        (G + s)->id = s;
        insert(G, s, t, 0);
    }

    double start = START();
    
    /* 3. 显示邻接表 */
    show_adj_list(G, n);

    /* 4. 邻接表转邻接矩阵 */
    int matrix[n][n];
    for (i = 0; i < n; ++i)  
        for (j = 0; j < n; ++j) 
            matrix[i][j] = WEIGHT_INFTY;
    
    list2matrix(G, n, (int **)matrix);

    /* 5. 显示邻接矩阵 */
    show_adj_matrix((int **)matrix, n);

    /* 6. 邻接矩阵转邻接表 */
    _Vertex_ *G1 = calloc_buf(n, _Vertex_);
    init(G1, n);
    matrix2list((int **)matrix, n, G1);
    show_adj_list(G1, n);

    FINISH(start);

    clear_G(G, n);
    free_buf(G);
    clear_G(G1, n);
    free_buf(G1);
    return 0;
}