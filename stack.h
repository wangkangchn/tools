/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: stack.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 数组栈
			使用方法:
				1. 使用 DEFINE_ELEMENT_TYPE(type, name) 定义使用的栈元素类型
				其中:
					type: 指定栈数组类型
					name: 结构体名称
				2. 使用 init(Q, max) 来初始化栈结构体元素
				其中:
					max: 栈的最大长度
				3. xxx_mutex 为带互斥锁操作
时间	   	: 2020-08-23 17:16
***************************************************************/
#ifndef __WKANGK_STACK_H__
#define __WKANGK_STACK_H__
#include <stdlib.h>
#include <pthread.h>

#define DEFINE_ELEMENT_TYPE(type, name)	\
	struct name							\
	{									\
		pthread_mutex_t mutex;			\
		unsigned int max_length; 		\
		int top;						\
		type *data;						\
	}

/* 成功返回1, 失败返回0 */
#define init(S, max) ({									\
        S->max_length = max;							\
		S->top = -1;									\
        S->data = calloc(max, sizeof(*S->data) * max);  \
		S->data && !pthread_mutex_init(&S->mutex, NULL);	})

/* size - 获取栈长度*/
#define size(S) 		S->top + 1

/* is_empty - 判断栈是否为空 */
#define is_empty(S) 	S->top < 0

/* is_full - 判断栈是否为满 */
#define is_full(S) 		S->top == S->max_length

/* pop - 弹栈 */
#define pop(S) 			S->data[S->top--]

/* push - 入栈 */
#define push(S, x) ({ 				\
	const typeof( x ) __x = (x);	\
	S->data[++S->top] = __x;	})

/* clear - 清空缓冲区 */
#define clear(S) 		({				\
	pthread_mutex_destroy(&S->mutex);	\
	free(S->data);	})

/* pop_mutex - 弹栈(带互斥锁) */
#define pop_mutex(S) 	({					\
		typeof( *S->data ) __x;				\
		pthread_mutex_lock(&S->mutex);		\
		__x = pop(S);						\
		pthread_mutex_unlock(&S->mutex);	\
		__x;	})

/* push_mutex - 入栈(带互斥锁) */
#define push_mutex(S, x) ({ 		\
	pthread_mutex_lock(&S->mutex);	\
	push(S, x);						\
	pthread_mutex_unlock(&S->mutex);	})

#endif // !__WKANGK_STACK_H__