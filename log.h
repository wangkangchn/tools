/***************************************************************
Copyright © wangk <wangkangchn@163.com>
文件名		: log.h
作者	  	: wangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 用户态按照不同级别打印消息, 并且存储进日志文件
时间	   	: 2020-08-12 16:42
***************************************************************/
/* ==============================================================================
 *                                  使用说明
 * 
 * 1. pr_xxx族
 *      pr_xxx族只会根据设置的级别来打印信息, 并不会将消息输出到日志
 * 
 * 2. prl_xxx族
 *      prl_xxx族不仅会根据设置的级别来打印信息, 并且将消息输出到日志
 *  Note:
 *      (1) 如果要使用prl_xxx族, 需要在导入 log.h 前先定义 CONFIG_LOG 宏
 *      例如:
 *          #define CONFIG_LOG
 *          #include "log.h"
 *      (2) prl_xxx族创建的日志文件名并非传入的日志名, 会在其上加入编号, 
 *      最终的形式为 prefix _num suffix
 * 
 * 参数说明:
 *      MODULENAME:             模块名字
 *      SINGLE_LOGFILE_SIZE:    单个日志文件的大小, 当写入文件的信息大于该值时, 
 *                              会创建新的日志文件
 * ============================================================================== */
#ifndef __CONSOLE_LOGLEVEL_H__
#define __CONSOLE_LOGLEVEL_H__
#include <stdio.h>

#include "color.h"

#ifndef MODULENAME
#define MODULENAME      "tools"
#endif

/* 不同等级的输出颜色 */
#define COLOR_FATAL     "\033[7;31m"
#define COLOR_CRITICAL  LIGHT_YELLOW
#define COLOR_ERR       LIGHT_RED
#define COLOR_WARNING   LIGHT_PURPLE
#define COLOR_INFO      LIGHT_GREEN
#define COLOR_DEBUG     LIGHT_WHITE

enum log_level {
    CONSOLE_LOGLEVEL_DISABLE = 0,       /* no log is allowed output */
    CONSOLE_LOGLEVEL_FATAL = 1,         /* only FATAL is allowed output */
    CONSOLE_LOGLEVEL_CRITICAL = 2,      /* CRITICAL/FATAL are allowed output */
    CONSOLE_LOGLEVEL_ERR = 3,           /* ERR/CRITICAL/FATAL are allowed output */
	CONSOLE_LOGLEVEL_WARNING = 4,       /* WARNING/ERR/CRITICAL/FATAL and INFO are allowed output */
    CONSOLE_LOGLEVEL_INFO = 5,          /* INFO/WARNING/ERR/CRITICAL/FATAL and INFO are allowed output */
    CONSOLE_LOGLEVEL_DEBUG = 6,         /* all log are allowed output */
};

/* the macro to set debug level, you should call it 
 * once in the files you need use debug system*/
static enum log_level default_level = CONSOLE_LOGLEVEL_DEBUG;
#define SET_DEFAULT_LEVEL(level)  (default_level = level)

#define PREFIX_FORMAT                           "[%s File: %s, Func: %s, Line: %d]"
#define SUFFIX_FORMAT(color, level, fmt)        "[\033[40m"color""level"\033[0m]: "color""fmt"\033[0m" 
#define PLACEHOLDER                             MODULENAME, __FILE__, __func__, __LINE__

/* ====================================================================================
    pr_xxx族 分等级显示消息
    ---------------------------------------------------------------------------------- */
#define pr_fatal(fmt, ...)   do {                        \
	if (default_level >= CONSOLE_LOGLEVEL_FATAL)            \
		printf(PREFIX_FORMAT SUFFIX_FORMAT(COLOR_FATAL, "FATAL", fmt), \
                PLACEHOLDER, ##__VA_ARGS__);	    \
} while (0)

#define pr_crit(fmt, ...)   do {                        \
	if (default_level >= CONSOLE_LOGLEVEL_CRITICAL)            \
		printf(PREFIX_FORMAT SUFFIX_FORMAT(COLOR_CRITICAL, "CRITICAL", fmt), \
                PLACEHOLDER, ##__VA_ARGS__);       \
} while (0)

#define pr_err(fmt, ...)   do {                        \
	if (default_level >= CONSOLE_LOGLEVEL_ERR)            \
		printf(PREFIX_FORMAT SUFFIX_FORMAT(COLOR_ERR, "ERROR", fmt), \
                PLACEHOLDER, ##__VA_ARGS__);	        \
} while (0)

#define pr_warning(fmt, ...)   do {                        \
	if (default_level >= CONSOLE_LOGLEVEL_WARNING)            \
		printf(PREFIX_FORMAT SUFFIX_FORMAT(COLOR_WARNING, "WARNING", fmt), \
                PLACEHOLDER, ##__VA_ARGS__);	        \
} while (0)

#define pr_info(fmt, ...)   do {                        \
	if (default_level >= CONSOLE_LOGLEVEL_INFO)            \
		printf(PREFIX_FORMAT SUFFIX_FORMAT(COLOR_INFO, "INFO", fmt),   \
                PLACEHOLDER, ##__VA_ARGS__);	        \
} while (0)

#define pr_debug(fmt, ...)   do {                        \
	if (default_level >= CONSOLE_LOGLEVEL_DEBUG)            \
		printf(PREFIX_FORMAT SUFFIX_FORMAT(COLOR_DEBUG, "DEBUG", fmt),   \
                PLACEHOLDER, ##__VA_ARGS__);	        \
} while (0)
/* ==================================================================================== */


