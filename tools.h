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


/* 标记 */
#define LAB(name)               lab_ ## name    

/* 数组大小 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0])) 


/* 验证指针是非为NULL */
#define CHECK_PTR_IS_EXIST(ptr) do { \
    if (!ptr) {   \
        ret = -ENOMEM;  \
        pr_err("为 "#ptr" 分配空间失败.\n");  \
        goto LAB(return);\
    } \
} while(0)

/* 检查值是否小于 0 */
#define IS_LESS_THEN_ZERO(val, msg, ...) do { \
    if (val < 0) {  \
        pr_err(msg, ##__VA_ARGS__); \
        goto LAB(return);   \
    }   \
} while (0)

/* 分配设备缓冲区 */
#define calloc_buf(buf, size) do {    \
    buf = calloc(1, size);   \
    CHECK_PTR_IS_EXIST(buf);  \
} while(0)

/* 释放缓冲区缓冲区 */
#define free_buf(buf) do {    \
    free(buf); buf = NULL;   \
} while(0)

#endif // !__MY_USER_TOOLS_H__

