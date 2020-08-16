/***************************************************************
Copyright © wangk <wangkangchn@163.com>
文件名		: log.c
作者	  	: wangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 日志文件
时间	   	: 2020-08-12 19:44
***************************************************************/
#include <fcntl.h>      /* 读写标记 */
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "tools.h"

#ifndef CONFIG_LOG
#define CONFIG_LOG
#endif

#include "log.h"

struct log *plog = NULL;

/* 分配空间填充名字 */
#define fill_name(dst, src) do { \
    calloc_buf(dst, strlen(src)+1);   \
    strncpy(dst, src, strlen(src)); \
} while(0)


static int create_new_name(void);
void clearup_log(void);

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
 * create_file - 创建日志文件
 * @return: 成功返回正数, 失败返回-1
 */
static int create_file(void)
{
    int fd = open(plog->log_fname->filename, O_CREAT | O_TRUNC, 0644);	/* 存在就删除内容 */ 
    if (fd) {   
        // char s[32];
        // time_t t = time(0);
        // strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", localtime(&t));
        // if (write(fd, s, sizeof(s)))
        //     return -1;
        pr_debug("文件: %s 创建成功\n", plog->log_fname->filename);
        close(fd);
        return 0;
    } 
    return -1;
}

/**
 * get_file_size - 获取文件大小
 * @filename:   待获取大小的文件名字
 * @return:     成功返回文件大小, 失败返回-1
 */
static inline unsigned long get_file_size(const char *filename)  
{  
    struct stat buf;  

    return stat(filename, &buf) == 0 ? (unsigned long)buf.st_size : -1;
} 

/**
 * function - 拷贝已满的缓冲区的的内容并将其写入文件
 * @fd:     文件描述符
 * @return: 成功返回0, 失败返回-1
 */
static int buf2file(int fd)
{   
    int ret = 0;
    char *buf;
    calloc_buf(buf, plog->buf_size);  /* 先将log中满的缓冲区拷贝下来, 慢慢再向文件写 */
    char *log_buf = get_disable_buf();      /* 不可用, 即满的那个buf */
    
    /* 先保存后清空缓冲区中的内容 */
    strncpy(buf, log_buf, strlen(log_buf));
    memset(log_buf, 0, plog->buf_size);

    ret = write(fd, buf, strlen(buf));  /* 不写入\0 */

    free_buf(buf);

LAB(return):
    return ret > 0 ? 0 : -1;
}

/**
 * write_file - 向文件写入缓冲区的内容
 * @return:     成功返回0, 失败返回-1
 */
static int write_file(void)
{   
    int ret = 0;
	int fd = open(plog->log_fname->filename, O_WRONLY | O_APPEND);	
    unsigned long file_size = 0;
    
	if (fd > 0) {
        /* 拷贝缓冲区信息, 并写入文件 */
        ret = buf2file(fd);
        if (ret < 0) {
            pr_err("缓冲区写入日志文件失败\n");
            close(fd);
            goto LAB(return);
        }
        	 
        /* 检查文件大小是否超过指定的最大值, 超过则新建文件 */
        file_size = get_file_size(plog->log_fname->filename);
        IS_LESS_THEN_ZERO(file_size, "获取文件大小失败\n");
        
        if (file_size > SINGLE_LOGFILE_SIZE){
            ret = create_new_name();
            IS_LESS_THEN_ZERO(ret, "新文件名获取失败\n");

            ret = create_file();
            IS_LESS_THEN_ZERO(ret, "文件 %s 创建失败\n", plog->log_fname->filename);
        }
	} else {
        ret = fd;
        pr_err("打开文件 %s failed.\n", plog->log_fname->filename);    
    }

LAB(return):
    return ret;
}

/**
 * thread_log - 写线程函数
 * @plog:	日志结构体
 * @return: 无
 */
static void *thread_log(void) {
    int signum;
    __u32 err_num = 0;          /* 写入文件失败的次数 */
    __u32 max_err_num = 10;     /* 最大的写入文件失败的次数, 超过此次数, 则退出线程 */
    sigset_t sig;               /* 该线程所能处理的信号集 */
    sigemptyset(&sig);          /* 清空 */   
    sigaddset(&sig, SIG_WRITE);  /* 设置能处理的信号, 这个时自定义的信号 */
    pthread_sigmask(SIG_BLOCK, &sig, NULL);  

    IS_LESS_THEN_ZERO(create_file(), "文件 %s 创建失败\n", plog->log_fname->filename);

    plog->state = 1;
    for (; ;) {
        pr_debug("waitting ...\n");
        sigwait(&sig, &signum);
        pr_debug("thread_log 收到写信号, 向文件 %s 写\n", plog->log_fname->filename); 
        if (write_file() == 0) {   
            pr_debug("向文件 %s 写入完成\n", plog->log_fname->filename);
            pthread_testcancel();
        } else {
            pr_warning("第 (%d/%d) 次写入文件失败\n", ++err_num, max_err_num);
            if (err_num > max_err_num)
                break;
        }
    }

/* 释放缓冲区, 退出线程 */
LAB(return):
    plog->state = 0;
    pthread_attr_destroy(&plog->attr);
    clearup_log();
    return (void *)0;
}

/**
 * analyse_filename - 将文件名分割为前缀和后缀
 * @filename:   待解析的文件名	
 * @return:     成功返回0, 失败返回 -ENOMEM
 */