/* ==========================================================================================
    prl_xxx族 分等级显示消息, 并将消息写入到日志文件
    ------------------------------------------------------------------------------------------*/
#ifdef CONFIG_LOG
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>     /* close */
#include <signal.h>
#include <asm/types.h>

#include "tools.h"

/* the macro to set debug level, you should call it 
 * once in the files you need use debug system*/
static enum log_level default_log_level = CONSOLE_LOGLEVEL_DEBUG;             /* 写入日志的最低级别 */
#define SET_DEFAULT_LOG_LEVEL(level)  (default_log_level = level)

#define SUFFIX_FORMAT_NONE(level, fmt)          "["level"]: "fmt

/* 自定义信号 */
#define SIG_WRITE             SIGUSR1     /* 将设备读取的数据写入文件 */

#define FILENAME_MAX_LENGTH   250          
#define DEFAULT_BUF_SIZE      0x1000         /* 4KB */  
#define SINGLE_MSG_SIZE       0x100         /* 单条日志的最大长度 256B */
#define SINGLE_LOGFILE_SIZE   DEFAULT_BUF_SIZE << 10    /* 最大的缓冲区文件大小, 默认写入1024次文件后创建新文件 */  

struct file_name 
{
    char *filename;         /* 文件名全名 */
    char *prefix;           /* 文件名前缀 */
    char *suffix;           /* 文件名后缀 */

    __u32 fcount;           /* 当前文件名中的编号 */
};

struct log
{   
    struct file_name *log_fname;

    pthread_t tid;          /* 设备对应的线程的线程号 */
    pthread_attr_t attr;    /* 线程属性 */

    __u8 *buf1, *buf2;
    __u8 *msg;             /* 临时保存单条日志信息 */

    __u32 buf_size;         /* 缓冲区大小 */
    __u8 state;             /* 线程状态 1线程成功创建, 2线程创建失败 */
    __u8 activite;          /* 标记正在使用的缓冲区 1 2 */
};

extern struct log *plog;

int setup_log_default(char *log_fname); 
int setup_log(char *log_fname, __u32 buf_size); 
void clearup_log(void);
void write_buf(void);

#define prl_fatal(fmt, ...)   do {  \
    pr_fatal(fmt, ##__VA_ARGS__);	    \
    if (default_log_level >= CONSOLE_LOGLEVEL_FATAL) {\
        if (plog && plog->state) { \
            snprintf(plog->msg, SINGLE_MSG_SIZE, PREFIX_FORMAT SUFFIX_FORMAT_NONE("FATAL", fmt), \
                        PLACEHOLDER, ##__VA_ARGS__);	    \
            write_buf();    \
        } \
    }   \
} while (0)

#define prl_crit(fmt, ...)   do {                           \
    pr_crit(fmt, ##__VA_ARGS__);  \
    if (default_log_level >= CONSOLE_LOGLEVEL_CRITICAL) {           \
        if (plog && plog->state) { \
            snprintf(plog->msg, SINGLE_MSG_SIZE, PREFIX_FORMAT SUFFIX_FORMAT_NONE("CRITICAL", fmt), \
                        PLACEHOLDER, ##__VA_ARGS__);       \
            write_buf();    \
        } \
    }   \
} while (0)

#define prl_err(fmt, ...)   do {                           \
	pr_err(fmt, ##__VA_ARGS__);	        \
    if (default_log_level >= CONSOLE_LOGLEVEL_ERR) {            \
        if (plog && plog->state) { \
            snprintf(plog->msg, SINGLE_MSG_SIZE, PREFIX_FORMAT SUFFIX_FORMAT_NONE("ERROR", fmt), \
                        PLACEHOLDER, ##__VA_ARGS__);	        \
            write_buf();    \
        } \
    }   \
} while (0)

#define prl_warning(fmt, ...)   do {                           \
	pr_warning(fmt, ##__VA_ARGS__);	        \
    if (default_log_level >= CONSOLE_LOGLEVEL_WARNING)            \
        if (plog && plog->state) { \
            snprintf(plog->msg, SINGLE_MSG_SIZE, PREFIX_FORMAT SUFFIX_FORMAT_NONE("WARNING", fmt), \
                        PLACEHOLDER, ##__VA_ARGS__);	        \
            write_buf();    \
        } \
    }   \
} while (0)

#define prl_info(fmt, ...)   do {                           \
	pr_info(fmt, ##__VA_ARGS__);	        \
    if (default_log_level >= CONSOLE_LOGLEVEL_INFO) {            \
        if (plog && plog->state) { \
            snprintf(plog->msg, SINGLE_MSG_SIZE, PREFIX_FORMAT SUFFIX_FORMAT_NONE("INFO", fmt),   \
                        PLACEHOLDER, ##__VA_ARGS__);	        \
            write_buf();    \
        } \
    }   \
} while (0)

#define prl_debug(fmt, ...)   do {                           \
	pr_debug(fmt, ##__VA_ARGS__);	        \
    if (default_log_level >= CONSOLE_LOGLEVEL_DEBUG) {   \
        if (plog && plog->state) { \
            snprintf(plog->msg, SINGLE_MSG_SIZE, PREFIX_FORMAT SUFFIX_FORMAT_NONE("DEBUG", fmt),   \
                        PLACEHOLDER, ##__VA_ARGS__);	        \
            write_buf();    \
        } \
    }   \
} while (0)

#endif // !CONFIG_LOG
/* ========================================================================================== */

/* 打印函数名 */
#define pr_there()  pr_debug("There is %s.\n", __func__)

#endif // !__LOG_H__