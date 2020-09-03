/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: queue.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 环形队列(数组实现)
			使用方法:
				1. 使用 DEFINE_ELEMENT_TYPE(type, name) 定义使用的队列元素类型
				其中:
					type: 指定队列数组类型
					name: 结构体名称
				2. 使用 init(Q, max) 来初始化队列结构体元素
				其中:
					max: 队列的最大长度
				3. xxx_mutex 为带互斥锁操作
			e.g.
				DEFINE_ELEMENT_TYPE(int, queue);

				const int MAX = 100;

				int main(int argc, char *argv[])
				{
					queue *Q = calloc(1, sizeof(queue));
					printf("%d\n", init(Q, MAX));
					return 0;
				}
时间	   	: 2020-08-23 15:29
***************************************************************/
#ifndef __WKANGK_QUEUE_H__
#define __WKANGK_QUEUE_H__
#include <stdlib.h>
#include <pthread.h>

#define DEFINE_ELEMENT_TYPE(type, name)		\
	typedef struct name {					\
		pthread_mutex_t mutex;			\
		unsigned int max_length; 		\
		unsigned int head, tail;		\
		unsigned int count;				\
		type *data;						\
	} name

/* 成功返回1, 失败返回0 */
#define init(Q, max) ({									\
        Q->max_length = (int)max;						\
		Q->head = Q->tail = Q->count = 0;				\
        Q->data = calloc(max, sizeof(*Q->data));  \
		Q->data && !pthread_mutex_init(&Q->mutex, NULL);	})

/* size - 获取队列长度*/
#define size(Q) 			({ Q->count; })

/* is_empty - 判断队列是否为空 */
#define is_empty(Q) 		({ Q->count == 0; })

/* is_full - 判断队列是否为满 */
#define is_full(Q) 			({ Q->count == Q->max_length; })

/* front - 查看队首元素 */
#define front(Q) 			({ ((type *)Q->data)[head]; })

/* pop - 弹出队首元素 */
#define pop(Q) ({ 								\
	typeof(*Q->data) __x = Q->data[Q->head];	\
    Q->head = ++Q->head % Q->max_length;		\
	--Q->count;									\
	__x; })

/* push - 向队列中添加元素x */
#define push(Q, x) ({ 						\
	typeof( *Q->data ) __x = (x);			\
	Q->data[Q->tail] = __x; 				\
	Q->tail = ++Q->tail % Q->max_length;	\
	++Q->count;})

/* clear - 清空缓冲区, 释放指针 */
#define clear(Q) ({						\
	pthread_mutex_destroy(&Q->mutex);	\
	free(Q->data);})

/* pop_mutex - 弹出队首元素(带互斥锁) */
#define pop_mutex(Q) 	({				\
	pthread_mutex_lock(&Q->mutex);		\
	typeof( *Q->data ) __x = pop(Q);	\
	pthread_mutex_unlock(&Q->mutex);	\
	__x;	})

/* push_mutex - 入栈(带互斥锁) */
#define push_mutex(Q, x) ({ 		\
	pthread_mutex_lock(&Q->mutex);	\
	push(Q, x);						\
	pthread_mutex_unlock(&Q->mutex);	})

#endif // !__WKANGK_QUEUE_H__