static int analyse_filename(const char *filename)
{   
    int ret = 0;
    char prefix[FILENAME_MAX_LENGTH] = {0};
    char suffix[FILENAME_MAX_LENGTH] = {0};

    /* 从后向前查找分割符的位置 */
	char *c = strrchr(filename, '.'); 

    if (c) {
        strncpy(prefix, filename, c-filename);  /* 拷贝前缀 */
	    strncpy(suffix, filename+(c-filename), strlen(filename)-(c-filename));  /* 拷贝后缀 */
    } else {
        strncpy(prefix, filename, strlen(filename));    /* 没有'.'时, 前缀即为文件名, 后缀为空 */
    }
    
    fill_name(plog->log_fname->prefix, prefix);
    fill_name(plog->log_fname->suffix, suffix);

LAB(return):
    return ret;
}

/**
 * create_new_name - 组合新名字, 保存到plog->log_fname->filename中
 *                      新文件名为: prefix _num suffix
 * @return: 成功返回0, 失败返回-1
 */
static int create_new_name(void)
{   
    int ret = 0;
    char new_name[1024];
    /* 如果存在名字先删除 */
    if (plog->log_fname->filename)
        free_buf(plog->log_fname->filename);
    snprintf(new_name, ARRAY_SIZE(new_name), "%s_%d%s", 
                            plog->log_fname->prefix, 
                            ++plog->log_fname->fcount, 
                            plog->log_fname->suffix);
    fill_name(plog->log_fname->filename, new_name); 

LAB(return):
    return ret;
}

/**
 * setup_file_name - 将文件名分割为前缀和后缀, 而后加入编号重新组合为新文件名
 *              新文件名为 prefix _num suffix
 * @filename:   文件名
 * @return:     成功返回0, 失败返回-1
 */
static int setup_file_name(const char *filename)
{   
    int ret = 0;
    
    calloc_buf(plog->log_fname, sizeof(struct file_name));  /* 分配 file_name 结构体 */

    /* 解析前后缀 */
    ret = analyse_filename(filename);
    
    /* 组合新名字 */
    if (ret == 0) {
        ret = create_new_name();
        IS_LESS_THEN_ZERO(ret,  "创建新文件名失败\n");
    } else 
        pr_err("文件名解析失败\n");
    
     
LAB(return):
    return ret;
}

/**
 * cleanup_file_name - 释放file_name结构体中分配的内存, 并释放本身
 * @fname:  待清理的结构体	
 * @return: 无
 */
static void cleanup_file_name(struct file_name *fname)
{   
    if (fname) {
        /* 清理成员 */
        free_buf(fname->filename);
        free_buf(fname->prefix);
        free_buf(fname->suffix);

        /* 释放本身 */
        free_buf(fname);
    }
}
    
/**
 * check_name_length - 验证文件名长度小于允许的最大长度250
 * @filename:	待验证的文件名字长度
 * @return:     成功返回文件名字长, 失败返回-1
 */
static inline int check_name_length(const char *filename)
{
    int ret = strlen(filename);

    if ((ret > FILENAME_MAX_LENGTH) || (ret == 0)) 
        ret = -1;

    return ret;
}

/**
 * setup_log - 保存日志信息, 开启日志线程
 * @log_fname:	日志文件名字
 * @buf_size:   缓冲区大小
 * @return: 成功返回0, 失败返回-1
 */
int setup_log(char *log_fname, __u32 buf_size) 
{
    int ret = check_name_length(log_fname);
    IS_LESS_THEN_ZERO(ret,  "日志文件名错误(最少 1 字符, 最多 %d 字符).\n", FILENAME_MAX_LENGTH);

    calloc_buf(plog, sizeof(struct log));
 
    ret = setup_file_name(log_fname);    
    IS_LESS_THEN_ZERO(ret,  "配置文件名失败\n");

    plog->buf_size = buf_size; 
    calloc_buf(plog->buf1, plog->buf_size);              
    calloc_buf(plog->buf2, plog->buf_size); 
    calloc_buf(plog->msg, SINGLE_MSG_SIZE);  

    ret = pthread_attr_init(&plog->attr); 
    IS_LESS_THEN_ZERO(ret,  "线程属性初始化失败\n");

    pthread_attr_setdetachstate(&plog->attr, PTHREAD_CREATE_DETACHED); 
    ret = pthread_create(&plog->tid, &plog->attr,  (void *)thread_log, NULL); 
    if (ret == 0) {  
        /* 线程创建的成功与否与线程函数的执行错误并没有什么关系, 只要配置好函数就认为是线程创建成功 */
        pr_debug("Thread write create successfully.\n");
    } else {    
        pr_warning("Thread write can not create.\n");
    }   

LAB(return):
    return ret < 0 ? -1 : 0;
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
 * clearup_log - 清除log申请的内存, 并释放本身
 * @return:无
 */
void clearup_log(void)
{
    if (plog) {
        if (plog->state) {
            if (pthread_kill(plog->tid, 0) == 0) {  /* 检查线程是否已被关闭 */
                pthread_cancel(plog->tid);
                pthread_attr_destroy(&plog->attr);
                plog->state = 0;
            }
        }

        cleanup_file_name(plog->log_fname);
        free_buf(plog->buf1);
        free_buf(plog->buf2); 
        free_buf(plog->msg); 
        free_buf(plog);
    }
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
            pr_debug("信号发送失败\n");
        /* 如果在这改变缓冲区状态, 因为线程中存在函数调用, 所以不能保证线程在执行get_activite_buf时获取的是正确的
        buf, 因此, 需要加一个延时(总归不太好), 或者将状态在发送信号之前改变, 在线程中调用不可用的buf */
        // plog->activite = change_buf_state();    
        buf = get_activite_buf();
        strncat(buf, (plog->msg + plog->buf_size - 1 - length_buf), res_length);   /* 保存后半部分 */
    }
    
    memset(plog->msg, 0, SINGLE_MSG_SIZE);
} 