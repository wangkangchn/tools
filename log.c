/***************************************************************
Copyright © wangk <wangkangchn@163.com>
文件名		: log.c
作者	  	: wangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 日志文件
时间	   	: 2020-08-12 19:44
***************************************************************/
#include <fcntl.h>      /* 读写标记 */
#include <errno.h>
#include "my_user_tools.h"

#ifndef CONFIG_LOG
#define CONFIG_LOG
#endif

#include "log.h"

struct log *plog = NULL;

/**
 * get_activite_buf - 根据活动缓冲区状态, 返回活动的缓冲区的指针
 * @return: 活动缓冲区指针
 */
static inline void *get_activite_buf(void) { return plog->activite == 1 ? plog->buf1 : plog->buf2; }

/**
 * get_disable_buf - 根据活动缓冲区状态, 返回不可用的缓冲区的指针
 * @return: 不可用缓冲区指针
 */
static inline void *get_disable_buf(void) { return plog->activite == 1 ? plog->buf2 : plog->buf1; }

/**
 * change_buf_state - 更改活动缓冲区状态
 * @return: 可以活动的缓冲区id
 */
static inline int change_buf_state(void) { return plog->activite == 1 ? 2 : 1; }

/**
 * write_file - 向文件写入缓冲区的内容
 * @plog:	日志参数
 * @return: 无
 */
static void write_file(struct log *plog)
{   
	int fd = open(plog->log_fname, O_WRONLY | O_APPEND);	
    int ret = 0;
	if (fd > 0 ) {
        char *buf = calloc(1, plog->buf_size);  /* 先将log中满的缓冲区拷贝下来, 慢慢再向文件写 */
        char *log_buf = get_disable_buf();      /* 不可用, 即满的那个buf */

        /* 先保存后清空缓冲区中的内容 */
        strncpy(buf, log_buf, strlen(log_buf));
        memset(log_buf, 0, plog->buf_size);

        ret = write(fd, buf, strlen(buf));  /* 不写入\0 */
        if (ret < 0)
            printf("Write %s failed.\n", plog->log_fname); 
		
        pr_debug("写入 %d B\n", ret);
        
        /* 关闭文件, 释放缓冲区 */
        close(fd);	
        free(buf);	 

	} else {
        printf("Open %s failed.\n", plog->log_fname);    
    }
}

/**
 * create_file - 创建日志文件 
 * @param:	文件名
 * @return: 成功返回正数, 失败返回-1
 */
static int create_file(const char * const filename)
{
    int fd = open(filename, O_CREAT | O_TRUNC, 0644);	/* 存在就删除内容 */ 
    if (fd)    
        close(fd);
    return fd;
}

/**
 * thread_log - 写线程函数
 * @plog:	日志结构体
 * @return: 无
 */
static void *thread_log(struct log *plog) {
    int signum;
    sigset_t sig;               /* 该线程所能处理的信号集 */
    sigemptyset(&sig);          /* 清空 */   
    sigaddset(&sig, SIG_WRITE);  /* 设置能处理的信号, 这个时自定义的信号 */
    pthread_sigmask(SIG_BLOCK, &sig, NULL);  

    if (create_file(plog->log_fname)) {
        printf("%s create successfully.\n", plog->log_fname);
    } else {
        printf("%s can not create.\n", plog->log_fname);	
        return (void *)0;
    }

    plog->state = 1;
    for (; ;) {
        printf("waitting ...\n");
        sigwait(&sig, &signum);
        printf("thread_log 收到写信号, 向文件 %s 写\n", plog->log_fname); 
        write_file(plog);
        printf("向文件 %s 写入完成\n", plog->log_fname);
        pthread_testcancel();
    }
}


/* 验证指针是非为NULL */
#define CHECK_PTR_IS_EXIST(ptr) do { \
    if (!ptr) {   \
        ret = -ENOMEM;  \
        printf("buf 分配出错\n");   \
        goto LAB(return);\
    } \
} while(0)

