/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: list_demo.c
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: list 使用示例
时间	   	: 2020-09-21 10:54
***************************************************************/
#include <stdio.h>
#include "tools.h"
#include "log.h"
#include "list.h"

typedef struct test_list {
    int num;
    struct list_head list;
} test_list;


int main(int argc, char *argv[])
{
    // SET_DEFAULT_LEVEL(CONSOLE_LOGLEVEL_INFO);
    int i;

    /* 1. 创建并初始化队列头 */
    struct list_head *header = calloc_buf(1, struct list_head);
    INIT_LIST_HEAD(header);


    /* 2. 插入 与 遍历 */
    test_list *node;

    /* 2.1. 头插与正向遍历 */
    /* 2.1.1 头插 */
    for (i = 0; i < 5; i++) {
        node = calloc_buf(1, test_list);
        node->num = i;
        list_add(&(node->list), header);
    }
    
    /* 2.1.2 正向遍历 */
    printf("正向遍历(期望输出): 4 3 2 1 0\n");
    list_for_each_entry(node, header, list)
        printf("%d ", node->num);
    printf("\n");

    /* 2.2. 尾插与反向遍历 */
    /* 2.2.1 尾插 */
    for (i = 0; i < 5; i++) {
        node = calloc_buf(1, test_list);
        node->num = i;
        list_add_tail(&(node->list), header);
    }
    
    /* 2.2.2 反向遍历 */
    printf("反向遍历(期望输出): 4 3 2 1 0 0 1 2 3 4\n");
    list_for_each_entry_reverse(node, header, list)
        printf("%d ", node->num);
    printf("\n");

    /* 3. 删除节点及情况链表 */
    /*
`r  遍历链表不删除节点的时候, 可以使用 list_for_each() 来获得节点指针, 
`r  但是遍历过程中有节点删除操作时, 必须使用 list_for_each_safe(), 否则会报错.
    */
    struct list_head *pos, *q;
    list_for_each_safe(pos, q, header) {
        node = list_entry(pos, test_list, list);
        list_del(pos);      /* 将节点从链表上删除 */
        free_buf(node);         /* 释放节点空间 */
    }


    /* 4. 测试空 */
    printf("%s\n", list_empty(header) ? "空" : "非空");

    free_buf(header);
    return 0;
}