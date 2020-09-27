/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: pqueue.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 优先级队列(最大堆实现)
        使用方法:
				1. 使用 DEFINE_PQUEUE_ELEMENT_TYPE(type, name) 定义使用的队列元素类型和优先级队列名字
				其中:
					type: 指定队列数组类型
					name: 结构体名称
                e.g.
                    DEFINE_PQUEUE_ELEMENT_TYPE(int, pqueue);

                2. 定义一个优先级队列
                    pqueue *Q = calloc(1, sizeof(pqueue));

				3. 使用 pqinit(Q, max, compare) 来初始化队列结构体元素
				其中:
					max:        队列的最大长度
                    compare:    为函数指针, 用于比较优先级队列元素, 详细介绍间下方
                Note:
                    pqinit 初始化失败时会触发断言失败, 成功无返回值

        e.g.
            DEFINE_PQUEUE_ELEMENT_TYPE(int, pqueue);  

            const int MAX = 100;
            const int root = 1;
            int main(int argc, char *argv[])
            {
                pqueue *Q = calloc(1, sizeof(pqueue));
                pqinit(Q, MAX, compare);

                pqpush(Q, key);
                ...
                pqpop(Q);

                pqclear(Q);
                free(Q);
                return 0;
            }
时间	   	: 2020-09-11 09:54
***************************************************************/
#ifndef __PQUEUE_H__   
#define __PQUEUE_H__   
#include <assert.h>
#include <stdlib.h>

/* 函数指针, 用于比较优先级队列元素, 其返回值必须满足
    arg1 < arg2时， 返回值 < 0;
    arg1 == arg2时，返回值 = 0;
    arg1 > arg2时， 返回值 > 0.

e.g.
typedef int heap_element;

int compare(const void *arg1, const void *arg2)
{
    int ret = 0;
    heap_element __arg1 = *(heap_element *)arg1;
    heap_element __arg2 = *(heap_element *)arg2;

    if (__arg1 < __arg2) ret = -1;
    else if (__arg1 > __arg2) ret = 1;

    return ret;
}
*/
typedef int (*__compare_fn)(const void *, const void *);

/* 
 * @__compare:     函数指针比较队列元素的大小
 *  */
#define DEFINE_PQUEUE_ELEMENT_TYPE(type, name)	\
    typedef struct name {   \
        size_t count;       \
        type *heap;         \
        __compare_fn __compare; \
    } name

#define parnet(id)      ({ (id) >> 1; })
#define left(id)        ({ (id) << 1; })
#define right(id)       ({ ( (id) << 1 ) + 1; })

/**
 * pqinit - 初始化有限队列
 * @return: 失败产生断言错误
 */
#define pqinit(Q, max, compare) ({					        \
        (Q)->count = 0;                                     \
        (Q)->heap = calloc((max), sizeof(*(Q)->heap));      \
        assert((Q)->heap);                                  \
        (Q)->__compare = (compare);                      \
})

/**
 * max_henpify - 从根结点 i 向叶结点方向寻找 __heap[i] 值的恰当位置
 * @heap:       堆指针
 * @root:       根节点
 */
#define max_henpify(Q, root)    ({  \
    size_t __n = (Q)->count;                   \
    typeof( (Q)->heap ) __heap = (Q)->heap;   \
    int __root = (root);                \
    int __left, __right, __largest;     \
\
    for(; ;) {                          \
        __left = left(__root);        \
        __right = right(__root);      \
        __largest = __root;             \
\
        if ( (__left  <= __n) && ( (Q)->__compare( (Q)->heap + __left,  (Q)->heap + __root    ) > 0) ) __largest = __left;    \
        if ( (__right <= __n) && ( (Q)->__compare( (Q)->heap + __right, (Q)->heap + __largest ) > 0) ) __largest = __right;   \
\
        if (__largest != __root) {      \
            swap(__heap + __root, __heap + __largest);  \
            __root = __largest;                         \
        } else      \
            break;   \
    }    \
})

/**
 * pqpush - 向优先队列插入元素 
 *     只有小于 0 才交换, 相同元素不进行交换
 * @heap:   优先队列指针
 * @key:    待插入值    
 */
#define pqpush(Q, val) ({          \
    typeof( *( (Q)->heap ) ) __val = (val); \
    (Q)->heap[++((Q)->count)] = __val;    \
    for (size_t i = (Q)->count; \
        (i > 1) && ((Q)->__compare((Q)->heap + parnet(i), (Q)->heap + i) < 0); \
        i = parnet(i))    \
        swap((Q)->heap + i, (Q)->heap + parnet(i));     \
})

/**
 * pqpop - 获取优先队列的最大值而后删除
 * @Q:	优先队列
 * @return: 优先队列中的最大值
 */
#define pqpop(Q) ({      \
    typeof( (Q)->heap ) __heap = (Q)->heap;    \
    assert((Q)->count);  \
    typeof( *__heap ) __max = __heap[1];        \
    __heap[1] = __heap[(Q)->count--];           \
    max_henpify(Q, 1);         \
    __max;                                      \
})   

/* pqtop - 查看队首元素 */
#define pqtop(Q) 			({ ((Q)->heap)[1]; })

/* pqclear - 清空优先队列 */
#define pqclear(Q)          ({ free((Q)->heap); })

/* pqis_empty - 判断优先队列是否为空 */
#define pqis_empty(Q)          ({ (Q)->count == 0; })

/* pqsize - 获取优先队列元素个数 */
#define pqsize(Q)          ({ (Q)->count; })

#endif	// __PQUEUE_H_