/* 分配设备缓冲区 */
#define calloc_buf(plog) do {    \
    plog->buf1 = calloc(1, plog->buf_size);   \
    CHECK_PTR_IS_EXIST(plog->buf1);  \
    plog->buf2 = calloc(1, plog->buf_size);   \
    CHECK_PTR_IS_EXIST(plog->buf2);  \
    plog->msg = calloc(1, SINGLE_MSG_SIZE);   \
    CHECK_PTR_IS_EXIST(plog->msg);  \
} while(0)

/**
 * setup_log - 保存日志信息, 开启日志线程
 * @log_fname:	日志文件名字
 * @buf_size:   缓冲区大小
 * @return: 成功返回0, 失败返回-1
 */
int setup_log(char *log_fname, __u32 buf_size) 
{
    int ret;
    plog = calloc(1, sizeof(struct log));        
    plog->log_fname = log_fname;             
    plog->buf_size = buf_size;               
    calloc_buf(plog);    

    ret = pthread_attr_init(&plog->attr); 
    if (ret < 0)    
        goto LAB(return); 

    pthread_attr_setdetachstate(&plog->attr, PTHREAD_CREATE_DETACHED); 
    ret = pthread_create(&plog->tid, &plog->attr,  (void *)thread_log, plog); 
    if (ret == 0) {  
        /* 线程创建的成功与否与线程函数的执行错误并没有什么关系, 只要配置好函数就认为是线程创建成功 */
        printf("Thread write create successfully.\n");
    } else {    
        printf("Thread write can not create.\n");
    }   

LAB(return):
    return ret;
} 

/**
 * setup_log_default - 重载函数, 使用默认的缓冲区大小
 * @log_fname:	日志文件名字
 * @return: 成功返回0, 失败返回-1
 */
inline int setup_log_default(char *log_fname) 
{
    return setup_log(log_fname, DEFAULT_BUF_SIZE);       
} 

/**
 * cancel_log - 清除log申请的缓冲区
 * @return:无
 */
void cancel_log(void)
{
    if (plog->state) {
        if (pthread_kill(plog->tid, 0) == 0) {  /* 检查线程是否已被关闭 */
            pthread_cancel(plog->tid);
            pthread_attr_destroy(&plog->attr);
            plog->state = 0;
        }
    }
    free(plog->buf1); plog->buf1 = NULL; 
    free(plog->buf2); plog->buf2 = NULL; 
    free(plog->msg); plog->msg = NULL; 
    free(plog);  plog = NULL;
}


/**
 * write_buf - 向缓冲区中添加字符串, 并且在缓冲区满后切换到另一缓冲区继续保存
 * @return: 无
 */
void write_buf(void)
{      
    char *buf = get_activite_buf();     /* 获取当前活动的缓冲区指针 */
    int length_buf = strlen(buf);       /* 当前缓冲区的长度, 纯字符不含\0 */
    int length_msg = strlen(plog->msg); /* 单条日志消息的长度 */
    /* 缓冲区填充这条消息后的剩余长度, 留下一个B给\0 */
    int res_length = plog->buf_size - 1 - length_msg - length_buf; 
    
    /* 缓冲区没有被填满 */
    if (res_length > 0) {
        strncat(buf, plog->msg, length_msg);    /* 将该条消息追加到缓冲区尾部 */
    
    /* 空间不足 */
    } else {
        strncat(buf, plog->msg, plog->buf_size - 1 - length_buf);   /* 保存前一半 */
        plog->activite = change_buf_state();

        if (pthread_kill(plog->tid, SIG_WRITE) != 0)   /* 发送信号, 写进程开始将满的缓冲区内容写入文件 */
            printf("信号发送失败\n");
        /* 如果在这改变缓冲区状态, 因为线程中存在函数调用, 所以不能保证线程在执行get_activite_buf时获取的是正确的
        buf, 因此, 需要加一个延时(总归不太好), 或者将状态在发送信号之前改变, 在线程中调用不可用的buf */
        // plog->activite = change_buf_state();    
        buf = get_activite_buf();
        strncat(buf, (plog->msg + plog->buf_size - 1 - length_buf), res_length);   /* 保存后半部分 */
    }
    
    memset(plog->msg, 0, SINGLE_MSG_SIZE);
} 