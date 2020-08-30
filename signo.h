/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: signo.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 
时间	   	: 2020-08-13 09:27
***************************************************************/
#ifndef __SIGNO_H__
#include <signal.h>
#include <asm/types.h>

/* 自定义信号 */
#define OFFSET          3       /* 前3个信号保留 */
#define MY_SIGRTMIN     __SIGRTMIN + OFFSET

/* 信号池大小为32 */
typedef __u32 pool;
#define POLL_SIZE   sizeof(pool) * 8 - OFFSET

extern pool signo_pool;

int get_signo(void);
inline void delete_signo(int signo);

#define GET_SIGNO   do { get_signo(); } while(0);

#endif // !__SIGNO_H__
