/***************************************************************
Copyright © wangk <wangkangchn@163.com>
文件名		: my_user_tools.h
作者	  	: wangk <wangkangchn@163.com>
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
#include <assert.h>
#define calloc_buf(num, type) ({            \
    type *_buf = calloc(num, sizeof(type)); \
    assert(_buf);                           \
    _buf;})

/* 释放缓冲区缓冲区 */
#define free_buf(buf) ({ free(buf); buf = NULL; })

#endif // !__MY_USER_TOOLS_H__

