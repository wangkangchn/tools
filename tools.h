/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: my_user_tools.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 用户空间便捷工具
时间	   	: 2020-08-04 21:37
***************************************************************/
#ifndef __MY_USER_TOOLS_H__
#define __MY_USER_TOOLS_H__

// #ifdef __KERNEL__
/* 标记 */
#define LAB(name)               lab_ ## name    

/* 数组大小 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0])) 

/* 检查值是否小于 0 */
#define IS_LESS_THEN_ZERO(val, msg, ...) do { \
    if (val < 0) {  \
        pr_err(msg, ##__VA_ARGS__); \
        goto LAB(return);   \
    }   \
} while (0)

/* 分配设备缓冲区, 并对返回指针进行断言 */
#include <stdlib.h>
#include <assert.h>
#define calloc_buf(num, type) ({            \
    type *_buf = calloc( (num), sizeof( type )); \
    assert(_buf);                           \
    _buf;})

/* 释放缓冲区缓冲区 */
#define free_buf(buf) ({ free( (buf) ); (buf) = NULL; })


/* swap - 交换两变量元素(使用异或操作进行交换)
    x, y 需要为同一种指针类型 

`b Note:
`b    x, y 中不能使用 ++ --(自增自减运算符) 

`r 致命问题!!!
`r   使用异或进行元素交换有一个致命的问题, 如果交换的是同一变量结果永远为0.
`r 但是置换两个相同值的变量可以. 也就是说不能使用异或交换操作交换同一地址的值.
`r e.g.
`r a = 0010 1101;
`r a ^= a; ==> 0   (结果永远等于0)

#define swap1(x, y)  ({  \
    typeof( (x) ) __y = (y); \
    typeof( (y) ) __x = (x); \
    (*__x) ^= (*__y);        \
    (*__y) ^= (*__x);        \
    (*__x) ^= (*__y); })
*/
#define swap(x, y)  ({              \
    typeof( (x) ) __y = (y);        \
    typeof( *(y) ) __temp = *(x);   \
    *(x) = *__y; *__y = __temp; })

/* 计算两者的大值
`b Note:
`b    x, y 中不能使用 ++ --(自增自减运算符) 
 */
#define max(x, y) ({ (x) >= (y) ? (x) : (y); })

/**
 * trace - 按顺序输出数组元素
 * @A:      待排序数组	
 * @num:    数组元素个数
 */
#define trace(A, num) ({    \
    for (int i = 0; i < (num); ++i) { \
        if (i > 0) printf(" ");     \
        printf("%d", (A)[i]);         \
    }                               \
    printf("\n");})

/* 
            时间测试
e.g.
double start = START();
...
FINISH(start);
 */
#include <time.h>
#define START()         ({ clock(); })
#define FINISH(start)   ({ printf( "%.9f seconds\n", (double)( clock() - (start) ) / CLOCKS_PER_SEC); })

#endif // ! __MY_USER_TOOLS_H__

